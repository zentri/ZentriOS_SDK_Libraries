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

/**
 * Measure the relative humidity
 *
 * @param[out] humidity: Relative humidity measured (raw value)
 * @return @ref zos_result_t
 */
zos_result_t hih5030_humidity_read(uint16_t* humidity);

