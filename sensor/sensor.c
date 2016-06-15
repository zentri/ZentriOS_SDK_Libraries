/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "sensor.h"



static const struct
{
    const sensor_init_prototype_t init;
    const sensor_has_data_prototype_t has_data;
    const sensor_get_data_prototype_t get_data;
}
sensor_functions[] =
{
#ifdef SENSOR_LIB_ACCELEROMETER
        SENSOR_LIB_REGISTER_SENSOR_TYPE(accelerometer, ACCELEROMETER),
#endif
#ifdef SENSOR_LIB_HYGROMETER
        SENSOR_LIB_REGISTER_SENSOR_TYPE(hygrometer, HYGROMETER),
#endif
#ifdef SENSOR_LIB_THERMOMETER
        SENSOR_LIB_REGISTER_SENSOR_TYPE(thermometer, THERMOMETER),
#endif
#ifdef SENSOR_LIB_GYROSCOPE
        SENSOR_LIB_REGISTER_SENSOR_TYPE(gyroscope, GYROSCOPE),
#endif
#ifdef SENSOR_LIB_MAGNETOMETER
        SENSOR_LIB_REGISTER_SENSOR_TYPE(magnetometer, MAGNETOMETER),
#endif
#ifdef SENSOR_LIB_LIGHT
        SENSOR_LIB_REGISTER_SENSOR_TYPE(light, LIGHT),
#endif
#ifdef SENSOR_LIB_AIRQUALITY
        SENSOR_LIB_REGISTER_SENSOR_TYPE(airquality, AIRQUALITY),
#endif
#ifdef SENSOR_LIB_CAPACITIVE_INPUT
        SENSOR_LIB_REGISTER_SENSOR_TYPE(capacitive_input, CAPACITIVE_INPUT)
#endif
#ifdef SENSOR_LIB_REGISTER_USER_SENSORS
        SENSOR_LIB_REGISTER_USER_SENSORS
#endif
};


/* Input sensor and config */
zos_result_t sensor_init(sensor_id_t sensor_id, const void *config)
{
    if(sensor_id >= SENSOR_COUNT)
    {
        return ZOS_INVALID_ARG;
    }
    return sensor_functions[sensor_id].init(config);
}

/*************************************************************************************************/
/* Input sensor, output true if new data available*/
zos_result_t sensor_has_new_data(sensor_id_t sensor_id, zos_bool_t *has_data)
{
    *has_data = ZOS_FALSE;

    if(sensor_id >= SENSOR_COUNT)
    {
        return ZOS_INVALID_ARG;
    }
    return sensor_functions[sensor_id].has_data(has_data);
}

/*************************************************************************************************/
/* Input sensor, output data */
zos_result_t sensor_get_data(sensor_id_t sensor_id, void *data)
{
    if(sensor_id >= SENSOR_COUNT)
    {
        return ZOS_INVALID_ARG;
    }
    return sensor_functions[sensor_id].get_data(data);
}


