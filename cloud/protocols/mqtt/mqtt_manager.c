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
 *  Connection manager
 *
 *  Handles connection state and server exceptions.
 */

#include "mqtt_api.h"
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
static zos_result_t mqtt_manager_resend_packet( mqtt_frame_type_t type, void *arg, void *p_user );
static void mqtt_manager_tick( void* arg );
static zos_result_t mqtt_manager_heartbeat_init( uint16_t keep_alive, void *p_user, mqtt_heartbeat_t *heartbeat );
inline static void mqtt_manager_heartbeat_send_reset( mqtt_heartbeat_t *heartbeat );
inline static void mqtt_manager_heartbeat_recv_reset( mqtt_heartbeat_t *heartbeat );
inline static zos_result_t mqtt_manager_heartbeat_send_step( mqtt_heartbeat_t *heartbeat );
inline static zos_result_t mqtt_manager_heartbeat_recv_step( mqtt_heartbeat_t *heartbeat );
static void mqtt_manager_heartbeat_deinit( mqtt_heartbeat_t *heartbeat );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

static void mqtt_manager_tick( void* arg )
{
    mqtt_connection_t *conn = (mqtt_connection_t *) arg;
    if ( mqtt_manager( MQTT_EVENT_TICK, NULL, conn ) != ZOS_SUCCESS )
    {

        /* Publish is an async method (we don't get an OK), so we simulate the OK after sending it */
        if ( conn->callback != NULL )
        {

            mqtt_event_info_t event;
            event.type = MQTT_EVENT_TYPE_DISCONNECTED;
            event.data.err_code = MQTT_CONN_ERR_CODE_INVALID;
            conn->callback( &event );
        }
    }
}

static zos_result_t mqtt_manager_heartbeat_init( uint16_t keep_alive, void *p_user, mqtt_heartbeat_t *heartbeat )
{
    heartbeat->step_value = 1;
    heartbeat->reset_value = keep_alive;
    heartbeat->send_counter = keep_alive;
    heartbeat->recv_counter = (uint32_t) 2 * keep_alive;
    return mqtt_event_register_periodic( mqtt_manager_tick, p_user, heartbeat->step_value * 1000, EVENT_FLAGS1(RUN_NOW) );

}

inline static void mqtt_manager_heartbeat_send_reset( mqtt_heartbeat_t *heartbeat )
{
    heartbeat->send_counter = heartbeat->reset_value;
}

inline static void mqtt_manager_heartbeat_recv_reset( mqtt_heartbeat_t *heartbeat )
{
    heartbeat->recv_counter = 2 * heartbeat->reset_value;
}

inline static zos_result_t mqtt_manager_heartbeat_send_step( mqtt_heartbeat_t *heartbeat )
{
    if ( heartbeat->send_counter >= heartbeat->step_value )
    {
        heartbeat->send_counter -= heartbeat->step_value;
    }
    else
    {
        heartbeat->send_counter = 0;
    }
    return ( heartbeat->send_counter == 0 ) ? ZOS_ERROR : ZOS_SUCCESS;
}

inline static zos_result_t mqtt_manager_heartbeat_recv_step( mqtt_heartbeat_t *heartbeat )
{
    if ( heartbeat->recv_counter >= heartbeat->step_value )
    {
        heartbeat->recv_counter -= heartbeat->step_value;
    }
    else
    {
        heartbeat->recv_counter = 0;
    }
    return ( heartbeat->recv_counter == 0 ) ? ZOS_ERROR : ZOS_SUCCESS;
}

static void mqtt_manager_heartbeat_deinit( mqtt_heartbeat_t *heartbeat )
{
    mqtt_event_unregister( mqtt_manager_tick, NULL );
}

zos_result_t mqtt_manager( mqtt_event_t event, void *args, mqtt_connection_t *conn )
{
    zos_result_t result = ZOS_SUCCESS;

    switch ( event )
    {
        case MQTT_EVENT_SEND_CONNECT:
        {
            mqtt_connect_arg_t *connect_arg = (mqtt_connect_arg_t *) args;
            if ( connect_arg->keep_alive )
            {
                mqtt_manager_heartbeat_init( connect_arg->keep_alive, conn, &conn->heartbeat );
            }
            mqtt_backend_put_connect( args, conn );
        }
            break;

        case MQTT_EVENT_SEND_DISCONNECT:
        {
            mqtt_manager_heartbeat_deinit( &conn->heartbeat );
            if ( ( result = mqtt_backend_put_disconnect( conn ) ) == ZOS_SUCCESS )
            {
                mqtt_backend_connection_close( conn );
            }
        }
            break;

        case MQTT_EVENT_SEND_SUBSCRIBE:
        {
            mqtt_backend_put_subscribe( args, conn );
            mqtt_manager_heartbeat_send_reset( &conn->heartbeat );
            mqtt_session_add_item( MQTT_PACKET_TYPE_SUBSCRIBE, args, conn->session );
        }
            break;

        case MQTT_EVENT_SEND_UNSUBSCRIBE:
        {
            mqtt_backend_put_unsubscribe( args, conn );
            mqtt_manager_heartbeat_send_reset( &conn->heartbeat );
            mqtt_session_add_item( MQTT_PACKET_TYPE_UNSUBSCRIBE, args, conn->session );
        }
            break;

        case MQTT_EVENT_SEND_PUBLISH:
        {
            mqtt_publish_arg_t *publish_args = (mqtt_publish_arg_t *) args;
            mqtt_session_add_item( MQTT_PACKET_TYPE_PUBLISH, args, conn->session );
            result = mqtt_backend_put_publish( args, conn );
            if ( ( publish_args->qos != MQTT_QOS_DELIVER_AT_MOST_ONCE ) )
            {
                mqtt_manager_heartbeat_send_reset( &conn->heartbeat );
            }

            if ( publish_args->qos == MQTT_QOS_DELIVER_AT_MOST_ONCE )
            {
                if ( mqtt_session_remove_item( MQTT_PACKET_TYPE_PUBLISH, publish_args->packet_id, conn->session ) != ZOS_SUCCESS )
                {
                    MQTT_LOG( "Publish packet %d not in session queue", publish_args->packet_id );
                }
                if ( ( result == ZOS_SUCCESS ) & ( conn->callback != NULL ) )
                {
                    mqtt_event_info_t callback_event;
                    callback_event.type = MQTT_EVENT_TYPE_PUBLISHED;
                    conn->callback( &callback_event );
                }
            }
        }
            break;

        case MQTT_EVENT_RECV_PUBLISH:
        {
            mqtt_publish_arg_t *publish_args = (mqtt_publish_arg_t *) args;
            mqtt_manager_heartbeat_recv_reset( &conn->heartbeat );
            if ( publish_args->qos == MQTT_QOS_DELIVER_AT_LEAST_ONCE )
            {
                mqtt_puback_arg_t puback_args;
                puback_args.packet_id = publish_args->packet_id;
                mqtt_backend_put_puback( &puback_args, conn );
                mqtt_manager_heartbeat_send_reset( &conn->heartbeat );
            }
            else if ( publish_args->qos == MQTT_QOS_DELIVER_EXACTLY_ONCE )
            {
                mqtt_pubrec_arg_t pubrec_args;
                pubrec_args.packet_id = publish_args->packet_id;
                mqtt_backend_put_pubrec( &pubrec_args, conn );
                mqtt_manager_heartbeat_send_reset( &conn->heartbeat );
                if ( mqtt_session_item_exist( MQTT_PACKET_TYPE_PUBREC, publish_args->packet_id, conn->session ) != ZOS_SUCCESS )
                {
                    /* new publish packet */
                    mqtt_session_add_item( MQTT_PACKET_TYPE_PUBREC, &pubrec_args, conn->session );
                }
                else
                {
                    /* This item is already received before shouldn't be passed to user */
                    publish_args->data = NULL;
                }
            }
        }
            break;

        case MQTT_EVENT_RECV_SUBACK:
        {
            mqtt_suback_arg_t *suback_args = (mqtt_suback_arg_t *) args;
            mqtt_manager_heartbeat_recv_reset( &conn->heartbeat );
            if ( mqtt_session_remove_item( MQTT_PACKET_TYPE_SUBSCRIBE, suback_args->packet_id, conn->session ) != ZOS_SUCCESS )
            {
                MQTT_LOG( "Suback packet %d not in session queue", suback_args->packet_id );
            }
        }
            break;

        case MQTT_EVENT_RECV_UNSUBACK:
        {
            mqtt_unsuback_arg_t *unsuback_args = (mqtt_unsuback_arg_t *) args;
            mqtt_manager_heartbeat_recv_reset( &conn->heartbeat );
            if ( mqtt_session_remove_item( MQTT_PACKET_TYPE_UNSUBSCRIBE, unsuback_args->packet_id, conn->session ) != ZOS_SUCCESS )
            {
                MQTT_LOG( "Unsuback packet %d not in session queue", unsuback_args->packet_id );
            }
        }
            break;

        case MQTT_EVENT_RECV_PUBACK:
        {
            mqtt_puback_arg_t *puback_args = (mqtt_puback_arg_t *) args;
            mqtt_manager_heartbeat_recv_reset( &conn->heartbeat );
            if ( mqtt_session_remove_item( MQTT_PACKET_TYPE_PUBLISH, puback_args->packet_id, conn->session ) != ZOS_SUCCESS )
            {
                MQTT_LOG( "Puback packet %d not in session queue", puback_args->packet_id );
            }
        }
            break;

        case MQTT_EVENT_RECV_PUBREC:
        {
            mqtt_pubrec_arg_t *pubrec_args = (mqtt_pubrec_arg_t *) args;
            mqtt_pubrel_arg_t pubrel_args;
            pubrel_args.packet_id = pubrec_args->packet_id;

            mqtt_manager_heartbeat_recv_reset( &conn->heartbeat );
            if ( mqtt_session_remove_item( MQTT_PACKET_TYPE_PUBLISH, pubrec_args->packet_id, conn->session ) != ZOS_SUCCESS )
            {
                MQTT_LOG( "Publish packet %d not in session queue", pubrec_args->packet_id );
            }

            mqtt_backend_put_pubrel( &pubrel_args, conn );
            mqtt_manager_heartbeat_send_reset( &conn->heartbeat );
            if ( mqtt_session_item_exist( MQTT_PACKET_TYPE_PUBREL, pubrel_args.packet_id, conn->session ) != ZOS_SUCCESS )
            {
                mqtt_session_add_item( MQTT_PACKET_TYPE_PUBREL, &pubrel_args, conn->session );
            }
        }
            break;

        case MQTT_EVENT_RECV_PUBREL:
        {
            mqtt_pubrel_arg_t *pubrel_args = (mqtt_pubrel_arg_t *) args;
            mqtt_pubcomp_arg_t pubcomp_args;
            pubcomp_args.packet_id = pubrel_args->packet_id;

            mqtt_manager_heartbeat_recv_reset( &conn->heartbeat );
            if ( mqtt_session_remove_item( MQTT_PACKET_TYPE_PUBREC, pubrel_args->packet_id, conn->session ) != ZOS_SUCCESS )
            {
                MQTT_LOG( "Pubrec packet %d not in session queue", pubrel_args->packet_id );
            }
            mqtt_backend_put_pubcomp( &pubcomp_args, conn );
            mqtt_manager_heartbeat_send_reset( &conn->heartbeat );
        }
            break;

        case MQTT_EVENT_RECV_PUBCOMP:
        {
            mqtt_pubcomp_arg_t *pubcomp_args = (mqtt_pubcomp_arg_t *) args;

            mqtt_manager_heartbeat_recv_reset( &conn->heartbeat );
            if ( mqtt_session_remove_item( MQTT_PACKET_TYPE_PUBREL, pubcomp_args->packet_id, conn->session ) != ZOS_SUCCESS )
            {
                MQTT_LOG( "Pubrel packet %d not in session queue", pubcomp_args->packet_id );
            }
        }
            break;

        case MQTT_EVENT_RECV_PINGRES:
        {
            mqtt_manager_heartbeat_recv_reset( &conn->heartbeat );
            /* Resend any thing in the session */
            if ( mqtt_session_iterate_through_items( mqtt_manager_resend_packet, conn, conn->session ) != ZOS_SUCCESS )
            {
                MQTT_LOG( "Error resending session messages" );
            }
        }
            break;

        case MQTT_EVENT_RECV_CONNACK:
        {
            mqtt_manager_heartbeat_recv_reset( &conn->heartbeat );
            /* Resend any thing in the session */
            if ( mqtt_session_iterate_through_items( mqtt_manager_resend_packet, conn, conn->session ) != ZOS_SUCCESS )
            {
                MQTT_LOG( "Error resending session messages" );
            }
        }
            break;

        case MQTT_EVENT_TICK:
        {
            if ( mqtt_manager_heartbeat_recv_step( &conn->heartbeat ) != ZOS_SUCCESS )
            {
                mqtt_manager_heartbeat_deinit( &conn->heartbeat );

                /* Reset counter timed out and we didn't receive any thing from broker */
                MQTT_LOG("Heartbeat timeout. Connection closed with broker. Close local connection");
                mqtt_network_close( conn );
                result = ZOS_ERROR;
            }
            else
            {
                if ( mqtt_manager_heartbeat_send_step( &conn->heartbeat ) != ZOS_SUCCESS )
                {
                    mqtt_backend_put_pingreq( conn );
                    mqtt_manager_heartbeat_send_reset( &conn->heartbeat );
                }
            }
        }
            break;

        case MQTT_EVENT_CONNECTION_CLOSE:
        {
            mqtt_network_close( conn );
        }
            break;

        case MQTT_EVENT_SEND_PINGREQ:
        case MQTT_EVENT_SEND_PUBREL:
        case MQTT_EVENT_SEND_PUBREC:
        case MQTT_EVENT_SEND_PUBCOMP:
        case MQTT_EVENT_SEND_PUBACK:
        default:
            break;
    }

    return result;
}

static zos_result_t mqtt_manager_resend_packet( mqtt_frame_type_t type, void *arg, void *p_user )
{
    mqtt_connection_t *conn = (mqtt_connection_t*) p_user;
    zos_result_t result;
    switch ( type )
    {
        case MQTT_PACKET_TYPE_SUBSCRIBE:
        {
            result = mqtt_backend_put_subscribe( arg, conn );
        }
            break;
        case MQTT_PACKET_TYPE_UNSUBSCRIBE:
        {
            result = mqtt_backend_put_unsubscribe( arg, conn );
        }
            break;
        case MQTT_PACKET_TYPE_PUBLISH:
        {
            result = mqtt_backend_put_publish( arg, conn );
        }
            break;
        case MQTT_PACKET_TYPE_PUBREC:
        {
            result = mqtt_backend_put_pubrec( arg, conn );
        }
            break;
        case MQTT_PACKET_TYPE_PUBREL:
        {
            result = mqtt_backend_put_pubrel( arg, conn );
        }
            break;
        case MQTT_PACKET_TYPE_PUBCOMP:
        case MQTT_PACKET_TYPE_PUBACK:
        case MQTT_PACKET_TYPE_UNSUBACK:
        case MQTT_PACKET_TYPE_SUBACK:
        case MQTT_PACKET_TYPE_CONNACK:
        case MQTT_PACKET_TYPE_PINGREQ:
        case MQTT_PACKET_TYPE_PINGRESP:
        case MQTT_PACKET_TYPE_CONNECT:
        case MQTT_PACKET_TYPE_DISCONNECT:
        default:
            return ZOS_ERROR;
    }
    return result;
}
