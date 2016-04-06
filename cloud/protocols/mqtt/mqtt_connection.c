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
 *  Connection functions
 *
 *  Provides connection methods for use by applications
 */

#include "zos_types.h"
#include "mqtt_internal.h"
#include "mqtt_connection.h"
#include "mqtt_frame.h"
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

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/******************************************************
 *      Backend functions called from mqtt_queue
 ******************************************************/
zos_result_t mqtt_backend_put_connect( const mqtt_connect_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_frame_t frame;
    mqtt_connect_arg_t final_args;
    memcpy(&final_args, args, sizeof(mqtt_connect_arg_t));

    /* Fixing connect args based on specification */
    if ( final_args.username_flag == ZOS_FALSE )
    {
        /* make sure password flag is 0 as well */
        final_args.password_flag = ZOS_FALSE;
    }

    if ( final_args.will_flag == ZOS_FALSE )
    {
        /* make sure will_retain and will_qos are 0 */
        final_args.will_retain = ZOS_FALSE;
        final_args.will_qos = MQTT_QOS_DELIVER_AT_MOST_ONCE;
    }

    /* Send Protocol Header */
    ret = mqtt_frame_create( MQTT_CONNECTION_FRAME_MAX, &frame, &conn->socket );
    if ( ret != ZOS_SUCCESS )
    {
        return ret;
    }

    mqtt_frame_put_connect( &frame, &final_args );

    MQTT_LOG("Send CONNECT frame");
    ret = mqtt_frame_send( &frame, &conn->socket );

    /* delete packet as soon as sent */
    mqtt_frame_delete(&frame);

    return ret;
}

zos_result_t mqtt_backend_get_connack( mqtt_connack_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret = ZOS_SUCCESS;
    UNUSED_PARAMETER(args);
    if ( ret == ZOS_SUCCESS )
    {
        /* Publish is an async method (we don't get an OK), so we simulate the OK after sending it */

        if ( conn->callback != NULL )
        {

            mqtt_event_info_t event;
            event.type = MQTT_EVENT_TYPE_CONNECTED;
            event.data.err_code = args->return_code;
            conn->callback( &event );
        }
    }
    return ret;
}

zos_result_t mqtt_backend_put_publish( const mqtt_publish_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_frame_t frame;
    mqtt_publish_arg_t final_args;
    memcpy(&final_args, args, sizeof(mqtt_publish_arg_t));

    /* Send Protocol Header */
    ret = mqtt_frame_create( MQTT_CONNECTION_FRAME_MAX, &frame, &conn->socket );
    if ( ret != ZOS_SUCCESS )
    {
        return ret;
    }
    mqtt_frame_put_publish( &frame, &final_args );

    MQTT_LOG("Send PUBLISH frame");
    ret = mqtt_frame_send( &frame, &conn->socket );

    /* delete packet as soon as sent */
    mqtt_frame_delete(&frame);

    return ret;
}

zos_result_t mqtt_backend_get_publish( mqtt_publish_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_event_info_t event;
    ret = mqtt_manager( MQTT_EVENT_RECV_PUBLISH, args, conn );
    if ( ( ret == ZOS_SUCCESS ) && ( args->data != NULL ) )
    {
        /* Publish is an async method (we don't get an OK), so we simulate the OK after sending it */
        if ( conn->callback != NULL )
        {
            event.type = MQTT_EVENT_TYPE_PUBLISH_MSG_RECEIVED;
            event.data.pub_recvd.topic = args->topic.str;
            event.data.pub_recvd.topic_len = args->topic.len;
            event.data.pub_recvd.data = args->data;
            event.data.pub_recvd.data_len = args->data_len;
            conn->callback( &event );
        }
    }
    return ret;
}

zos_result_t mqtt_backend_put_puback( const mqtt_puback_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_frame_t frame;

    /* Send Protocol Header */
    ret = mqtt_frame_create( MQTT_CONNECTION_FRAME_MAX, &frame, &conn->socket );
    if ( ret != ZOS_SUCCESS )
    {
        return ret;
    }
    mqtt_frame_put_puback( &frame, args );

    MQTT_LOG("Send PUBACK frame");
    ret = mqtt_frame_send( &frame, &conn->socket );

    /* delete packet as soon as sent */
    mqtt_frame_delete(&frame);

    return ret;
}

zos_result_t mqtt_backend_get_puback( mqtt_puback_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    ret = mqtt_manager( MQTT_EVENT_RECV_PUBACK, args, conn );
    if ( ret == ZOS_SUCCESS )
    {
        if ( conn->callback != NULL )
        {
            mqtt_event_info_t event;
            event.type = MQTT_EVENT_TYPE_PUBLISHED;
            event.data.msgid = args->packet_id;
            conn->callback( &event );
        }
    }
    return ret;
}

zos_result_t mqtt_backend_put_pubrec( const mqtt_pubrec_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_frame_t frame;

    /* Send Protocol Header */
    ret = mqtt_frame_create( MQTT_CONNECTION_FRAME_MAX, &frame, &conn->socket );
    if ( ret != ZOS_SUCCESS )
    {
        return ret;
    }
    mqtt_frame_put_pubrec( &frame, args );

    MQTT_LOG("Send PUBREC frame");
    ret = mqtt_frame_send( &frame, &conn->socket );

    /* delete packet as soon as sent */
    mqtt_frame_delete(&frame);

    return ret;
}

zos_result_t mqtt_backend_get_pubrec( mqtt_pubrec_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    ret = mqtt_manager( MQTT_EVENT_RECV_PUBREC, args, conn );
    if ( ret == ZOS_SUCCESS )
    {
        /* Publish is an async method (we don't get an OK), so we simulate the OK after sending it */
        if ( conn->callback != NULL )
        {
            mqtt_event_info_t event;
            event.type = MQTT_EVENT_TYPE_UNKNOWN;
            event.data.msgid = args->packet_id;
            conn->callback( &event );
        }

    }
    return ret;
}

zos_result_t mqtt_backend_put_pubrel( const mqtt_pubrel_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_frame_t frame;

    /* Send Protocol Header */
    ret = mqtt_frame_create( MQTT_CONNECTION_FRAME_MAX, &frame, &conn->socket );
    if ( ret != ZOS_SUCCESS )
    {
        return ret;
    }
    mqtt_frame_put_pubrel( &frame, args );

    MQTT_LOG("Send PUBREL frame");
    ret = mqtt_frame_send( &frame, &conn->socket );

    /* delete packet as soon as sent */
    mqtt_frame_delete(&frame);

    return ret;
}

zos_result_t mqtt_backend_get_pubrel( mqtt_pubrel_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    ret = mqtt_manager( MQTT_EVENT_RECV_PUBREL, args, conn );
    if ( ret == ZOS_SUCCESS )
    {
        if ( conn->callback != NULL )
        {
            mqtt_event_info_t event;
            event.type = MQTT_EVENT_TYPE_UNKNOWN;
            event.data.msgid = args->packet_id;
            conn->callback( &event );
        }

    }
    return ret;
}

zos_result_t mqtt_backend_put_pubcomp( const mqtt_pubcomp_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_frame_t frame;

    /* Send Protocol Header */
    ret = mqtt_frame_create( MQTT_CONNECTION_FRAME_MAX, &frame, &conn->socket );
    if ( ret != ZOS_SUCCESS )
    {
        return ret;
    }
    mqtt_frame_put_pubcomp( &frame, args );

    MQTT_LOG("Send PUBCOMP frame");
    ret = mqtt_frame_send( &frame, &conn->socket );

    /* delete packet as soon as sent */
    mqtt_frame_delete(&frame);

    return ret;
}

zos_result_t mqtt_backend_get_pubcomp( mqtt_pubcomp_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    ret = mqtt_manager( MQTT_EVENT_RECV_PUBCOMP, args, conn );
    if ( ret == ZOS_SUCCESS )
    {
        /* Publish is an async method (we don't get an OK), so we simulate the OK after sending it */
        if ( conn->callback != NULL )
        {
            mqtt_event_info_t event;
            event.type = MQTT_EVENT_TYPE_PUBLISHED;
            event.data.msgid = args->packet_id;
            conn->callback( &event );
        }

    }
    return ret;
}

zos_result_t mqtt_backend_put_subscribe( const mqtt_subscribe_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_frame_t frame;
    mqtt_subscribe_arg_t final_args;
    memcpy(&final_args, args, sizeof(mqtt_subscribe_arg_t));

    /* Send Protocol Header */
    ret = mqtt_frame_create( MQTT_CONNECTION_FRAME_MAX, &frame, &conn->socket );
    if ( ret != ZOS_SUCCESS )
    {
        return ret;
    }
    mqtt_frame_put_subscribe( &frame, &final_args );

    MQTT_LOG("Send SUBSCRIBE frame");
    ret = mqtt_frame_send( &frame, &conn->socket );

    /* delete packet as soon as sent */
    mqtt_frame_delete(&frame);

    return ret;
}

zos_result_t mqtt_backend_get_suback( mqtt_suback_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    ret = mqtt_manager( MQTT_EVENT_RECV_SUBACK, args, conn );
    if ( ret == ZOS_SUCCESS )
    {
        /* Publish is an async method (we don't get an OK), so we simulate the OK after sending it */

        if ( conn->callback != NULL )
        {

            mqtt_event_info_t event;
            event.type = MQTT_EVENT_TYPE_SUBCRIBED;
            event.data.msgid = args->packet_id;
            conn->callback( &event );
        }

    }
    return ret;
}

zos_result_t mqtt_backend_put_unsubscribe( const mqtt_unsubscribe_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_frame_t frame;
    mqtt_unsubscribe_arg_t final_args;
    memcpy(&final_args, args, sizeof(mqtt_subscribe_arg_t));

    /* Generate packet ID */
    // final_args.packet_id = conn->packet_id++;

    /* Send Protocol Header */
    ret = mqtt_frame_create( MQTT_CONNECTION_FRAME_MAX, &frame, &conn->socket );
    if ( ret != ZOS_SUCCESS )
    {
        return ret;
    }
    mqtt_frame_put_unsubscribe( &frame, &final_args );

    MQTT_LOG("Send UNSUBSCRIBE frame");
    ret = mqtt_frame_send( &frame, &conn->socket );

    /* delete packet as soon as sent */
    mqtt_frame_delete(&frame);

    return ret;
}

zos_result_t mqtt_backend_get_unsuback( mqtt_unsuback_arg_t *args, mqtt_connection_t *conn )
{
    zos_result_t ret;
    ret = mqtt_manager( MQTT_EVENT_RECV_UNSUBACK, args, conn );
    if ( ret == ZOS_SUCCESS )
    {

        if ( conn->callback != NULL )
        {

            mqtt_event_info_t event;
            event.type = MQTT_EVENT_TYPE_UNSUBSCRIBED;
            event.data.msgid = args->packet_id;

            conn->callback( &event );
        }
    }
    return ret;
}

zos_result_t mqtt_backend_put_disconnect( mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_frame_t frame;

    ret = mqtt_frame_create( MQTT_CONNECTION_FRAME_MAX, &frame, &conn->socket );
    if ( ret != ZOS_SUCCESS )
    {
        return ret;
    }
    mqtt_frame_put_disconnect( &frame );

    MQTT_LOG("Send DISCONNECT frame");
    ret = mqtt_frame_send( &frame, &conn->socket );

    /* delete packet as soon as sent */
    mqtt_frame_delete(&frame);

    return ret;
}

zos_result_t mqtt_backend_connection_close( mqtt_connection_t *conn )
{
    zos_result_t ret;
    ret = mqtt_manager( MQTT_EVENT_CONNECTION_CLOSE, NULL, conn );
    if ( ret == ZOS_SUCCESS )
    {
        /* Publish is an async method (we don't get an OK), so we simulate the OK after sending it */

        if ( conn->callback != NULL )
        {

            mqtt_event_info_t event;
            event.type = MQTT_EVENT_TYPE_DISCONNECTED;
            conn->callback( &event );
        }
    }
    return ret;
}

zos_result_t mqtt_backend_put_pingreq( mqtt_connection_t *conn )
{
    zos_result_t ret;
    mqtt_frame_t frame;

    ret = mqtt_frame_create( MQTT_CONNECTION_FRAME_MAX, &frame, &conn->socket );
    if ( ret != ZOS_SUCCESS )
    {
        return ret;
    }
    mqtt_frame_put_pingreq( &frame );

    MQTT_LOG("Send PINGREQ frame");
    ret = mqtt_frame_send( &frame, &conn->socket );

    /* delete packet as soon as sent */
    mqtt_frame_delete(&frame);

    return ret;
}

zos_result_t mqtt_backend_get_pingres( mqtt_connection_t *conn )
{
    return mqtt_manager( MQTT_EVENT_RECV_PINGRES, NULL, conn );
}
