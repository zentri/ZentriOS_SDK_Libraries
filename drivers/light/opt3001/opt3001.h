/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once



#include "zos.h"

zos_result_t opt3001_init();
zos_result_t opt3001_light_read(uint16_t *raw_lux);
