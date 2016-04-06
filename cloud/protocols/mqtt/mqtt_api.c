/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include "zos_types.h"
#include "mqtt_api.h"
#include "mqtt_manager.h"
#include "mqtt_internal.h"
#include "string.h"

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

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               extern Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

static mqtt_session_t session;

/******************************************************
 *               Function Definitions
 ******************************************************/


zos_result_t mqtt_init( mqtt_connection_t* mqtt_connection )
{
    mqtt_connection->session_init = ZOS_TRUE;
    return ZOS_SUCCESS;
}

zos_result_t mqtt_deinit( mqtt_connection_t* mqtt_connection )
{
    mqtt_network_close( mqtt_connection );
    mqtt_connection->session_init = ZOS_FALSE;
    return ZOS_SUCCESS;
}

zos_result_t mqtt_open( mqtt_connection_t* mqtt_connection, const char *address, uint16_t port_number, zos_interface_t interface, mqtt_callback_t callback, zos_bool_t security )
{
    if ( port_number == 0 )
    {
        port_number = ( security == ZOS_FALSE ) ? MQTT_CONNECTION_DEFAULT_PORT : MQTT_CONNECTION_SECURE_PORT;
    }

    mqtt_connection->callback = callback;
    return mqtt_network_open( address, port_number, interface, mqtt_connection, &mqtt_connection->socket, security );
}

zos_result_t mqtt_connect( mqtt_connection_t* mqtt_connection, mqtt_pkt_connect_t *conninfo )
{
    mqtt_event_message_t current_event;
    mqtt_connect_arg_t args;

    memset( &args, 0, sizeof(mqtt_connect_arg_t) );

    args.clean_session = conninfo->clean_session;
    args.will_flag = 0;
    if ( conninfo->username != NULL )
    {
        args.username_flag = 1;
        args.username.str = conninfo->username;
        args.username.len = (uint16_t) strlen( (char*) conninfo->username );
    }
    else
    {
        args.username_flag = 0;
    }

    if ( conninfo->password != NULL )
    {
        args.password_flag = 1;
        args.password.str = conninfo->password;
        args.password.len = (uint16_t) strlen( (char*) conninfo->password );
    }
    else
    {
        args.password_flag = 0;
    }

    args.client_id.str = conninfo->client_id;
    args.client_id.len = (uint16_t) strlen( (char*) conninfo->client_id );
    args.keep_alive = conninfo->keep_alive; /*in seconds */
    args.mqtt_version = conninfo->mqtt_version;

    if ( args.clean_session == 1 )
    {
        mqtt_session_init( &session );
    }
    else
    {
        if ( mqtt_connection->session_init == ZOS_TRUE )
        {
            mqtt_session_init( &session );
        }
        mqtt_connection->session_init = ZOS_FALSE;
    }
    mqtt_connection->session = &session;

    current_event.send_context.event_t = MQTT_EVENT_SEND_CONNECT;
    current_event.send_context.conn = mqtt_connection;
    current_event.send_context.args.conn_args = args;
    current_event.event_type = MQTT_SEND_EVENT;

    return mqtt_manager( MQTT_EVENT_SEND_CONNECT, &current_event.send_context.args, current_event.send_context.conn );
}

zos_result_t mqtt_disconnect( mqtt_connection_t* mqtt_connection )
{
    mqtt_event_message_t current_event;

    current_event.send_context.event_t = MQTT_EVENT_SEND_DISCONNECT;
    current_event.send_context.conn = mqtt_connection;
    current_event.event_type = MQTT_SEND_EVENT;

    return mqtt_manager( MQTT_EVENT_SEND_DISCONNECT, &current_event.send_context.args, current_event.send_context.conn );
}

mqtt_msgid_t mqtt_subscribe( mqtt_connection_t* mqtt_connection, uint8_t *topic, uint8_t qos )
{
    mqtt_event_message_t current_event;
    mqtt_subscribe_arg_t args;

    args.topic_filter.str = topic;
    args.topic_filter.len = (uint16_t) strlen( (char*) topic );
    args.qos = qos;
    args.packet_id = ( ++mqtt_connection->packet_id );

    current_event.send_context.event_t = MQTT_EVENT_SEND_SUBSCRIBE;
    current_event.send_context.conn = mqtt_connection;
    current_event.send_context.args.sub_args = args;
    current_event.event_type = MQTT_SEND_EVENT;

    if ( mqtt_manager( MQTT_EVENT_SEND_SUBSCRIBE, &current_event.send_context.args, current_event.send_context.conn) != ZOS_SUCCESS )
    {
        return 0;
    }
    return args.packet_id;
}

mqtt_msgid_t mqtt_unsubscribe( mqtt_connection_t* mqtt_connection, uint8_t *topic )
{
    mqtt_event_message_t current_event;
    mqtt_unsubscribe_arg_t args;

    args.topic_filter.str = topic;
    args.topic_filter.len = (uint16_t) ( strlen( (char*) topic ) );
    args.packet_id = ( ++mqtt_connection->packet_id );

    current_event.send_context.event_t = MQTT_EVENT_SEND_UNSUBSCRIBE;
    current_event.send_context.conn = mqtt_connection;
    current_event.send_context.args.unsub_args = args;
    current_event.event_type = MQTT_SEND_EVENT;

    if ( mqtt_manager( MQTT_EVENT_SEND_UNSUBSCRIBE, &current_event.send_context.args, current_event.send_context.conn ) != ZOS_SUCCESS )
    {
        return 0;
    }
    return args.packet_id;
}

mqtt_msgid_t mqtt_publish( mqtt_connection_t* mqtt_connection, uint8_t *topic, uint8_t *data, uint32_t data_len, uint8_t qos )
{
    mqtt_event_message_t current_event;
    mqtt_publish_arg_t args;
    args.topic.str = topic;
    args.topic.len = (uint16_t) strlen( (char*) topic );
    args.data = data;
    args.data_len = data_len;
    args.dup = 0;
    args.qos = qos;
    args.retain = 0;
    args.packet_id = ( ++mqtt_connection->packet_id );

    current_event.send_context.event_t = MQTT_EVENT_SEND_PUBLISH;
    current_event.send_context.conn = mqtt_connection;
    current_event.send_context.args.pub_args = args;
    current_event.event_type = MQTT_SEND_EVENT;

    if ( mqtt_manager( MQTT_EVENT_SEND_PUBLISH, &current_event.send_context.args, current_event.send_context.conn ) != ZOS_SUCCESS )
    {
        return 0;
    }
    return args.packet_id;
}
