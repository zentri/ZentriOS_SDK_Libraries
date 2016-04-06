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
#include "si7021.h"


#define SI7021_TEMP_SCALE_MULTIPLIER (uint32_t)(175.72 * SENSOR_THERMOMETER_OUTPUT_SCALE)
#define SI7021_TEMP_SCALE_DIVIDER    (uint32_t)(65536)
#define SI7021_TEMP_SCALE_OFFSET     (uint32_t)(46.85 * SENSOR_THERMOMETER_OUTPUT_SCALE)


/*************************************************************************************************/
zos_result_t sensor_thermometer_init(const thermometer_config_t *config)
{
    zos_result_t result;
    const si7021_config_t si7021_config =
    {
            .resolution = RES_RH_12BIT_TEMP_14_BIT,
            .heater_enable = SI7021_HEATER_DISABLE,
            .heater_current = SI7021_HEATER_CUR_OFF
    };

    if(ZOS_FAILED(result, si7021_init(&si7021_config)))
    {
    }

    return result;
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
    return si7021_temperature_read(raw_data, ZOS_FALSE, ZOS_FALSE);
}

/*************************************************************************************************/
int32_t sensor_thermometer_convert(uint16_t raw_value)
{
    return (int32_t)((int16_t)((raw_value * SI7021_TEMP_SCALE_MULTIPLIER)/SI7021_TEMP_SCALE_DIVIDER) - SI7021_TEMP_SCALE_OFFSET);
}
