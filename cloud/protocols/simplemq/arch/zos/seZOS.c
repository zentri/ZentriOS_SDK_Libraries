/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include <selib.h>


//#define SMQ_ZOS_DEBUG_ENABLED
#ifdef SMQ_ZOS_DEBUG_ENABLED
#define SMQ_ZOS_DEBUG ZOS_LOG
#else
#define SMQ_ZOS_DEBUG(x, ...)
#endif




static SOCKET* sockets[MAX_STREAM_COUNT];



/*************************************************************************************************/
int se_connect(SOCKET* sock, const char* address, U16 port)
{
    zos_result_t result = zn_tcp_connect(ZOS_WLAN, address, port, &sock->handle);
    SMQ_ZOS_DEBUG("connect: %d, %d", result, sock->handle);
    return (result == ZOS_SUCCESS) ? 0 : -3;
}

/*************************************************************************************************/
void se_close(SOCKET* sock)
{
    sockets[sock->handle] = NULL;
    SMQ_ZOS_DEBUG("diconnect: %d", sock->handle);
    zn_tcp_disconnect(sock->handle);
}

/*************************************************************************************************/
S32 se_send(SOCKET* sock, const void* buf, U32 len)
{
#ifdef SMQ_ZOS_DEBUG_ENABLED
    zn_dump_buffer(buf, len, "tx", ZOS_DUMP_FLAGS(16, 1, BIG, ADD_SPACE, PRINT_ADDRESSES, PRINT_ASCII));
#endif
    zos_result_t result = zn_tcp_write(sock->handle, buf, len, ZOS_FALSE);
    SMQ_ZOS_DEBUG("send: %d, %d",  sock->handle, len);
    return (result == ZOS_SUCCESS) ? 0 : -1;
}

/*************************************************************************************************/
S32 se_recv(SOCKET* sock, void* buf, U32 len, U32 timeout)
{
    S32 retval = 0;
    const uint32_t start_time = zn_rtos_get_time();

    do
    {
        uint32_t bytes_read = 0;
        zos_result_t result = zn_tcp_read(sock->handle, buf, len, &bytes_read);
        if(result != ZOS_SUCCESS)
        {
            retval = -1;
        }
        else
        {
            retval = bytes_read;
        }
    } while((zn_rtos_get_time() - start_time) < timeout && retval == 0);

    SMQ_ZOS_DEBUG("recv: %d, %d", retval, sock->handle);
    return retval;
}

/*************************************************************************************************/
void SMQ_register_message_handler(SOCKET* sock, zos_event_handler_t message_handler)
{
    sock->message_handler = message_handler;
    sockets[sock->handle] = sock;
    zn_tcp_register_client_event_handlers(sock->handle, disconnect_event_handler, receive_event_handler);
}

/*************************************************************************************************/
static void disconnect_event_handler(uint32_t handle)
{
    SOCKET* sock = sockets[handle];
    sockets[handle] = NULL;

    if(sock != NULL && sock->message_handler != NULL)
    {
        sock->message_handler(sock);
    }
}

/*************************************************************************************************/
static void receive_event_handler(uint32_t handle)
{
    SOCKET* sock = sockets[handle];

    SMQ_ZOS_DEBUG("rx event");

    if(sock != NULL && sock->message_handler != NULL)
    {
        sock->message_handler(sock);
    }
}
