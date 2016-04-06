/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "ftp_internal.h"




/*************************************************************************************************/
zos_result_t ftp_async_upload(ftp_context_t **context_ptr, const ftp_upload_config_t *config)
{
    zos_result_t result;
    internal_ftp_context_t *context;

    *context_ptr = NULL;

    zn_settings_set_uint32("stream.auto_close", 0);


    if(ZOS_FAILED(result, zn_malloc((uint8_t**)&context, sizeof(internal_ftp_context_t))))
    {
        return result;
    }

    memcpy(&context->config, config, sizeof(ftp_upload_config_t));
    context->id = FTP_CONTEXT_ID;
    context->timestamp = zn_rtos_get_time();
    context->data_handle = ZOS_INVALID_HANDLE;
    context->control_handle = ZOS_INVALID_HANDLE;
    context->state = FTP_STATE_INIT;

    FTP_DEBUG("Connecting to %s:%d", config->host, config->port);
    if(ZOS_FAILED(result, zn_tcp_connect(ZOS_WLAN, config->host, config->port, &context->control_handle)))
    {
        context->id = 0;
        free(context);
    }
    else
    {
        ZOS_LOG("Connected: handle=%d", context->control_handle);
        zn_event_register_periodic(ftp_processing_handler, (void*)context, FTP_PROCESSING_PERIOD, EVENT_FLAGS1(RUN_NOW));
        *context_ptr = (ftp_context_t*)context;
    }

    return result;
}

/*************************************************************************************************/
zos_result_t ftp_async_upload_write(ftp_context_t *_context, const void *data, uint16_t length)
{
    zos_result_t result;
    internal_ftp_context_t *context = (internal_ftp_context_t*)_context;

    if(!ftp_context_is_valid(context))
    {
        result = ZOS_NULL_PTR;
    }
    else if(data == NULL)
    {
        // flush any remaining data
        uint8_t *dummy;
        if(ZOS_FAILED(result, zn_tcp_write(context->data_handle, &dummy, 0, ZOS_FALSE)))
        {
            FTP_DEBUG("Failed to flush file data: %d", result);
            context->state = FTP_STATE_FAIL;
        }
        else
        {
            context->state = FTP_STATE_QUIT;
        }
    }
    else
    {

        if(ZOS_FAILED(result, zn_tcp_write(context->data_handle, data, length, ZOS_TRUE)))
        {
            FTP_DEBUG("Failed to write file data: %d", result);
            context->state = FTP_STATE_FAIL;
        }
    }

    return result;
}

/*************************************************************************************************/
void ftp_async_upload_cleanup(ftp_context_t *_context)
{
    internal_ftp_context_t *context = (internal_ftp_context_t*)_context;

    FTP_DEBUG("Cleaning up context");

    zn_event_unregister_all(ftp_processing_handler);

    if(ftp_context_is_valid(context))
    {
        if(context->data_handle != ZOS_INVALID_HANDLE)
        {
            FTP_DEBUG("Disconnecting data socket");
            zn_tcp_disconnect(context->data_handle);
        }

        if(context->control_handle != ZOS_INVALID_HANDLE)
        {
            FTP_DEBUG("Disconnecting control socket");
            send_ftp_command(context, "QUIT");
            zn_rtos_delay_milliseconds(100);
            zn_tcp_disconnect(context->control_handle);
        }

        context->id = 0;
        free(context);
    }

    internal_context = NULL;
}



/*************************************************************************************************/
const char* ftp_get_state_str(ftp_state_t state)
{
    static const char* const ftp_state_str[FTP_STATE_COUNT] =
    {
            [FTP_STATE_IDLE]    = "IDLE",
            [FTP_STATE_INIT]    = "INIT",
            [FTP_STATE_OPEN]    = "OPEN",
            [FTP_STATE_LOGIN]   = "LGIN",
            [FTP_STATE_USER]    = "USER",
            [FTP_STATE_PASS]    = "PASS",
            [FTP_STATE_CWD]     = "CWD",
            [FTP_STATE_TYPE]    = "TYPE",
            [FTP_STATE_LISTEN]  = "LSTN",
            [FTP_STATE_ASK]     = "ASK",
            [FTP_STATE_STOR]    = "STOR",
            [FTP_STATE_DATA]    = "DATA",
            [FTP_STATE_QUIT]    = "QUIT",
            [FTP_STATE_CLOSE]   = "CLOS",
            [FTP_STATE_END]     = "END",
            [FTP_STATE_FAIL]    = "FAIL"
    };

    return (state < FTP_STATE_COUNT) ? ftp_state_str[state] : "UNKNOWN";
}
