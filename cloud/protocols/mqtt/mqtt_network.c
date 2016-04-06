/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *  I/O functions
 *  Provides functions for sending and receiving to the network for use by
 *  framing layer
 */

#include "mqtt_frame.h"
#include "mqtt_network.h"
#include "mqtt_connection.h"
#include "mqtt_manager.h"
#include "mqtt_wrapper/mqtt_zentrios_wrapper.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef struct mqtt_network_message_s
{
    uint8_t *data;
} mqtt_network_message_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/
static void mqtt_receive_handler( uint32_t handle );
static void mqtt_disconnect_handler( uint32_t handle );

/******************************************************
 *               Variable Definitions
 ******************************************************/
// ToDo: global variable? remove!!
mqtt_connection_t *gConn;
/******************************************************
 *               Function Definitions
 ******************************************************/
zos_result_t mqtt_network_open( const char *server_ip_address, uint16_t portnumber, zos_interface_t interface, void *p_user, mqtt_socket_t *socket, zos_bool_t security )
{
    zos_result_t result = ZOS_SUCCESS;
    mqtt_connection_t *conn = (mqtt_connection_t *)p_user;
    gConn = conn;

    if(!mqtt_network_is_up(ZOS_WLAN))
    {
        return ZOS_ERROR;
    }

    memcpy(socket->server_ip_address, server_ip_address, strlen(server_ip_address));
    socket->portnumber = portnumber;

    if(security == ZOS_FALSE)
    {
        result = mqtt_tcp_connect(ZOS_WLAN, socket->server_ip_address, socket->portnumber, &socket->socket_handle);
        if(result != ZOS_SUCCESS)
        {
            MQTT_LOG("Failed to open connection (error %d)", result);
            goto ERROR_CREATE_SOCKET;
        }
    }
    else
    {
        result = mqtt_tls_connect(ZOS_WLAN, socket->server_ip_address, socket->portnumber, &socket->socket_handle);
        if(result != ZOS_SUCCESS)
        {
            MQTT_LOG("Failed to open secure connection (error %d)", result);
            goto ERROR_CREATE_SOCKET;
        }
    }

    mqtt_tcp_register_client_event_handlers(socket->socket_handle, mqtt_disconnect_handler, mqtt_receive_handler);
    conn->net_init_ok = ZOS_TRUE;
    return result;

ERROR_CREATE_SOCKET:

    return result;
}

zos_result_t mqtt_network_close( void *p_user )
{
    mqtt_connection_t *conn = (mqtt_connection_t *)p_user;
    if(conn->net_init_ok != ZOS_TRUE)
    {
        // Already deinitialized before. Do nothing
        return ZOS_SUCCESS;
    }

    // MQTT_LOG("Server disconnected, attempting to read any remaining data");
    // mqtt_network_receive_buffer(conn->socket.socket_handle);

    MQTT_LOG("Closing TCP connection (socket %u)", conn->socket.socket_handle);
    mqtt_tcp_disconnect(conn->socket.socket_handle);

    conn->net_init_ok = ZOS_FALSE;
    conn->socket.socket_handle = 0;
    return ZOS_SUCCESS;
}

zos_result_t mqtt_network_create_buffer( mqtt_buffer_t *buffer, uint16_t size, mqtt_socket_t *socket )
{
    /* Create the TCP packet. Memory for the tx_data is automatically allocated */
    mqtt_malloc(&buffer->data, size);
    if ( buffer->data == NULL )
    {
        MQTT_LOG("Don't have memory to allocate for buffer...\n");
        return ZOS_NO_MEM;
    }

    return ZOS_SUCCESS;
}

zos_result_t mqtt_network_delete_buffer( uint8_t *data )
{
    /* Delete frame data allocated by mqtt_network_create_buffer */
    return mqtt_free( data );
}

zos_result_t mqtt_network_send_buffer( uint8_t *data, uint32_t size, mqtt_socket_t *socket )
{
    return mqtt_tcp_write(socket->socket_handle, data, size, ZOS_FALSE);
}

zos_result_t mqtt_network_receive_buffer(uint32_t socket_handle, void *p_user)
{
    zos_result_t result;
    uint32_t bytes_read;
    mqtt_buffer_t buffer;
    mqtt_connection_t *conn = (mqtt_connection_t *)p_user;

    mqtt_malloc(&buffer.data, MQTT_CONNECTION_FRAME_MAX);

    result = mqtt_tcp_read(socket_handle, buffer.data, MQTT_CONNECTION_FRAME_MAX, &bytes_read);
    if(result != ZOS_SUCCESS)
    {
        MQTT_LOG("Failed to read data (error %d)", result);
    }
    else if(bytes_read == 0)
    {
        result = ZOS_NO_DATA;
    }
    else
    {
        mqtt_frame_recv( &buffer, conn );
    }

    mqtt_free( buffer.data );
    return result;
}
static void mqtt_disconnect_handler( uint32_t handle )
{
    MQTT_LOG("Disconnect handler invoked (socket %d)", handle);
    mqtt_network_close( gConn );
}

static void mqtt_receive_handler( uint32_t handle )
{
    //MQTT_LOG("Receive handler invoked: socket %d", handle);
    mqtt_network_receive_buffer(handle, gConn);
}

