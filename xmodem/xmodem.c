/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "xmodem.h"

//#define XMODEM_DEBUG
#ifdef XMODEM_DEBUG
#define  DEBUG_XMODEM ZOS_LOG
#else
#define  DEBUG_XMODEM(...)
#endif

#define MAX_ERROR_COUNT 8
#define IDLE_TIMEOUT 1000
#define PACKET_TIMEOUT 3000

#define XMODEM_PACKET_SIZE_1K 1024
#define XMODEM_PACKET_SIZE_LEGACY 128

// Communication control characters
#define XMODEM_SOH 0x01
#define XMODEM_STX 0x02
#define XMODEM_ETX 0x03
#define XMODEM_EOT 0x04
#define XMODEM_ENQ 0x05
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_ETB 0x17
#define XMODEM_CAN 0x18
#define XMODEM_EOF 0x1A

#define XMODEM_STX2K 0x20
#define XMODEM_STX4K 0x21
#define XMODEM_STX8K 0x22

typedef enum
{
    STATE_READ_SOH,
    STATE_READ_BLK_NUM,
    STATE_READ_NBLK_NUM,
    STATE_READ_PACKET,
    STATE_READ_CRC,
    STATE_READ_PACKET_VALIDATE
} xmodem_rx_state_t;



typedef struct
{
    xmodem_config_t config;
    zos_buffer_t *buffer;
    uint8_t *ptr;
    uint16_t packet_len;
    uint16_t rx_bytes_remaining;
    zos_uart_callback_t saved_uart_callback;
    xmodem_rx_state_t rx_state;
    uint8_t retries;
    uint8_t blk_num;
    uint8_t nblk_num;
    uint8_t crc;
    uint32_t timestamp;
    zos_bool_t halted;
    uint8_t can_count;
    zos_bool_t block_received;
} xmodem_context_t;


static xmodem_context_t context;


/*************************************************************************************************/
zos_result_t xmodem_init(const xmodem_config_t *config)
{
    zn_event_enable_irq_events(3);
    memcpy(&context.config, config, sizeof(xmodem_config_t));

    if(context.config.idle_period == 0)
    {
        context.config.idle_period = IDLE_TIMEOUT;
    }

    context.saved_uart_callback = zn_uart_register_rx_callback(context.config.uart, uart_rx_callback);
    zn_uart_register_rx_callback(context.config.uart, context.saved_uart_callback );

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t xmodem_read_start(void)
{
    DEBUG_XMODEM("Xmodem RX started");
    zn_cmd_set_console_enabled(ZOS_FALSE);
    zn_event_register_periodic(xmodem_read_event_handler, NULL, MIN(context.config.idle_period, 100), 0);
    zn_uart_register_rx_callback(context.config.uart, uart_rx_callback);
    context.retries = 0;
    context.halted = ZOS_FALSE;
    context.block_received = ZOS_FALSE;
    reset_rx_context(ZOS_TRUE);
    send_byte(XMODEM_NAK);

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t xmodem_read_halt(void)
{
    context.halted = ZOS_TRUE;
    reset_rx_context(ZOS_TRUE);
    send_byte(XMODEM_CAN);
    zn_uart_register_rx_callback(context.config.uart, context.saved_uart_callback);

    return ZOS_TRUE;
}

/*************************************************************************************************/
zos_result_t xmodem_read_stop(void)
{
    DEBUG_XMODEM("Xmodem RX stop");

    if(context.rx_state != STATE_READ_SOH)
    {
        send_byte(XMODEM_CAN);
    }

    zn_event_unregister_all(xmodem_read_event_handler);
    zn_uart_register_rx_callback(context.config.uart, context.saved_uart_callback);
    zn_cmd_set_console_enabled(ZOS_TRUE);
    free_buffer();

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t xmodem_write(void *data, uint32_t length, zos_bool_t eot)
{
    return ZOS_UNIMPLEMENTED;
}

/*************************************************************************************************/
static void uart_rx_callback(uint32_t unused)
{
    const uint8_t *data;
    uint16_t length;
    zn_uart_peek_bytes(context.config.uart, &data, &length);

    if(length == 0)
    {
        return;
    }
    else if(context.rx_state == STATE_READ_PACKET)
    {
        if(length >= MIN(context.rx_bytes_remaining, 128))
        {
            zn_event_issue(xmodem_read_event_handler, NULL, EVENT_FLAGS1(FROM_IRQ));
        }
    }
    else
    {
        static uint32_t timestamp = 0;
        const uint32_t now = zn_rtos_get_time();

        // event generated from an interrupt have higher priority than normal events
        // so make sure we don't call this too often as it'll starve other events from executing
        if((now - timestamp) > 50)
        {
            timestamp = now;
            zn_event_issue(xmodem_read_event_handler, NULL, EVENT_FLAGS1(FROM_IRQ));
        }
    }

}


/*************************************************************************************************
 * Executes when a UART RX char is received
 */
static void xmodem_read_event_handler(void *arg)
{
    uint8_t rx_value;

    loop:
    if(!read_next_value(&rx_value))
    {
        check_timeout();
        return;
    }

    switch(context.rx_state)
    {
    case STATE_READ_SOH:
        if(context.halted)
        {
            send_cancel_sequence();
        }
        else if(rx_value == XMODEM_SOH ||
                rx_value == XMODEM_STX ||
                rx_value == XMODEM_STX2K ||
                rx_value == XMODEM_STX4K ||
                rx_value == XMODEM_STX8K)
        {
            //DEBUG_XMODEM("STATE_READ_SOH: STX");
            context.rx_state = STATE_READ_BLK_NUM;
            context.packet_len = (rx_value == XMODEM_STX8K) ? 8092 :
                                 (rx_value == XMODEM_STX4K) ? 4096 :
                                 (rx_value == XMODEM_STX2K) ? 2048 :
                                 (rx_value == XMODEM_STX)   ? 1024 : 128;
            context.block_received = ZOS_FALSE;
            goto loop;
        }
        else if(rx_value == XMODEM_CAN)
        {
            DEBUG_XMODEM("STATE_READ_SOH: CAN");
            ++context.can_count;
            context.block_received = ZOS_FALSE;

            if(context.can_count > 1)
            {
                // Host cancel ?
                context.config.callback.read_error(ZOS_ABORTED);
                reset_rx_context(ZOS_TRUE);
            }
        }
        else if(rx_value == XMODEM_EOT)
        {
            if(context.block_received)
            {
                //DEBUG_XMODEM("STATE_READ_SOH: EOT");
                send_byte(XMODEM_ACK);

                context.config.callback.block_read(NULL);
                reset_rx_context(ZOS_FALSE);
                goto loop;
            }
            else
            {
                send_byte(XMODEM_NAK);
            }
            context.block_received = ZOS_FALSE;
        }
        else
        {
            context.block_received = ZOS_FALSE;
            send_byte(XMODEM_NAK);
        }
        break;

    case STATE_READ_BLK_NUM:
        //DEBUG_XMODEM("STATE_READ_BLK_NUM: 0x%02X", rx_value);
        context.blk_num = rx_value;
        context.rx_state = STATE_READ_NBLK_NUM;
        goto loop;

    case STATE_READ_NBLK_NUM:
        context.nblk_num = rx_value;
        //DEBUG_XMODEM("STATE_READ_NBLK_NUM: 0x%02X", rx_value);
        if(allocate_buffer())
        {
            context.rx_state = STATE_READ_PACKET;
            goto loop;
        }
        else
        {
            context.rx_state = STATE_READ_SOH;
            send_cancel_sequence();
        }
        break;

    case STATE_READ_PACKET:
        if(context.rx_bytes_remaining == 0)
        {
            //DEBUG_XMODEM("STATE_READ_PACKET: read");
            context.rx_state = STATE_READ_CRC;
            goto loop;
        }
        break;

    case STATE_READ_CRC:
        //DEBUG_XMODEM("STATE_READ_CRC: 0x%02X", rx_value);
        context.crc = rx_value;
        context.rx_state = STATE_READ_PACKET_VALIDATE;
        goto loop;

    case STATE_READ_PACKET_VALIDATE:
        validate_packet();
        goto loop;
    }
}

/*************************************************************************************************/
static zos_bool_t read_next_value(uint8_t *char_ptr)
{
    if(context.rx_state == STATE_READ_PACKET_VALIDATE)
    {
        return ZOS_TRUE;
    }
    else if(context.rx_state != STATE_READ_PACKET)
    {
        if(zn_uart_receive_bytes(context.config.uart, char_ptr, 1, ZOS_NO_WAIT) == ZOS_SUCCESS)
        {
            context.timestamp = zn_rtos_get_time();
            return ZOS_TRUE;
        }
    }
    else
    {
        const uint8_t *data;
        uint16_t length;
        zn_uart_peek_bytes(context.config.uart, &data, &length);

        const uint16_t read_length = MIN(length, context.rx_bytes_remaining);
        if(read_length > 0 && zn_uart_receive_bytes(context.config.uart, context.ptr, read_length, ZOS_NO_WAIT) == ZOS_SUCCESS)
        {
            context.ptr += read_length;
            context.rx_bytes_remaining -= read_length;
            //DEBUG_XMODEM("STATE_READ_PACKET: %d read, %d remaining", read_length, context.rx_bytes_remaining);
            context.timestamp = zn_rtos_get_time();
            return ZOS_TRUE;
        }
    }

    return ZOS_FALSE;
}

/*************************************************************************************************/
static void validate_packet(void)
{
    const uint8_t calc_crc = calculate_crc();

    if(context.blk_num != (uint8_t)~context.nblk_num || (calc_crc != context.crc))
    {
        ++context.retries;

        if(context.blk_num != (uint8_t)~context.nblk_num)
        {
            DEBUG_XMODEM("STATE_READ_PACKET_VALIDATE: blk num invalid");
        }
        else
        {
            DEBUG_XMODEM("STATE_READ_PACKET_VALIDATE: CRC invalid: 0x%02X != 0x%02X", calc_crc, context.crc);
        }

        if(context.retries > MAX_ERROR_COUNT)
        {
            context.retries = 0;
            context.config.callback.read_error(ZOS_ERROR);
            send_cancel_sequence();
            reset_rx_context(ZOS_TRUE);
        }
        else
        {
            reset_rx_context(ZOS_TRUE);
            send_byte(XMODEM_NAK);
        }
    }
    else
    {
        send_byte(XMODEM_ACK);
        context.retries = 0;
        context.block_received = ZOS_TRUE;
        context.config.callback.block_read(context.buffer);
        free_buffer();


        //DEBUG_XMODEM("STATE_READ_PACKET_VALIDATE: packet read");

        reset_rx_context(ZOS_FALSE);
    }
}

/*************************************************************************************************/
static void check_timeout(void)
{
    const uint32_t now = zn_rtos_get_time();

    if(context.rx_state == STATE_READ_SOH)
    {
        if(!context.halted && (now - context.timestamp) >= context.config.idle_period)
        {
            send_byte(XMODEM_NAK);
            context.timestamp = now;
        }
    }
    else
    {
        if((now - context.timestamp) > PACKET_TIMEOUT)
        {
            DEBUG_XMODEM("PACKET_TIMEOUT");
            reset_rx_context(ZOS_TRUE);
        }
    }
}

/*************************************************************************************************/
static void reset_rx_context(zos_bool_t flush_input_buffer)
{
    if(flush_input_buffer)
    {
        uint8_t temp[256];
        while(zn_uart_receive_bytes(context.config.uart, temp, sizeof(temp), 25) == ZOS_SUCCESS)
        {
        }
    }

    context.can_count = 0;
    context.rx_state = STATE_READ_SOH;
}

/*************************************************************************************************/
static uint8_t calculate_crc(void)
{
    zos_buffer_t *buf = context.buffer;
    const uint8_t *end_buffer = &buf->data[context.packet_len];
    uint8_t cal_crc = 0;

#ifdef XMODEM_DEBUG
    //zn_dump_buffer(context.buffer, context.packet_len, "Data:", ZOS_DUMP_FLAGS(16, 1, LITTLE, ADD_SPACE, PRINT_ADDRESSES, PRINT_ASCII));
#endif

    for(const uint8_t *ptr = buf->data; ptr < end_buffer; ++ptr)
    {
        cal_crc += *ptr;
    }

    return cal_crc;
}

/*************************************************************************************************/
static void send_byte(uint8_t c)
{
    zn_uart_transmit_bytes(context.config.uart, &c, 1);
}

/*************************************************************************************************/
static zos_bool_t allocate_buffer(void)
{
    free_buffer();

    zos_buffer_t *buf = zn_malloc_ptr(sizeof(zos_buffer_t) + context.packet_len);
    if(buf != NULL)
    {
        buf->data = (uint8_t*)&buf[1];
        buf->size = context.packet_len;
        context.buffer = buf;
        context.ptr = buf->data;
        context.rx_bytes_remaining = context.packet_len;

        return ZOS_TRUE;
    }
    else
    {
        return ZOS_FALSE;
    }
}

/*************************************************************************************************/
static void free_buffer(void)
{
    context.rx_bytes_remaining = 0;
    context.ptr = NULL;

    if(context.buffer != NULL)
    {
        zn_free(context.buffer);
        context.buffer = NULL;
    }
}

/*************************************************************************************************/
static void send_cancel_sequence(void)
{
    static uint32_t timestamp = 0;
    const uint32_t now = zn_rtos_get_time();

    context.block_received = ZOS_FALSE;

    // ensure we don't send the cancel sequence too often
    if((now - timestamp) > 100)
    {
        timestamp = now;
        send_byte(XMODEM_CAN);
        send_byte(XMODEM_CAN);
    }
}
