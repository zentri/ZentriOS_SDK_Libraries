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

#define HIH6130_SLAVE_ADDRESS    0x27

typedef enum
{
    HIH6130_STATUS_NORMAL,
    HIH6130_STATUS_STALE_DATA,
    HIH6130_STATUS_COMMAND_MODE,
    HIH6130_STATUS_DIAGNOSTIC
} hih6130_status_t;

/**
 * Measure and read the relative humidity, temperature and status
 *
 * @param[out] status: Status of sensor
 * @param[out] humidity: Relative humidity measured (raw value)
 * @param[out] temperature: Ambient temperature (raw value)
 * @param[in] use_last: ZOS_TRUE to read value from last measurement, ZOS_FALSE to take new measurement
 * @return @ref zos_result_t
 */
zos_result_t hih6130_read(hih6130_status_t* status, uint16_t* humidity, uint16_t* temperature, const zos_bool_t use_last);

/**
 * Measure and read the relative humidity
 *
 *
 * @param[out] humidity: Relative humidity measured (raw value)
 * @param[in] use_last: ZOS_TRUE to read value from last measurement, ZOS_FALSE to take new measurement
 * @return @ref zos_result_t
 */
zos_result_t hih6130_humidity_read(uint16_t* humidity, const zos_bool_t use_last);

/**
 * Read the ambient temperature (get last measurement or take new sample)
 *
 *
 * @param[out] temperature: Ambient temperature (raw value)
 * @param[in] use_last: ZOS_TRUE to read value from last measurement, ZOS_FALSE to take new measurement
 * @return @ref zos_result_t
 */
zos_result_t hih6130_temperature_read(uint16_t *temperature, const zos_bool_t use_last);

/**
 * Get status
 *
 *
 * @param[out] status: Status of sensor
 * @return @ref zos_result_t
 */
zos_result_t hih6130_status_get(hih6130_status_t* status);

