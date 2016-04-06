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
 *  Frame packing and unpacking functions
 */

#include "zos_types.h"
#include "mqtt_internal.h"
#include "mqtt_connection.h"
#include "mqtt_frame.h"
#include "mqtt_session.h"

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
 *               Interface functions
 ******************************************************/
zos_result_t mqtt_session_init( mqtt_session_t *session )
{
    /* Initialize both list heads used and non used */
    INIT_LIST_HEAD( &session->used_list );
    INIT_LIST_HEAD( &session->nonused_list );

    return ZOS_SUCCESS;
}

zos_result_t mqtt_session_add_item( mqtt_frame_type_t type, void *args, mqtt_session_t *session)
{
    mqtt_session_item_t *item;

    /* Check if there are any non used slots to grab */
    if ( list_empty( &session->nonused_list ) )
    {
        return ZOS_SUCCESS;
    }

    /* Get first item in the empty list */
    item = list_entry( session->nonused_list.next, mqtt_session_item_t, list );
    list_del( &item->list );

    /* Fill data */
    item->type = type;
    if ( type == MQTT_PACKET_TYPE_PUBLISH )
    {
        item->args.publish = *((mqtt_publish_arg_t *) args);
    }
    else if ( type == MQTT_PACKET_TYPE_SUBSCRIBE )
    {
        item->args.subscribe = *((mqtt_subscribe_arg_t *) args);
    }
    else if ( type == MQTT_PACKET_TYPE_UNSUBSCRIBE )
    {
        item->args.unsubscribe = *((mqtt_unsubscribe_arg_t *) args);
    }
    else if ( type == MQTT_PACKET_TYPE_PUBREC )
    {
        item->args.pubrec = *((mqtt_pubrec_arg_t *) args);
    }
    else if ( type == MQTT_PACKET_TYPE_PUBREL )
    {
        item->args.pubrel = *((mqtt_pubrel_arg_t *) args);
    }
    else
    {
        list_add( &item->list, &session->nonused_list );
        return ZOS_ERROR;
    }

    /* Add it to the used list */
    list_add_tail( &item->list, &session->used_list );

    return ZOS_SUCCESS;
}

zos_result_t mqtt_session_remove_item( mqtt_frame_type_t type, uint16_t packet_id, mqtt_session_t *session)
{
    struct list_head *pos, *q;
    mqtt_session_item_t *item = NULL;

    if ( list_empty( &session->used_list ) )
    {
        return ZOS_SUCCESS;
    }

    list_for_each_safe( pos, q, &session->used_list )
    {
        item = list_entry( pos, mqtt_session_item_t, list );
        if ( ( ( type == MQTT_PACKET_TYPE_PUBLISH     ) && ( item->args.publish.packet_id     == packet_id ) ) ||
             ( ( type == MQTT_PACKET_TYPE_SUBSCRIBE   ) && ( item->args.subscribe.packet_id   == packet_id ) ) ||
             ( ( type == MQTT_PACKET_TYPE_UNSUBSCRIBE ) && ( item->args.unsubscribe.packet_id == packet_id ) ) ||
             ( ( type == MQTT_PACKET_TYPE_PUBREC      ) && ( item->args.pubrec.packet_id      == packet_id ) ) ||
             ( ( type == MQTT_PACKET_TYPE_PUBREL      ) && ( item->args.pubrel.packet_id      == packet_id ) ) )
        {
            /* Remove item from used lists */
            list_del( &item->list );
            /* Add Item to non used list */
            list_add( &item->list, &session->nonused_list );

            return ZOS_SUCCESS;
        }
    }
    /* No match */
    return ZOS_ERROR;
}


zos_result_t mqtt_session_item_exist( mqtt_frame_type_t type, uint16_t packet_id, mqtt_session_t *session)
{
    struct list_head *pos;
    mqtt_session_item_t *item = NULL;

    if ( list_empty( &session->used_list ) )
    {
        return ZOS_ERROR;
    }

    list_for_each( pos, &session->used_list )
    {
        item = list_entry( pos, mqtt_session_item_t, list );
        if ( ( ( type == MQTT_PACKET_TYPE_PUBLISH     ) && ( item->args.publish.packet_id == packet_id     ) ) ||
             ( ( type == MQTT_PACKET_TYPE_SUBSCRIBE   ) && ( item->args.subscribe.packet_id == packet_id   ) ) ||
             ( ( type == MQTT_PACKET_TYPE_UNSUBSCRIBE ) && ( item->args.unsubscribe.packet_id == packet_id ) ) ||
             ( ( type == MQTT_PACKET_TYPE_PUBREC      ) && ( item->args.pubrec.packet_id == packet_id      ) ) ||
             ( ( type == MQTT_PACKET_TYPE_PUBREL      ) && ( item->args.pubrel.packet_id == packet_id      ) ) )
        {
            return ZOS_SUCCESS;
        }
    }
    /* No match */
    return ZOS_ERROR;
}

zos_result_t mqtt_session_iterate_through_items( zos_result_t (*iter_func)(mqtt_frame_type_t type, void *arg, void *p_user ), void* p_user, mqtt_session_t *session)
{
    struct list_head *pos;
    mqtt_session_item_t *item = NULL;

    if ( list_empty( &session->used_list ) )
    {
        return ZOS_SUCCESS;
    }

    list_for_each( pos, &session->used_list )
    {
        item = list_entry( pos, mqtt_session_item_t, list );
        if ( iter_func( item->type, &item->args, p_user ) != ZOS_SUCCESS )
        {
            return ZOS_ERROR;
        }
    }
    /* No match */
    return ZOS_SUCCESS;
}
