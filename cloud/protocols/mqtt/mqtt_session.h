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
 *  MQTT session APIs and types.
 *
 *  Internal types not to be included directly by applications.
 */
#pragma once

#include "zos_types.h"
#include "mqtt_frame.h"
#include "linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define     SESSION_ITEMS_SIZE      (MQTT_QUEUE_SIZE * 2)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *              MQTT session type Definitions
 ******************************************************/
typedef union mqtt_session_item_agrs_u
{
    mqtt_publish_arg_t     publish;
    mqtt_subscribe_arg_t   subscribe;
    mqtt_unsubscribe_arg_t unsubscribe;
    mqtt_pubrec_arg_t      pubrec;
    mqtt_pubrel_arg_t      pubrel;
}mqtt_session_item_args_t;

typedef struct mqtt_session_item_s
{
    struct list_head          list;
    mqtt_frame_type_t         type;
    mqtt_session_item_args_t  args;
}mqtt_session_item_t;

typedef struct mqtt_session_s
{
    struct list_head used_list;
    struct list_head nonused_list;
}mqtt_session_t;
/******************************************************
 *             Content Frame Type Definitions
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
zos_result_t mqtt_session_init                 (                                                                      mqtt_session_t *session );
zos_result_t mqtt_session_add_item             ( mqtt_frame_type_t type, void *args                           , mqtt_session_t *session );
zos_result_t mqtt_session_remove_item          ( mqtt_frame_type_t type, uint16_t packet_id                   , mqtt_session_t *session );
zos_result_t mqtt_session_item_exist           ( mqtt_frame_type_t type, uint16_t packet_id                   , mqtt_session_t *session );
zos_result_t mqtt_session_iterate_through_items( zos_result_t (*iter_func)(mqtt_frame_type_t type, void *arg , void *p_user ), void* p_user, mqtt_session_t *session);

#ifdef __cplusplus
} /* extern "C" */
#endif
