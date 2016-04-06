/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "ftp_internal.h"


// some FTP reply codes   kinda cheating here I'm just looking at the first char of
// the return code  but it works.

#define  RX_INVALID     0
#define  RX_OK          '2'     // any 2xx code is generally OK
#define  RX_USER_OK     '3'     // actually code 331
#define  RX_PASSIVE     '7'     // code 227 = Passive mode engaged
#define  RX_ERROR       '5'     // any 5xx code is generally bad JUJU
#define  XFER_COMPLETE  '6'     //  226 = file done
#define  XFER_OPEN      '1'   // actually is code 125



// FIXME this makes this now thread-safe!
internal_ftp_context_t *internal_context;



/*************************************************************************************************/
void ftp_processing_handler(void * arg)
{
    internal_ftp_context_t *context = (internal_ftp_context_t*)arg;

    internal_context = context;

    loop:
    if(!ftp_context_is_valid(context))
    {
        FTP_DEBUG("Invalid context!");
        ftp_async_upload_cleanup(NULL);
        return;
    }

    if(context->state != context->old_state)
    {
        FTP_DEBUG("State: %s, RX_status: %d", ftp_get_state_str(context->state), context->rx_status);
        context->old_state = context->state;
        context->timestamp = zn_rtos_get_time();  // reset the timeout

        if(context->config.state_callback_callback != NULL)
        {
            if(context->config.state_callback_callback(context->state) != ZOS_SUCCESS)
            {
                return;
            }
        }
    }

    switch(context->state)
    {
    case FTP_STATE_IDLE:
        // do nothing here
        break;

    case FTP_STATE_INIT:
        ++context->state;
        break;

    case FTP_STATE_OPEN:
        // once open, register the RX data handler
        zn_tcp_register_client_event_handlers(context->control_handle, NULL, ftp_receive_event_handler);
        ++context->state;
        break;

    case FTP_STATE_LOGIN:

        /* Read the welcome message from the server */
        if (context->rx_status == RX_OK )
        {
            // now send the USER command
            send_ftp_command(context, "USER %s", context->config.username);
        }
        break;

    case FTP_STATE_USER:
        // read the USER command response
        if (context->rx_status == RX_USER_OK)
        {
            // now send the PASS command
            send_ftp_command(context, "PASS %s", context->config.password);
        }
        break;

    case FTP_STATE_PASS:
        // read the PASS command response
        if(context->rx_status == RX_OK)
        {
            // skip to need state if no directory
            if(context->config.directory == NULL)
            {
                ++context->state;
            }
            else
            {
                // send CWD command
                send_ftp_command(context, "CWD %s", context->config.directory);
            }
        }
        break;

    case FTP_STATE_CWD:
        // read the result of the CWD command
        if(context->rx_status == RX_OK)
        {
            // send TYPE command
            send_ftp_command(context, "TYPE I");
        }

        break;

    case FTP_STATE_TYPE:
        // read the result of the TYPE command
        if (context->rx_status == RX_OK)
        {
            send_ftp_command(context, "PASV");
        }
        break;

    case FTP_STATE_LISTEN:

        // wait here until PAssive port comes back
        if (context->passive_port != 0  )
        {
            zos_result_t result;

            FTP_DEBUG("Connect out to FTP port=%d", context->passive_port);

            if(ZOS_FAILED(result, zn_tcp_connect(ZOS_WLAN, context->config.host, context->passive_port, &context->data_handle)))
            {
                FTP_DEBUG("Failed to connect: %d", result);
                ++context->state;
            }
            else
            {
                FTP_DEBUG("DATA PORT CONNECTED handle: %d", context->data_handle);
                ++context->state;
            }
        }  // 2nd connection  create
        break;

    case FTP_STATE_ASK:
        send_ftp_command(context, "STOR %s", context->config.filename);
        break;

    case FTP_STATE_STOR:
        // wait until the stream is open
        if ( context->rx_status == XFER_OPEN )
        {
            ++context->state;
        }
        break;

    case FTP_STATE_DATA:
        // at this point ftp_async_upload_write() should be used to write data
        // once all the data is written, call ftp_async_upload_write() with a NULL buffer
        context->timestamp = zn_rtos_get_time();
        break;

    case FTP_STATE_QUIT:
        ZOS_LOG("FTP QUIT");
        zn_tcp_disconnect(context->data_handle);
        context->data_handle = ZOS_INVALID_HANDLE;
        send_ftp_command(context, "QUIT");
        break;

    case FTP_STATE_CLOSE:
        ZOS_LOG("FTP CLOSE");
        // wait for response to QUIT command
        if (context->rx_status == RX_OK)
        {
            zn_tcp_disconnect(context->control_handle);
            context->control_handle = ZOS_INVALID_HANDLE;
            ++context->state;
        }
        break;

    case FTP_STATE_END:
        ZOS_LOG("FTP END");
        ftp_async_upload_cleanup((void*)context);
        return;

    case FTP_STATE_FAIL:
    default:
        ZOS_LOG("FTP FAIL");
        ftp_async_upload_cleanup((void*)context);
        return;
    }

    // return to beginning if the state changed while processing
    if(context->state != context->old_state)
    {
        goto loop;
    }

    // check if the connection has been idle too long
    if((zn_rtos_get_time() - context->timestamp) > context->config.timeout)
    {
        ZOS_LOG("FTP Timeout! (%d)", context->config.timeout);
        ftp_async_upload_cleanup((void*)context);
    }

}

/*************************************************************************************************/
zos_bool_t ftp_context_is_valid(internal_ftp_context_t *context)
{
    return (context != NULL && context->id == FTP_CONTEXT_ID);
}

/*************************************************************************************************/
void send_ftp_command(internal_ftp_context_t *context, const char*fmt, ...)
{
    zos_result_t result;
    char buffer[128];
    va_list args;

    va_start(args, fmt);
    int len = vsnprintf(buffer, sizeof(buffer)-3, fmt, args);
    va_end(args);
    FTP_DEBUG("SEND ---> %s", buffer);

    buffer[len++] = '\r';
    buffer[len++] = '\n';

    if(ZOS_FAILED(result, zn_tcp_write(context->control_handle, buffer, len, ZOS_FALSE)))
    {
        context->state = FTP_STATE_FAIL;
        FTP_DEBUG("Failed to send command");
    }
    else
    {
        context->rx_status = RX_INVALID;
        ++context->state;
    }
}

/*************************************************************************************************/
void ftp_receive_event_handler(uint32_t handle)
{
    zos_result_t result;
    uint32_t rx_read;

    if(!ftp_context_is_valid(internal_context))
    {
        return;
    }
    else if(ZOS_FAILED(result, zn_tcp_read(handle,
                                           internal_context->rx_buffer,
                                           sizeof(internal_context->rx_buffer),
                                           &rx_read)))
    {
        ZOS_LOG("Failed to receive data: %d", result);
        internal_context->state = FTP_STATE_FAIL;
        return;
    }

    internal_context->timestamp = zn_rtos_get_time();  // reset the timeout

    char *rx_buffer = (char*)internal_context->rx_buffer;
    rx_buffer[rx_read] = 0;

    FTP_DEBUG("RECV <-- %s", str_strip(rx_buffer,"\r\n"));


    internal_context->rx_status = (uint8_t)rx_buffer[0];

    if ( internal_context->rx_status == RX_ERROR )
    {
        internal_context->state = FTP_STATE_FAIL;
        return;
    }

    // special case of PASSIVE port coming back
    if ( rx_buffer[2] == RX_PASSIVE )
    {

        int i,j;
        i=20;j=0;

        while ( ( ++i< rx_read ) && ( j < 4 ) )
            if (rx_buffer[i] == ',' ) j++;

        internal_context->passive_port =  ( atoi(&rx_buffer[i]) << 8 );


        j=0;
        while ( ( ++i< rx_read ) && ( j < 1 ) )
            if (rx_buffer[i] == ',' ) j++;

        j=i;
        while (  ++j< rx_read )
            if (rx_buffer[j] == ')' )
            {
                rx_buffer[j]=0;
                j=rx_read;  // done looking here.
            }


        internal_context->passive_port = internal_context->passive_port + atoi( &rx_buffer[i]);
    }

}
