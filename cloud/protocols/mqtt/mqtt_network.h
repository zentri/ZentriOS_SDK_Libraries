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
 * Network APIs.
 *
 * Internal, not to be used directly by applications.
 */
#pragma once

#include "network/zos_network.h"
#include "zos_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/*
 * Network functions.
 *
 * Internal functions not to be used by user applications.
 */
zos_result_t mqtt_network_open            ( const char *server_ip_address, uint16_t portnumber, zos_interface_t interface,  void *p_user, mqtt_socket_t *socket, zos_bool_t security );
zos_result_t mqtt_network_close           ( void *p_user );

zos_result_t mqtt_network_create_buffer   ( mqtt_buffer_t *buffer, uint16_t size, mqtt_socket_t *socket );
zos_result_t mqtt_network_send_buffer     ( uint8_t *data, uint32_t size, mqtt_socket_t *socket );
zos_result_t mqtt_network_receive_buffer  ( uint32_t socket_handle, void *p_user );
zos_result_t mqtt_network_delete_buffer   ( uint8_t *data );

#ifdef __cplusplus
} /* extern "C" */
#endif
