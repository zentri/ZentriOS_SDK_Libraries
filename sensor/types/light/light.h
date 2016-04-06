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
 * @addtogroup  lib_sensor_light
 * @{
 */

typedef struct
{
    uint8_t unused;
} light_config_t;

/**
 * @brief Ambient light data
 */
typedef struct
{
    uint32_t lux; //!< the ambient light value (LUX)
} light_data_t;


/**
 * @}
 */

/**
 * Initialise light sensor driver.
 *
 * @param[in] config: config used by initialisation function
 * @return @ref zos_result_t
 */
zos_result_t sensor_light_init(const light_config_t *config);

/**
 * Poll light sensor to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref zos_result_t
 */
zos_result_t sensor_light_has_new_data(zos_bool_t *has_data);

/**
 * Get latest data from light sensor
 *
 * @param[out] data: light in lux
 * @return @ref zos_result_t
 */
zos_result_t sensor_light_get_data(light_data_t *data);
