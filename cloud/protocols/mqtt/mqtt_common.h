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
 *  WICED MQTT constants, data types which are common to API and library
 */
#pragma once

#include "zos_types.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define MQTT_CONNECTION_DEFAULT_PORT              (1883)          /* Unsecure port based on broker */
#define MQTT_CONNECTION_SECURE_PORT               (8883)          /* secure port based on broker */
#define MQTT_PROTOCOL_VER4                        (4)             /* Mqtt protocol version 4 */
#define MQTT_PROTOCOL_VER3                        (3)             /* Mqtt protocol version 3 */
#define MQTT_CONNECTION_TIMEOUT                   (5000)          /* Tcp connection timeout */
#define MQTT_CONNECTION_NUMBER_OF_RETRIES         (3)             /* Tcp connection retries */
/******************************************************
 *                  typedef Enumerations
 ******************************************************/
/**
 * Defines the QoS levels supported by MQTT library
 */
typedef enum mqtt_qos_level_s
{
    MQTT_QOS_DELIVER_AT_MOST_ONCE   = 0x00,               /* QoS level 0 */
    MQTT_QOS_DELIVER_AT_LEAST_ONCE  = 0x01,               /* QoS level 1 */
    MQTT_QOS_DELIVER_EXACTLY_ONCE   = 0x02,               /* QoS level 2 */
    MQTT_QOS_DELIVER_FAILURE        = 0x80,               /* Invalid QoS level */
} mqtt_qos_level_t;

/**
 * Defines MQTT event types received in call-back function
 */
typedef enum mqtt_event_s
{
    MQTT_EVENT_TYPE_CONNECTED = 1,                        /* Event sent when broker accepts CONNECT request */
    MQTT_EVENT_TYPE_DISCONNECTED,                         /* Event sent when broker accepts DISCONNECT request or when there is any network issue */
    MQTT_EVENT_TYPE_PUBLISHED,                            /* Event sent for QOS-1 and QOS-2 for the published when successfully delivered. No event will be sent for QOS-0. */
    MQTT_EVENT_TYPE_SUBCRIBED,                            /* Event sent when broker accepts SUBSCRIBED request */
    MQTT_EVENT_TYPE_UNSUBSCRIBED,                         /* Event sent when broker accepts UNSUBSCRIBED request */
    MQTT_EVENT_TYPE_PUBLISH_MSG_RECEIVED,                 /* Event sent when PUBLISH message is received from the broker for a subscribed topic */
    MQTT_EVENT_TYPE_UNKNOWN                               /* Event type not known */
} mqtt_event_type_t;

/**
 * List of error codes returned by MQTT library
 */
typedef enum mqtt_err_code_s
{
    MQTT_CONN_ERR_CODE_NONE = 0,                          /* Success */
    MQTT_CONN_ERR_CODE_UNACCEPTABLE_PROTOCOL_VERSION,     /* Protocol version is not acceptable by MQTT Broker */
    MQTT_CONN_ERR_CODE_IDENTIFIER_REJECTED,               /* Client ID rejected by MQTT Broker */
    MQTT_CONN_ERR_CODE_SERVER_UNAVAILABLE,                /* MQTT Broker unavailable */
    MQTT_CONN_ERR_CODE_BAD_USER_NAME_OR_PASSWORD,         /* Bad user name or password */
    MQTT_CONN_ERR_CODE_NOT_AUTHORIZED,                    /* Client not authorized to connect */
    MQTT_CONN_ERR_CODE_INVALID                            /* Invalid error code */
} mqtt_conn_err_code_t;

/******************************************************
 *              MQTT Type Definitions
 ******************************************************/
/**
 * Message ID for the given MQTT message
 */
typedef uint16_t mqtt_msgid_t;

/**
 * Contains the message received for a topic from the Broker
 */
typedef struct mqtt_topic_msg_s
{
    uint8_t*    topic;                                          /* Name of the topic associated with the message. It's not 'null' terminated */
    uint32_t    topic_len;                                      /* Length of the topic */
    uint8_t*    data;                                           /* Payload of the message */
    uint32_t    data_len;                                       /* Length of the message payload */
} mqtt_topic_msg_t;

/* MQTT Event info */
typedef struct mqtt_event_info_s
{
    mqtt_event_type_t             type;                   /* Message event type */
    union
    {
        mqtt_conn_err_code_t      err_code;               /* Valid only for MQTT_EVENT_TYPE_CONNECT_REQ_STATUS event. Indicates the error identified while connecting to Broker */
        mqtt_msgid_t              msgid;                  /* Valid only for MQTT_EVENT_TYPE_PUBLISHED, MQTT_EVENT_TYPE_SUBCRIBED, MQTT_EVENT_TYPE_UNSUBSCRIBED events. Indicates message ID */
        mqtt_topic_msg_t          pub_recvd;              /* Valid only for MQTT_EVENT_TYPE_PUBLISH_MSG_RECEIVED event. Indicates the message received from Broker */
    }                             data;                   /* Event data */
} mqtt_event_info_t;

/**
 * Contains information related to establishing connection with Broker
 */
typedef struct mqtt_pkt_connect_s
{
    uint8_t     mqtt_version;                                   /* Indicates mqtt version number. Supported versions are 3 and 4. Any value other than 4 will be treated as 3 (default)*/
    uint16_t    keep_alive;                                     /* Indicates keep alive interval to Broker */
    uint8_t     clean_session;                                  /* Indicates if the session to be cleanly started */
    uint8_t*    client_id;                                      /* Client ID */
    uint8_t*    username;                                       /* User name to connect to Broker */
    uint8_t*    password;                                       /* Password to connect to Broker */
} mqtt_pkt_connect_t;

/** Call-back function for MQTT events
 *
 * @param[in] event             : Pointer to event structure which contains the details about the event
 *
 * @return @ref zos_result_t
 */
typedef zos_result_t (*mqtt_callback_t)( mqtt_event_info_t *event );

#ifdef __cplusplus
} /* extern "C" */
#endif
