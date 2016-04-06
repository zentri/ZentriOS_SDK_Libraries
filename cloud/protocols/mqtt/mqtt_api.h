/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#pragma once

#include "zos_types.h"
#include "mqtt_common.h"
#include "mqtt_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup lib_cloud_mqtt
 * @{
 */

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                   Function Prototypes
 ******************************************************/
/** Initializes MQTT object
 *
 * @param[in] mqtt_connection   : Contains address of a memory location, having size of MQTT_OBJECT_MEMORY_SIZE_REQUIREMENT bytes
 *                                Application has to allocate it non stack memory area. And application has to free it after use
 * @return @ref zos_result_t
 * NOTE :  The mqtt_connection memory here can be freed or reused by application after calling mqtt_deinit()
 *
 */
zos_result_t mqtt_init( mqtt_connection_t* mqtt_connection );


/** De-initializes MQTT object
 *
 * @param[in] mqtt_connection   : Contains address of a memory location which is passed during MQTT init
 *
 * @return @ref zos_result_t
 */
zos_result_t mqtt_deinit( mqtt_connection_t* mqtt_connection );

/** Opens a TCP/TLS connection with MQTT broker.
 *
 *
 * @param[in] mqtt_connection   : Contains address of a memory location which is passed during MQTT init
 * @param[in] address           : IP address of the Broker
 * @param[in] port_number       : Port number broker is listening to
 * @param[in] interface         : Network interface to be used for establishing connection with Broker
 * @param[in] callback          : Event callback function which is used for notifying the events from library
 * @param[in] security          : Security (TLS) is enabled (TRUE) or disabled (FALSE)
 *
 * @return @ref zos_result_t
 */
zos_result_t mqtt_open( mqtt_connection_t* mqtt_connection, const char *address, uint16_t port_number, zos_interface_t interface, mqtt_callback_t callback, zos_bool_t security );

/** Send connection frame to MQTT broker.
 *
 * NOTE:
 *      This is an asynchronous API. Connection status will be notified using callback function.
 *      MQTT_EVENT_TYPE_CONNECTED event will be sent using callback function
 *
 * @param[in] mqtt_connection   : Contains address of a memory location which is passed during MQTT init
 * @param[in] conninfo          : MQTT connect message related information
 *
 * @return @ref zos_result_t
 * NOTE: Allocate memory for conninfo->client_id, conninfo->username, conninfo->password in non-stack area.
 *       And free/resuse them after getting event MQTT_EVENT_TYPE_CONNECT_REQ_STATUS or MQTT_EVENT_TYPE_DISCONNECTED
 */
zos_result_t mqtt_connect( mqtt_connection_t* mqtt_connection, mqtt_pkt_connect_t *conninfo );

/** Disconnect from MQTT broker.
 *
 * NOTE:
 *      This is an asynchronous API. Disconnect status will be notified using using callback function.
 *      MQTT_EVENT_TYPE_DISCONNECTED event will be sent using callback function
 *
 * @param[in] mqtt_connection   : Contains address of a memory location which is passed during MQTT init
 *
 * @return @ref zos_result_t
 *
 * NOTE: Allocate memory for conninfo->client_id, conninfo->username, conninfo->password in non-stack area.
 *       And free/resuse them after getting event MQTT_EVENT_TYPE_CONNECT_REQ_STATUS or MQTT_EVENT_TYPE_DISCONNECTED
 *
 */
zos_result_t mqtt_disconnect( mqtt_connection_t* mqtt_connection );


/** Publish message to MQTT Broker on the given Topic
 *
 * NOTE:
 *      This is an asynchronous API. Publish status will be notified using using callback function.
 *      MQTT_EVENT_TYPE_PUBLISHED event will be sent using callback function
 *
 * @param[in] mqtt_connection   : Contains address of a memory location which is passed during MQTT init
 * @param[in] topic             : Contains the topic on which the message to be published
 * @param[in] message           : Pointer to the message to be published
 * @param[in] msg_len           : Length of the message pointed by 'message' pointer
 * @param[in] qos               : QoS level to be used for publishing the given message
 *
 * @return mqtt_msgid_t   : ID for the message being published
 * NOTE: Allocate memory for topic, data in non-stack area.
 *       And free/resuse them after getting event MQTT_EVENT_TYPE_PUBLISHED
 *       or MQTT_EVENT_TYPE_DISCONNECTED for given message ID (mqtt_msgid_t)
 */
mqtt_msgid_t mqtt_publish( mqtt_connection_t* mqtt_connection, uint8_t *topic, uint8_t *message, uint32_t msg_len, uint8_t qos );


/** Subscribe for a topic with MQTT Broker
 *
 * NOTE:
 *      This is an asynchronous API. Subscribe status will be notified using using callback function.
 *      MQTT_EVENT_TYPE_SUBCRIBED event will be sent using callback function
 *
 * @param[in] mqtt_connection   : Contains address of a memory location which is passed during MQTT init
 * @param[in] topic             : Contains the topic to be subscribed to
 * @param[in] qos               : QoS level to be used for receiving the message on the given topic
 *
 * @return mqtt_msgid_t   : ID for the message being subscribed
 * NOTE: Allocate memory for topic in non-stack area.
 *       And free/resuse them after getting event MQTT_EVENT_TYPE_SUBCRIBED
 *       or MQTT_EVENT_TYPE_DISCONNECTED for given message ID (mqtt_msgid_t)
 */
mqtt_msgid_t mqtt_subscribe( mqtt_connection_t* mqtt_connection, uint8_t *topic, uint8_t qos );


/** Unsubscribe the topic from MQTT Broker
 *
 * NOTE:
 *      This is an asynchronous API. Unsubscribe status will be notified using using callback function.
 *      MQTT_EVENT_TYPE_UNSUBCRIBED event will be sent using callback function
 *
 * @param[in] mqtt_connection   : Contains address of a memory location which is passed during MQTT init
 * @param[in] topic             : Contains the topic to be unsubscribed
 *
 * @return mqtt_msgid_t   : ID for the message being subscribed
 * NOTE: Allocate memory for topic in non-stack area.
 *       And free/resuse them after getting event MQTT_EVENT_TYPE_UNSUBSCRIBED
 *       or MQTT_EVENT_TYPE_DISCONNECTED for given message ID (mqtt_msgid_t)
 */
mqtt_msgid_t mqtt_unsubscribe( mqtt_connection_t* mqtt_connection, uint8_t *topic );

/**
 * @}
 */

#ifdef __cplusplus
} /* extern "C" */
#endif
