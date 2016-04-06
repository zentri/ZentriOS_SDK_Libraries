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


#define SENSOR_THERMOMETER_OUTPUT_SCALE            (uint32_t)100 //scalar applied to output value

/**
 * @addtogroup  lib_sensor_thermometer
 * @{
 */

typedef struct
{
    uint8_t unused;
} thermometer_config_t;

/**
 * @brief Thermometer data
 */
typedef struct
{
    uint32_t raw; //!< the raw thermometer value
    struct
    {
        int16_t whole; //!< the integer part of the thermometer value
        uint16_t fraction; //!< the decimal part of the thermometer value
    } value; //!< value
} thermometer_data_t;


/**
 * @}
 */

/**
 * Initialise thermometer driver.
 *
 * @param[in] config: config used by initialisation function
 * @return @ref zos_result_t
 */
zos_result_t sensor_thermometer_init(const thermometer_config_t *config);

/**
 * Poll thermometer to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref zos_result_t
 */
zos_result_t sensor_thermometer_has_new_data(zos_bool_t *has_data);

/**
 * Get latest data from thermometer
 *
 * @param[out] data: Temperature in degC x 100
 * @return @ref zos_result_t
 */
zos_result_t sensor_thermometer_get_data(thermometer_data_t *data);


/**
 * Read raw data from sensor
 *
 * @param[out] raw_data: data data read from sensor
 * @return @ref zos_result_t
 */
zos_result_t sensor_thermometer_read(uint16_t *raw_data);

/**
 * Convert raw data to normalized value
 * @param raw_value : raw data from @ref sensor_thermometer_read()
 * @return @ref zos_result_t
 */
int32_t sensor_thermometer_convert(uint16_t raw_value);


