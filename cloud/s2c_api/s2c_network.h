/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once





zos_result_t s2c_network_init(void);

void s2c_network_state_update(void);

void s2c_network_state_event_handler(void *arg);
