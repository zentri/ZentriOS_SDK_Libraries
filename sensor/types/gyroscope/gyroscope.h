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
 * @addtogroup  lib_sensor_gyroscope
 * @{
 */

/*
 * Gyroscope fullscale values
 */
typedef enum
{
    GYRO_FULLSCALE_250DPS,
    GYRO_FULLSCALE_500DPS,
    GYRO_FULLSCALE_1000DPS,
    GYRO_FULLSCALE_2000DPS
} gyro_fullscale_t;

#define GYRO_AXIS_EN_X 0x01
#define GYRO_AXIS_EN_Y 0x02
#define GYRO_AXIS_EN_Z 0x04

typedef uint8_t gyro_axis_en_t;

/** Gyroscope configuration */
typedef struct
{
    gyro_fullscale_t fullscale; //!< fullscale
    gyro_axis_en_t axis_en; //!< axis enable
} gyroscope_config_t;

typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} gyroscope_data_t;

/**
 * @}
 */

/**
 * Initialise gyroscope driver.
 *
 * @param[in] config: config used by initialisation function (specific to device)
 * @return @ref zos_result_t
 */
zos_result_t sensor_gyroscope_init(const gyroscope_config_t *config);

/**
 * Poll gyroscope to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref zos_result_t
 */
zos_result_t sensor_gyroscope_has_new_data(zos_bool_t *has_data);

/**
 * Get latest data from gyroscope sensor
 *
 * @param[out] data: Data from sensor in dps
 * @return @ref zos_result_t
 */
zos_result_t sensor_gyroscope_get_data(gyroscope_data_t *data);


