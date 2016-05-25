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

#ifdef SENSOR_LIB_ACCELEROMETER
#include "sensor/types/accelerometer/accelerometer.h"
#endif
#ifdef SENSOR_LIB_HYGROMETER
#include "sensor/types/hygrometer/hygrometer.h"
#endif
#ifdef SENSOR_LIB_THERMOMETER
#include "sensor/types/thermometer/thermometer.h"
#endif
#ifdef SENSOR_LIB_GYROSCOPE
#include "sensor/types/gyroscope/gyroscope.h"
#endif
#ifdef SENSOR_LIB_MAGNETOMETER
#include "sensor/types/magnetometer/magnetometer.h"
#endif
#ifdef SENSOR_LIB_AIRQUALITY
#include "sensor/types/airquality/airquality.h"
#endif
#ifdef SENSOR_LIB_LIGHT
#include "sensor/types/light/light.h"
#endif
#ifdef SENSOR_LIB_CAPACITIVE_INPUT
#include "sensor/types/capacitive_input/capacitive_input.h"
#endif
#ifdef SENSOR_LIB_USER_SENSORS
#include "user_sensors.h"
#endif


/**
 * @addtogroup lib_sensor
 * @{
 */

/**
 * @brief Register a sensor type with the sensor library
 * @def SENSOR_LIB_REGISTER_SENSOR_TYPE(name, id)
 */
#define SENSOR_LIB_REGISTER_SENSOR_TYPE(name, id)                               \
    [SENSOR_ ## id] =                                                           \
{                                                                               \
    .init     = (sensor_init_prototype_t)    sensor_ ## name ## _init,          \
    .has_data = (sensor_has_data_prototype_t)sensor_ ## name ## _has_new_data,  \
    .get_data = (sensor_get_data_prototype_t)sensor_ ## name ## _get_data       \
}


/** 
 *  Sensor id enumeration
 */
typedef enum
{
#ifdef SENSOR_LIB_ACCELEROMETER
    SENSOR_ACCELEROMETER,   //!< Accelerometer
#endif
#ifdef SENSOR_LIB_HYGROMETER
    SENSOR_HYGROMETER,      //!< Hygrometer
#endif
#ifdef SENSOR_LIB_THERMOMETER
    SENSOR_THERMOMETER,     //!< Thermometer
#endif
#ifdef SENSOR_LIB_GYROSCOPE
    SENSOR_GYROSCOPE,       //!< Gyroscope
#endif
#ifdef SENSOR_LIB_MAGNETOMETER
    SENSOR_MAGNETOMETER,    //!< Magnetometer
#endif
#ifdef SENSOR_LIB_AIRQUALITY
    SENSOR_AIRQUALITY,      //!< Air Quality Sensor
#endif
#ifdef SENSOR_LIB_LIGHT
    SENSOR_LIGHT,           //!< Ambient Light Sensor
#endif
#ifdef SENSOR_LIB_CAPACITIVE_INPUT
	SENSOR_CAPACITIVE_INPUT,//!< Capacitive Input Buttons
#endif
#ifdef SENSOR_LIB_USER_SENSOR_IDS
    SENSOR_LIB_USER_SENSOR_IDS,
#endif
    SENSOR_COUNT
} sensor_id_t;

/* Each low level sensor driver must implement the following functions */
typedef zos_result_t (*sensor_init_prototype_t)(const void *config);
typedef zos_result_t (*sensor_has_data_prototype_t)(zos_bool_t *has_data);
typedef zos_result_t (*sensor_get_data_prototype_t)(void *data);



/**
 * Initialise sensor driver.
 *
 * @param[in] sensor_id: ID of sensor to initialise
 * @param[in] config: config used by initialisation function (specific to device)
 * @return @ref zos_result_t
 */
zos_result_t sensor_init(sensor_id_t sensor_id, const void *config);

/**
 * Poll sensor to see if new data is available
 *
 * @param[in] sensor_id: ID of sensor to initialise
 * @param[out] has_data: True if there is new data available
 * @return @ref zos_result_t
 */
zos_result_t sensor_has_new_data(sensor_id_t sensor_id, zos_bool_t *has_data);

/**
 * Get latest data from sensor
 *
 * @param[in] sensor_id: ID of sensor to initialise
 * @param[out] data: Data from sensor (specific to sensor device)
 * @return @ref zos_result_t
 */
zos_result_t sensor_get_data(sensor_id_t sensor_id, void *data);

/**
 * @}
 */
