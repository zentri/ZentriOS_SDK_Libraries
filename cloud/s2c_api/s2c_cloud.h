/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once


zos_result_t s2c_cloud_connect(void);

void s2c_cloud_disconnect(void *unused);

void s2c_cloud_disconnected_callback(void *arg);

zos_bool_t s2c_cloud_check_internet_connection(void);
