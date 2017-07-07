// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothub_mqtt_internal.h"
#include "azure_c_shared_utility/xio.h"



static void tls_stream_disconnected_handler(uint32_t handle);
static void tls_stream_received_handler(uint32_t handle);



/*************************************************************************************************/
XIO_HANDLE xio_create(const IO_INTERFACE_DESCRIPTION* io_interface_description, const void* xio_create_parameters)
{
    connection_info.stream_handle = ZOS_INVALID_HANDLE;
    return NULL;
}

/*************************************************************************************************/
void xio_destroy(XIO_HANDLE xio)
{
    if(connection_info.stream_handle != ZOS_INVALID_HANDLE)
    {
        zn_tcp_disconnect(connection_info.stream_handle);
        connection_info.stream_handle = ZOS_INVALID_HANDLE;
    }
}

/*************************************************************************************************/
int xio_open(XIO_HANDLE xio, ON_IO_OPEN_COMPLETE on_io_open_complete,
             void* on_io_open_complete_context,
             ON_BYTES_RECEIVED on_bytes_received,
             void* on_bytes_received_context,
             ON_IO_ERROR on_io_error,
             void* on_io_error_context)
{
    int result;
    zos_result_t zos_result;

    xio_destroy(NULL);

    if(ZOS_FAILED(zos_result, zn_tls_connect(ZOS_WLAN, connection_info.hostname, connection_info.port, &connection_info.stream_handle)))
    {
        LogError("Failed to open TLS connection, err:%d", zos_result);
        result = __FAILURE__;
        on_io_open_complete(on_io_open_complete_context, IO_OPEN_ERROR);
    }
    else
    {
        result = 0;
        zn_tcp_register_client_event_handlers(connection_info.stream_handle, tls_stream_disconnected_handler, tls_stream_received_handler);
        connection_info.rx_callback = on_bytes_received;
        connection_info.rx_callback_context = on_bytes_received_context;
        connection_info.error_callback = on_io_error;
        connection_info.error_callback_context = on_io_error_context;
        on_io_open_complete(on_io_open_complete_context, IO_OPEN_OK);
    }

    return result;
}

/*************************************************************************************************/
int xio_close(XIO_HANDLE xio, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result;

    if(connection_info.stream_handle == ZOS_INVALID_HANDLE)
    {
        result = __FAILURE__;
    }
    else
    {
        zn_tcp_disconnect(connection_info.stream_handle);
        connection_info.stream_handle =  ZOS_INVALID_HANDLE;
        on_io_close_complete(callback_context);
        result = 0;
    }

    return result;
}

/*************************************************************************************************/
int xio_send(XIO_HANDLE xio, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;
    zos_result_t zos_result;

    if(connection_info.stream_handle == ZOS_INVALID_HANDLE)
    {
        result = __FAILURE__;
    }
    else
    {
        result = 0;

        if(ZOS_FAILED(zos_result, zn_tcp_write(connection_info.stream_handle, buffer, size, ZOS_FALSE)))
        {
            on_send_complete(callback_context, IO_SEND_ERROR);
        }
        else
        {
            on_send_complete(callback_context, IO_SEND_OK);
        }
    }

    return result;
}

/*************************************************************************************************/
void xio_dowork(XIO_HANDLE xio)
{
    if(connection_info.stream_handle != ZOS_INVALID_HANDLE)
    {
        uint32_t unused;
        // if data is available then the RX event will be invoked
        zn_tcp_poll(connection_info.stream_handle, &unused);
    }
}

/*************************************************************************************************/
int xio_setoption(XIO_HANDLE xio, const char* optionName, const void* value)
{
    return 0;
}

/** --------------------------------------------------------------------------------------------
 *  Internal functions
 * -------------------------------------------------------------------------------------------- **/

/*************************************************************************************************/
static void tls_stream_disconnected_handler(uint32_t handle)
{
    if(connection_info.stream_handle != ZOS_INVALID_HANDLE)
    {
        connection_info.error_callback(connection_info.error_callback_context);
    }
}

/*************************************************************************************************/
static void tls_stream_received_handler(uint32_t handle)
{
    zos_result_t result;
    zos_buffer_t buffer =
    {
            .size = UINT16_MAX
    };

    if(ZOS_FAILED(result, zn_tcp_read_with_buffer(connection_info.stream_handle, &buffer)))
    {
    }
    else
    {
        connection_info.rx_callback(connection_info.rx_callback_context, buffer.data, buffer.size);
    }
}
