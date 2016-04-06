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
#include "hih6130.h"


#define HIH6130_TEMP_MAX_CODE        (uint32_t)(16384 - 1)
#define HIH6130_TEMP_MAX_OUTPUT      (uint32_t)((165) * SENSOR_THERMOMETER_OUTPUT_SCALE)
#define HIH6130_TEMP_OFFSET          (int32_t)(40 * SENSOR_THERMOMETER_OUTPUT_SCALE)



/*************************************************************************************************/
zos_result_t sensor_thermometer_init(const thermometer_config_t *config)
{
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
    return hih6130_temperature_read(raw_data, ZOS_FALSE);
}

/*************************************************************************************************/
int32_t sensor_thermometer_convert(uint16_t raw_value)
{
    return (uint32_t)(((raw_value * HIH6130_TEMP_MAX_OUTPUT) / HIH6130_TEMP_MAX_CODE) - HIH6130_TEMP_OFFSET);
}
