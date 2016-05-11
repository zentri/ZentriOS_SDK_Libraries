/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "zos.h"
#include "sensor/types/thermometer/thermometer.h"
#include "pmodgyro.h"

/*************************************************************************************************/
zos_result_t sensor_thermometer_init(const thermometer_config_t *config)
{
    if (pmodgyro_init(ZOS_FALSE) != ZOS_SUCCESS)
        return ZOS_ERROR;
    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t sensor_thermometer_has_new_data(zos_bool_t *has_data)
{
    *has_data = ZOS_TRUE;
    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t sensor_thermometer_read(uint16_t *raw_data)
{
    return pmodgyro_temperature_read(raw_data);
}

/*************************************************************************************************/
int32_t sensor_thermometer_convert(uint16_t raw_value)
{
	return (40 - raw_value)*SENSOR_THERMOMETER_OUTPUT_SCALE;
}
