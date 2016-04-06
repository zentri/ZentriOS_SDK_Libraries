/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once

#include <stdint.h>
#include "zos.h"

/**
 * @addtogroup  lib_sensor_hygrometer
 * @{
 */


#define SENSOR_HYROMETER_OUTPUT_SCALE            (uint32_t)10 //scalar applied to output value


typedef struct
{
    uint8_t unused;
} hygrometer_config_t;


/**
 * Hygrometer data
 */
typedef struct
{
    uint32_t raw;           //!< The raw humidity value
    struct
    {
        uint8_t whole;      //!< The integer part of the humidity value
        uint8_t fraction;    //!< The decimal part of the humidity value
    } value;
} hygrometer_data_t;


/**
 * @}
 */

/**
 * Initialise hygrometer driver.
 *
 * @param[in] config: config used by initialisation function
 * @return @ref zos_result_t
 */
zos_result_t sensor_hygrometer_init(const hygrometer_config_t *config);

/**
 * Poll hygrometer to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref zos_result_t
 */
zos_result_t sensor_hygrometer_has_new_data(zos_bool_t *has_data);

/**
 * Get latest data from hygrometer
 *
 * @param[out] data: Relative humidity (0 - 100%) x 10
 * @return @ref zos_result_t
 */
zos_result_t sensor_hygrometer_get_data(hygrometer_data_t *data);


/**
 * Read raw data from sensor
 *
 * @param[out] raw_data: data data read from sensor
 * @param[in] use_last_value: use last read value instead of reading from sensor
 * @return @ref zos_result_t
 */
zos_result_t sensor_hygrometer_read(uint16_t *raw_data, zos_bool_t use_last_value);

/**
 * Convert raw data to normalized value
 * @param raw_value : raw data from @ref sensor_hygrometer_read()
 * @return @ref zos_result_t
 */
uint32_t sensor_hygrometer_convert(uint16_t raw_value);


