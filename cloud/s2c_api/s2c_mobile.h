/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once



void s2c_mobile_connected_callback(void *arg);

void s2c_mobile_disconnected_callback(void *arg);

zos_bool_t s2c_mobile_command_callback(const char *cmd_str);
