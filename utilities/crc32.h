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



void crc32_init(void);

void crc32_deinit(void);

uint32_t crc32(const uint8_t *message, uint32_t length);

uint32_t crc32_with_value(const uint8_t *message, uint32_t length, uint32_t previous_value);
