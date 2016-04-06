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
 * @addtogroup  lib_sensor_airquality
 * @{
 */

typedef struct
{
    uint8_t unused;
} airquality_config_t;

/**
 * @brief Air quality data
 */
typedef struct
{
    uint16_t co2_equ; //!< Equivalent Carbon Dioxide CO2(equiv)
    uint16_t tvoc;    //!< Volatile Organic Compounds VOCs
} airquality_data_t;


/**
 * @}
 */

/**
 * Initialise air quality sensor driver.
 *
 * @param[in] config: config used by initialisation function
 * @return @ref zos_result_t
 */
zos_result_t sensor_airquality_init(const airquality_config_t *config);

/**
 * Poll air quality sensor to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref zos_result_t
 */
zos_result_t sensor_airquality_has_new_data(zos_bool_t *has_data);

/**
 * Get latest data from air quality sensor
 *
 * @param[out] data: air quality parameters
 * @return @ref zos_result_t
 */
zos_result_t sensor_airquality_get_data(airquality_data_t *data);
