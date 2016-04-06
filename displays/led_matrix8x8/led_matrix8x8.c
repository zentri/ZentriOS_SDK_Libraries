/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "led_matrix8x8.h"
#include "ht16k33.h"
#include "displays/util/fonts/font8x8.h"

#include "matrix8x8_operations.c"

#define DISPLAY_BUFFER_LEN (sizeof(uint16_t)*8)


//#define DEBUG_LED_MATRIX
#ifdef DEBUG_LED_MATRIX
#define LED_DEBUG ZOS_LOG
#else
#define print_display_buffer(x)
#define LED_DEBUG(x, ...)
#endif


static led_matrix8x8_context_t context;



/*************************************************************************************************/
zos_result_t led_matrix8x8_init(zos_i2c_t i2c_port)
{
    zos_result_t result;

    context.blink_rate = HT16K33_BLINKRATE_OFF;
    context.brightness = Ht16K33_BRIGHTNESS_MAX;
    context.scroll.rate = 150;

    if(ZOS_FAILED(result, ht16k33_init(i2c_port, &context.device)))
    {
    }
    else if(ZOS_FAILED(result, led_matrix8x8_clear_display()))
    {
    }

    return result;
}

/*************************************************************************************************/
void led_matrix8x8_deinit(void)
{
    reset_context();
    led_matrix8x8_clear_display();
    ht16k33_deinit(&context.device);

    if(context.display_buffer != NULL)
    {
        zn_free(context.display_buffer);
    }
}

/*************************************************************************************************/
const led_matrix8x8_context_t* led_matrix8x8_get_context(void)
{
    return &context;
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_set_brightness(uint8_t brightness)
{
    if (brightness > Ht16K33_BRIGHTNESS_MAX)
    {
        brightness = Ht16K33_BRIGHTNESS_MAX;
    }
    context.brightness = brightness;
    return ht16k33_set_brightness(&context.device, brightness);
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_set_blink_rate(uint8_t rate)
{
    if (rate > HT16K33_BLINKRATE_HALFHZ)
    {
        rate = HT16K33_BLINKRATE_OFF;
    }
    context.blink_rate = rate;
    return ht16k33_set_blink_rate(&context.device, rate);
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_set_operation(led_matrix8x8_operation_t operation)
{
    context.operation = operation;
    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_set_scroll_rate(uint16_t rate_ms)
{
    context.scroll.rate = rate_ms;
    if(zn_event_is_registered(update_display, NULL))
    {
        return zn_event_update_periodic(update_display, NULL, rate_ms, 0);
    }
    else
    {
        return ZOS_SUCCESS;
    }
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_set_text(const char *text)
{
#define TEXT_BUFFER_LEN (text_len*8)
    zos_result_t result;

    reset_context();

    uint8_t *ptr;
    const uint16_t text_len = strlen(text)+1;

    if(!ZOS_FAILED(result, zn_malloc(&ptr, DISPLAY_BUFFER_LEN + TEXT_BUFFER_LEN + text_len)))
    {
        context.display_buffer = (uint16_t*)ptr;
        context.text_buffer = ptr + DISPLAY_BUFFER_LEN;
        context.text = (char*)(ptr + DISPLAY_BUFFER_LEN + TEXT_BUFFER_LEN);
        strcpy(context.text, text);

        uint8_t *text_buffer_ptr = context.text_buffer;
        for(int i = 0; i < text_len; ++i, text_buffer_ptr += FONT_8x8_ROW_LEN)
        {
            const uint8_t char_offset = (uint8_t)(text[i] == 0) ? 0 : (uint8_t)(text[i] - 32);
            if(ZOS_FAILED(result, zn_load_ro_memory(text_buffer_ptr, FONT_8x8_ROW_LEN, &font8x8, char_offset*FONT_8x8_ROW_LEN)))
            {
                goto exit;
            }
        }

        context.scroll.position = 0;
        context.scroll.column = 0;
        context.scroll.end = text_len;
        update_display(NULL);
        zn_event_register_periodic(update_display, NULL, context.scroll.rate, EVENT_FLAGS2(RUN_NOW, SYS_EVENT_THREAD));
    }

    exit:
    return result;
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_get_text(const char **text)
{
    if(context.text == NULL)
    {
        return ZOS_ERROR;
    }
    else
    {
        *text = context.text;
        return ZOS_SUCCESS;
    }
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_update_display(const uint8_t *display_buffer)
{
    zn_event_unregister(update_display, NULL);
    cleanup_text_context();
    ZOS_VERIFY(alloc_display_buffer());
    print_display_buffer(display_buffer);

    return draw_display(display_buffer);
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_clear_display(void)
{
    return ht16k33_clear_display(&context.device);
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_animate(led_matrix8x8_animate_context_t *animate_context, void (*populate_event_handler)(void *arg), uint8_t queue_size)
{
    zos_result_t result;

    reset_context();

    if(ZOS_FAILED(result, alloc_display_buffer()))
    {
        LED_DEBUG("Failed to alloc display buffer");
    }
    else if(ZOS_FAILED(result, zn_rtos_queue_init(&animate_context->frame_queue, sizeof(led_matrix8x8_frame_t), queue_size)))
    {
        LED_DEBUG("Failed to init frame queue");
    }
    else
    {
        animate_context->populate_event_handler = populate_event_handler;
        context.animate_context = animate_context;
        if(populate_event_handler != NULL)
        {
            animate_context->populate_event_handler(&animate_context->frame_queue);
        }

        display_next_frame();
    }

    return result;
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_flush_frames(void)
{
    if(context.animate_context != NULL)
    {
        return zn_rtos_queue_flush(&context.animate_context->frame_queue);
    }
    else
    {
        return ZOS_NULL_PTR;
    }
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_push_frame(const led_matrix8x8_frame_t *frame, uint32_t timeout_ms)
{
    if(context.animate_context != NULL)
    {
        zos_result_t result;

        LED_DEBUG("Pushing frame: %d", frame->display_time);
        print_display_buffer(frame->data);
        if(ZOS_FAILED(result, zn_rtos_queue_push(&context.animate_context->frame_queue, (void*)frame, timeout_ms)))
        {
            LED_DEBUG("Failed to push to queue: %d", result);
        }
        else if(!zn_event_is_registered(update_display, NULL))
        {
            LED_DEBUG("Display timer not running, starting now");
            display_next_frame();
        }

        return result;
    }
    else
    {
        return ZOS_NULL_PTR;
    }
}

/*************************************************************************************************/
zos_result_t led_matrix8x8_get_current_frame(led_matrix8x8_frame_t *frame_buffer)
{
    if(context.animate_context != NULL)
    {
        memcpy(frame_buffer, &context.animate_context->current_frame, sizeof(led_matrix8x8_frame_t));
        return ZOS_SUCCESS;
    }
    else
    {
        return ZOS_NULL_PTR;
    }
}


/*************************************************************************************************/
static zos_result_t draw_display(const uint8_t *display_buffer)
{
    uint16_t *ptr = context.display_buffer;

    for(int i = 0; i < FONT_8x8_ROW_LEN; ++i)
    {
        uint32_t b = (uint32_t)display_buffer[i];
        // reverse the bits
        b = __RBIT(b);
        ptr[i] = b >> 24;
    }
    perform_operation(context.display_buffer);

    return ht16k33_write_display(&context.device, context.display_buffer);
}

/*************************************************************************************************/
static void update_display(void *arg)
{
    UNUSED_PARAMETER(arg);

    if(context.animate_context != NULL)
    {
        display_next_frame();
    }
    else
    {
        const uint8_t *text_buffer_ptr = &context.text_buffer[context.scroll.position*FONT_8x8_ROW_LEN];
        uint16_t *display_buffer_ptr = context.display_buffer;
        const uint8_t column = context.scroll.column;
        for(int i = 0; i < FONT_8x8_ROW_LEN; ++i)
        {
            uint32_t b = (uint32_t)((text_buffer_ptr[i] << column) | ((text_buffer_ptr + FONT_8x8_ROW_LEN)[i] >> (FONT_8x8_ROW_LEN - column)));

            // reverse the bits
            b = __RBIT(b);
            display_buffer_ptr[i] = b >> 24;
        }
        perform_operation(context.display_buffer);

        ht16k33_write_display(&context.device, context.display_buffer);

        ++context.scroll.column;
        if(context.scroll.column >= 8)
        {
            context.scroll.column = 0;
            ++context.scroll.position;
        }
        if(context.scroll.position+1 >= context.scroll.end)
        {
            context.scroll.position = 0;
        }
    }
}

/*************************************************************************************************/
static void display_next_frame(void)
{
    led_matrix8x8_frame_t frame;

    if(zn_rtos_queue_pop(&context.animate_context->frame_queue, &frame, ZOS_NO_WAIT) == ZOS_SUCCESS)
    {
        draw_display(frame.data);
        memcpy(&context.animate_context->current_frame, &frame, sizeof(led_matrix8x8_frame_t));
        if(frame.display_time != 0)
        {
            if(!zn_event_is_registered(update_display, NULL))
            {
                zn_event_register_periodic(update_display, NULL, frame.display_time, EVENT_FLAGS1(SYS_EVENT_THREAD));
            }
            else
            {
                zn_event_update_periodic(update_display, NULL, frame.display_time, EVENT_FLAGS1(SYS_EVENT_THREAD));
            }
        }
    }
    else
    {
        zn_event_unregister(update_display, NULL);
    }

    if(context.animate_context->populate_event_handler != NULL)
    {
        zn_event_issue(context.animate_context->populate_event_handler, NULL, 0);
    }
}

/*************************************************************************************************/
static zos_result_t alloc_display_buffer(void)
{
    if(context.display_buffer == NULL)
    {
        zos_result_t result;

        if(ZOS_FAILED(result, zn_malloc((uint8_t**)&context.display_buffer, DISPLAY_BUFFER_LEN)))
        {
            return result;
        }
    }
    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static void reset_context(void)
{
    zn_event_unregister(update_display, NULL);
    clean_animate_context();
    cleanup_text_context();
}

/*************************************************************************************************/
static void cleanup_text_context(void)
{
    if(context.text != NULL)
    {
        context.text = NULL;
        context.text_buffer = NULL;
        if(context.display_buffer != NULL)
        {
            zn_free(context.display_buffer);
            context.display_buffer = NULL;
        }
    }
}

/*************************************************************************************************/
static void clean_animate_context(void)
{
    if(context.animate_context != NULL)
    {
        zn_rtos_queue_deinit(&context.animate_context->frame_queue);
        context.animate_context = NULL;
    }
}

/*************************************************************************************************/
static void perform_operation(uint16_t *buffer)
{
    if(context.operation != LED_MATRIX_NO_OPERATION)
    {
        uint64_t x;
        uint8_t *ptr = (uint8_t*)&x;

        for(int i = 0; i < sizeof(uint64_t); ++i)
        {
            *ptr++ = (uint8_t)buffer[i];
        }

        switch(context.operation)
        {
        case LED_MATRIX_ROTATE90_CLOCKWISE:
            x = rotate90clockwise(x);
            break;
        case LED_MATRIX_ROTATE90_ANTICLOCKWISE:
            x = rotate90antiClockwise(x);
            break;
        default:
            break;
        }

        ptr = (uint8_t*)&x;
        for(int i = 0; i < sizeof(uint64_t); ++i)
        {
            buffer[i] = (uint16_t)*ptr++;
        }
    }
}

/*************************************************************************************************/
#ifdef DEBUG_LED_MATRIX
static void print_display_buffer(const uint8_t *buffer)
{
    char line[9*8];
    char *line_ptr = line;

    for(int i = 0; i < 8; ++i, ++buffer)
    {
        const uint8_t row = (uint8_t)*buffer;
        for(int j = (1 << 7); j > 0; j >>= 1)
        {
            *line_ptr++ = (row & j) ? '#' : '.';
        }
        *line_ptr++ = '\n';
    }
    *(line_ptr-1) = 0;
    ZOS_LOG("%s", line);
}
#endif
