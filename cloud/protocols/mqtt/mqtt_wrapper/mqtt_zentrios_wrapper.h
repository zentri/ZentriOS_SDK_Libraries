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
#include "zos_types.h"

zos_bool_t mqtt_network_is_up(zos_interface_t interface);

zos_result_t mqtt_tcp_connect(zos_interface_t interface, const char *host, uint16_t port, uint32_t *handle);

zos_result_t mqtt_tls_connect(zos_interface_t interface, const char *host, uint16_t port, uint32_t *handle);

zos_result_t mqtt_tcp_register_client_event_handlers(uint32_t handle, zos_stream_event_handler_t disconnect, zos_stream_event_handler_t receive);

zos_result_t mqtt_tcp_disconnect(uint32_t handle);

zos_result_t mqtt_tcp_read(uint32_t handle, void *data, uint32_t max_size, uint32_t *bytes_read);

zos_result_t mqtt_tcp_write(uint32_t handle, const void *data, uint32_t size, zos_bool_t auto_flush);

zos_result_t mqtt_malloc(uint8_t **ptr, uint32_t size);

zos_result_t mqtt_free(void *ptr);

zos_result_t mqtt_event_unregister(zos_event_handler_t handler, void *arg);

zos_result_t mqtt_event_register_periodic(zos_event_handler_t handler, void *arg, uint32_t period_ms, zos_event_flag_t flags);

void mqtt_log(const char *fmt, ...);

#ifdef DEBUG
#define MQTT_LOG(msg, ...) mqtt_log(msg, ## __VA_ARGS__)
#else
#define MQTT_LOG(msg, ...)
#endif
