// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothub_mqtt_internal.h"
#include "azure_c_shared_utility/xio.h"


/*************************************************************************************************/
XIO_HANDLE xio_create(const IO_INTERFACE_DESCRIPTION* io_interface_description, const void* xio_create_parameters)
{
    connection_info.stream_handle = ZOS_INVALID_HANDLE;
    return NULL;
}

/*************************************************************************************************/
void xio_destroy(XIO_HANDLE xio)
{
    connection_info.stream_handle = ZOS_INVALID_HANDLE;
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

    if(ZOS_FAILED(zos_result, zn_tls_connect(ZOS_WLAN, connection_info.hostname, connection_info.port, &connection_info.stream_handle)))
    {
        LogError("Failed to open TLS connection, err:%d", zos_result);
        result = __FAILURE__;
        on_io_open_complete(on_io_open_complete_context, IO_OPEN_ERROR);
    }
    else
    {
        result = 0;
        zn_tcp_register_client_event_handlers(connection_info.stream_handle, )
        connection_info.rx_callback = on_bytes_received;
        connection_info.rx_callback_context = on_bytes_received_context;
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

    /* Codes_SRS_XIO_01_011: [No error check shall be performed on buffer and size.] */
    /* Codes_SRS_XIO_01_010: [If handle is NULL, xio_send shall return a non-zero value.] */
    if (xio == NULL)
    {
        result = __FAILURE__;
    }
    else
    {
        XIO_INSTANCE* xio_instance = (XIO_INSTANCE*)xio;

        /* Codes_SRS_XIO_01_008: [xio_send shall pass the sequence of bytes pointed to by buffer to the concrete IO implementation specified in xio_create, by calling the concrete_io_send function while passing down the buffer and size arguments to it.] */
        /* Codes_SRS_XIO_01_009: [On success, xio_send shall return 0.] */
        /* Codes_SRS_XIO_01_015: [If the underlying concrete_io_send fails, xio_send shall return a non-zero value.] */
        /* Codes_SRS_XIO_01_027: [xio_send shall pass to the concrete_io_send function the on_send_complete and callback_context arguments.] */
        result = xio_instance->io_interface_description->concrete_io_send(xio_instance->concrete_xio_handle, buffer, size, on_send_complete, callback_context);
    }

    return result;
}

/*************************************************************************************************/
void xio_dowork(XIO_HANDLE xio)
{
}

/*************************************************************************************************/
int xio_setoption(XIO_HANDLE xio, const char* optionName, const void* value)
{
    return 0;
}


