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

#define SI7021_SLAVE_ADDRESS    0x40

typedef enum
{
    RES_RH_12BIT_TEMP_14_BIT,
    RES_RH_8BIT_TEMP_12_BIT,
    RES_RH_10BIT_TEMP_13_BIT,
    RES_RH_11BIT_TEMP_11_BIT
} si7021_resolution_t;

typedef enum
{
    SI7021_HEATER_DISABLE,
    SI7021_HEATER_ENABLE
} si7021_heat_en_t;

#define SI7021_HEATER_CUR_OFF   0
#define SI7021_HEATER_CUR_MID   7
#define SI7021_HEATER_CUR_MAX   15
typedef uint8_t si7021_heat_current_t;

typedef enum
{
    SI7021_VDD_OK,
    SI7021_VDD_LOW
} si7021_vdd_status_t;

typedef uint64_t si7021_serial_num_t;

typedef uint8_t si7021_firmware_rev_t;

typedef struct
{
    si7021_resolution_t resolution;
    si7021_heat_en_t heater_enable;
    si7021_heat_current_t heater_current;
} si7021_config_t;

/**
 * Initialise device
 *
 * @param[in] config: Configuration to write to the device
 * @return @ref zos_result_t
 */
zos_result_t si7021_init(const si7021_config_t *config);

/**
 * Set resolution for relative humidity and temperature sensor
 *
 *
 * @param[in] res: Resolution to write to the device
 * @return @ref zos_result_t
 */
zos_result_t si7021_resolution_set(const si7021_resolution_t res);

/**
 * Measure and read the relative humidity
 *
 *
 * @param[out] humidity: Relative humidity measured (0 - 100% x 10)
 * @param[in] use_checksum: ZOS_TRUE if read value should be verified with a checksum
 * @return @ref zos_result_t
 */
zos_result_t si7021_humidity_read(uint16_t* humidity, const zos_bool_t use_checksum);

/**
 * Read the ambient temperature (get last measurement or take new sample)
 *
 *
 * @param[out] temperature: Ambient temperature (deg C x 100)
 * @param[in] use_last: ZOS_TRUE to read value from last humidity measurement, ZOS_FALSE to take new measurement
 * @param[in] use_checksum: ZOS_TRUE if read value should be verified with a checksum
 * @return @ref zos_result_t
 */
zos_result_t si7021_temperature_read(uint16_t *temperature, const zos_bool_t use_last, const zos_bool_t use_checksum);

/**
 * Enable/Disable heater (clears condensation)
 *
 *
 * @param[in] enable: SI7021_HEATER_ENABLE to enable heater
 * @return @ref zos_result_t
 */
zos_result_t si7021_heater_enable(const si7021_heat_en_t enable);

/**
 * Set heater current
 *
 *
 * @param[in] current: register value between 0 (min) and 15 (max)
 * @return @ref zos_result_t
 */
zos_result_t si7021_heater_current_set(const si7021_heat_current_t current);

/**
 * Get VDD status
 *
 *
 * @param[out] status: Status of sensor VDD
 * @return @ref zos_result_t
 */
zos_result_t si7021_vdd_status_get(si7021_vdd_status_t* status);

/**
 * Read unique serial number of device
 *
 *
 * @param[out] serial: Serial number of device
 * @return @ref zos_result_t
 */
zos_result_t si7021_serial_number_read(si7021_serial_num_t* serial);

/**
 * Read firmware revision of sensor
 *
 *
 * @param[out] rev: Firmware revision of device
 * @return @ref zos_result_t
 */
zos_result_t si7021_firmware_rev_read(si7021_firmware_rev_t* rev);

/**
 * De-initialise driver
 *
 *
 * @return @ref zos_result_t
 */
void si7021_deinit();

