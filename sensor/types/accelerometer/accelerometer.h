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
 * @addtogroup  lib_sensor_accelerometer
 * @{
 */

/**
 * Fullscale Range
 */
typedef enum
{
    ACCEL_FULLSCALE_2G, //!< 2G
    ACCEL_FULLSCALE_4G, //!< 4G
    ACCEL_FULLSCALE_8G, //!< 8G
    ACCEL_FULLSCALE_16G //!< 16G
} accel_fullscale_t;

/**
 * Sample Frequency
 */

typedef enum
{
    ACCEL_SAMP_FREQ_1HZ,     //!< 1Hz  
    ACCEL_SAMP_FREQ_10HZ,    //!< 10Hz
    ACCEL_SAMP_FREQ_25HZ,    //!< 25Hz
    ACCEL_SAMP_FREQ_50HZ,    //!< 50Hz
    ACCEL_SAMP_FREQ_100HZ,   //!< 100Hz
    ACCEL_SAMP_FREQ_200HZ,   //!< 200Hz
    ACCEL_SAMP_FREQ_400HZ,   //!< 400Hz
} accel_sample_freq_t;

/**
 * Mode
 */
typedef enum
{
    ACCEL_MODE_NORMAL,      //!< normal
    ACCEL_MODE_LOW_POWER,   //!< low power
    ACCEL_MODE_POWER_DOWN   //!< power down
} accel_mode_t;

#define ACCEL_AXIS_DISABLE 0x00
#define ACCEL_AXIS_EN_X 0x01
#define ACCEL_AXIS_EN_Y 0x02
#define ACCEL_AXIS_EN_Z 0x04


typedef uint8_t accel_axis_en_t; //<! axis enable
typedef uint8_t accel_wom_thr; //!< Wake on Motion threshold

/**
 * @brief Accelerometer configuration
 */
typedef struct
{
    accel_fullscale_t fullscale; //!< fullscale
    accel_sample_freq_t samp_freq; //!< sample frequency
    accel_axis_en_t axis_en; //!< axis enable
    accel_wom_thr wake_accel_threshold; //!< Threshold value for the Wake on Motion Interrupt for x/y/z axes. LSB = 4mg (range is 0mg to 1020mg)
    accel_mode_t mode; //!< mode
} accelerometer_config_t;

/** 
 * Data
 */
typedef struct
{
    int32_t x;   //!< x-axis acceleration
    int32_t y;   //!< y-axis acceleration
    int32_t z;   //!< z-axis acceleration
} accelerometer_data_t;


/**
 * @}
 */

/**
 * Initialise accelerometer driver.
 *
 * @param[in] config: config used by initialisation function (specific to device)
 * @return @ref zos_result_t
 */
zos_result_t sensor_accelerometer_init(const accelerometer_config_t *config);

/**
 * Poll accelerometer to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref zos_result_t
 */
zos_result_t sensor_accelerometer_has_new_data(zos_bool_t *has_data);

/**
 * Get latest data from accelerometer sensor
 *
 * @param[out] data: Data from sensor in mG
 * @return @ref zos_result_t
 */
zos_result_t sensor_accelerometer_get_data(accelerometer_data_t *data);

