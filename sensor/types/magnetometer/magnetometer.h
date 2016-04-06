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

#define MAGN_AXIS_EN_X 0x01
#define MAGN_AXIS_EN_Y 0x02
#define MAGN_AXIS_EN_Z 0x04

/**
 * @addtogroup  lib_sensor_magnetometer
 * @{
 */

typedef struct
{
    uint8_t unused;
} magnetometer_config_t;

/**
 * Magnetometer data
 */
 
typedef struct
{
    int32_t x; //!< X axis value
    int32_t y; //!< Y axis value
    int32_t z; //!< Z axis value
} magnetometer_data_t;


/**
 * @}
 */


/**
 * Initialise magnetometer driver.
 *
 * @param[in] config: config used by initialisation function (specific to device)
 * @return @ref zos_result_t
 */
zos_result_t sensor_magnetometer_init(const magnetometer_config_t *config);

/**
 * Poll magnetometer to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref zos_result_t
 */
zos_result_t sensor_magnetometer_has_new_data(zos_bool_t *has_data);

/**
 * Get latest data from magnetometer sensor
 *
 * @param[out] data: Data from sensor in dps
 * @return @ref zos_result_t
 */
zos_result_t sensor_magnetometer_get_data(magnetometer_data_t *data);

