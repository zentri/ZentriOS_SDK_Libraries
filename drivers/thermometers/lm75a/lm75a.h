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



#define LM75A_TEMP_MASK 0x3FF
#define LM75A_TEMP_SIGN_BIT 0x200



zos_result_t lm75a_temperature_read(uint16_t *raw_temp);
