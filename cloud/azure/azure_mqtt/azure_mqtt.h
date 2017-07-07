/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once



#include "zos.h"
#include "iothub_client_LL.h"
#include "iothub_message.h"


zos_result_t azure_mqtt_start(const char *hostname, const char *device_id, const char *device_key);

zos_result_t azure_mqtt_stop(void);

void         azure_mqtt_set_connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK callback);
void         azure_mqtt_set_message_received_callback(IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC callback, void *callback_arg);
void         azure_mqtt_set_message_sent_callback(IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback, void *callback_arg);

zos_result_t azure_mqtt_send_message_str(const char *json_str, void *callback_arg);
zos_result_t azure_mqtt_send_message(IOTHUB_MESSAGE_HANDLE message_handle, void *callback_arg);


void         azure_mqtt_trigger_processing(void);
