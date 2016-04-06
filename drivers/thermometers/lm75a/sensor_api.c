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
#include "lm75a.h"


#define LM75A_SCALE_FACTOR            (uint32_t)12 // .125 * 100


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
    return lm75a_temperature_read(raw_data);
}

/*************************************************************************************************/
int32_t sensor_thermometer_convert(uint16_t raw_value)
{
    if(raw_value & LM75A_TEMP_SIGN_BIT)
    {
        return (-raw_value) * LM75A_SCALE_FACTOR;
    }
    else
    {
        return raw_value * LM75A_SCALE_FACTOR;
    }
}
