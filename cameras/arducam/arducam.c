/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "arducam.h"
#include "drivers/arducam_drivers.h"


#define IMAGE_READY_POLL_PERIOD 5



typedef struct
{
    const arducam_driver_t *driver;
    zos_spi_device_t spi_device;
    zos_i2c_device_t i2c_device;
    uint16_t buffer_length;
    uint8_t *buffer1;
    uint8_t *buffer2;
    uint8_t *current_buffer;
    arducam_error_handler_t error_handler;

    arducam_callbacks_t callback;

    struct
    {
        zos_bool_t is_capturing;
        zos_bool_t is_image_ready;
    } flag;

    uint32_t image_data_remaining;
} arducam_context_t;


static arducam_context_t *context;



/*************************************************************************************************/
zos_result_t arducam_init(const arducam_config_t *config, arducam_type_t type)
{
    zos_result_t result;
    const arducam_driver_t *driver;

    arducam_deinit();

    if(ZOS_FAILED(result, arducam_get_driver(type, &driver)))
    {
        return result;
    }

    context = zn_malloc_ptr(sizeof(arducam_context_t) + config->max_read_length*2);
    if(context == NULL)
    {
        return ZOS_NO_MEM;
    }

    context->driver = driver;
    context->buffer1 = (uint8_t*)&context[1];
    context->buffer2 = context->buffer1 + config->max_read_length;
    context->buffer_length = config->max_read_length;
    memcpy(&context->callback, &config->callback, sizeof(arducam_callbacks_t));

    if(ZOS_FAILED(result, arducam_driver_init(&context->spi_device, &context->i2c_device, driver, config)))
    {
    }

    if(result != ZOS_SUCCESS)
    {
        arducam_deinit();
    }

    return result;
}

/*************************************************************************************************/
zos_result_t arducam_deinit(void)
{
    if(context != NULL)
    {
        reset_capture_context();
        zn_free(context);
        context = NULL;
    }

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t arducam_start_capture(void)
{
    zos_result_t result;

    if(context->flag.is_capturing)
    {
        return ZOS_SUCCESS;
    }

    reset_capture_context();

    if(ZOS_FAILED(result, context->driver->callback.start_capture()))
    {
    }
    else
    {
        context->flag.is_capturing = ZOS_TRUE;
        zn_event_register_timed(poll_image_status_handler, NULL, IMAGE_READY_POLL_PERIOD, 0);
    }

    return result;
}

/*************************************************************************************************/
zos_bool_t arducam_is_image_ready(void)
{
    return context->flag.is_image_ready;
}

/*************************************************************************************************/
zos_result_t arducam_start_reading_image(void)
{
    zos_result_t result;

    if(!context->flag.is_image_ready)
    {
        result = ZOS_NO_DATA;
    }
    else if(ZOS_FAILED(result, arducam_driver_spi_burst_read_start()))
    {
        arducam_abort_capture();

        context->callback.error_handler(result);
    }
    else
    {
        zn_event_issue(read_image_data_handler, NULL, 0);
    }

    return result;
}

/*************************************************************************************************/
zos_result_t arducam_abort_capture(void)
{
    reset_capture_context();
    return context->driver->callback.stop_capture();
}

/*************************************************************************************************/
zos_result_t arducam_set_setting(arducam_setting_type_t setting, int32_t value)
{
    return context->driver->callback.set_setting(setting, value);
}

/*************************************************************************************************/
zos_result_t arducam_get_setting(arducam_setting_type_t setting, int32_t *value_ptr)
{
    return context->driver->callback.get_setting(setting, value_ptr);
}


/*************************************************************************************************/
static void poll_image_status_handler(void *arg)
{
    zos_result_t result;

    if(ZOS_FAILED(result, context->driver->callback.is_capture_ready(&context->image_data_remaining)))
    {
        if(result == ZOS_PENDING)
        {
            zn_event_register_timed(poll_image_status_handler, NULL, IMAGE_READY_POLL_PERIOD, 0);
        }
        else
        {
            arducam_abort_capture();

            context->callback.error_handler(result);
        }
    }
    else
    {
        context->flag.is_image_ready = ZOS_TRUE;
        if(context->callback.image_ready(context->image_data_remaining))
        {
            arducam_start_reading_image();
        }
    }
}

/*************************************************************************************************/
static void write_image_data_handler(void *arg)
{
    zos_result_t result;
    const uint32_t chunk_size = (uint32_t)arg;
    const void *buffer = context->current_buffer;

    context->image_data_remaining -= chunk_size;

    if(context->image_data_remaining > 0)
    {
        zn_event_issue(read_image_data_handler, NULL, 0);
    }

    result = context->callback.data_writer(buffer, chunk_size, (context->image_data_remaining == 0));
    if((result != ZOS_SUCCESS) || (context->image_data_remaining == 0))
    {
        reset_capture_context();
    }
}

/*************************************************************************************************/
static void read_image_data_handler(void *arg)
{
    zos_result_t result;
    const uint32_t chunk_size = MIN(context->image_data_remaining, context->buffer_length);

    context->current_buffer = (context->current_buffer == context->buffer1) ? context->buffer2 : context->buffer1;

    if(ZOS_FAILED(result, context->driver->callback.read_data(context->current_buffer, chunk_size)))
    {
        context->callback.error_handler(result);
        reset_capture_context();
    }
    else
    {
        zn_event_issue(write_image_data_handler, (void*)chunk_size, EVENT_FLAGS1(NETWORK_THREAD));
    }
}

/*************************************************************************************************/
static void reset_capture_context(void)
{
    arducam_driver_spi_burst_read_stop();
    zn_event_unregister(poll_image_status_handler, NULL);
    zn_event_unregister(read_image_data_handler, NULL);
    zn_event_unregister(write_image_data_handler, NULL);
    context->flag.is_capturing = ZOS_FALSE;
    context->flag.is_image_ready = ZOS_FALSE;
    context->image_data_remaining = 0;
}
