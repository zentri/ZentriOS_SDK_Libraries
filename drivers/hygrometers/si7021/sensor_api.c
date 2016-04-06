/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#if defined(SENSOR_LIB_HYGROMETER) && defined(DRIVER_HYGROMETER_SI7021)

#include "zos.h"
#include "sensor/types/hygrometer/hygrometer.h"
#include "si7021.h"



#define SI7021_HUM_SCALE_MULTIPLIER (uint32_t)(125*SENSOR_HYROMETER_OUTPUT_SCALE)
#define SI7021_HUM_SCALE_DIVIDER    (uint32_t)(65536)
#define SI7021_HUM_SCALE_OFFSET     (uint32_t)(6*SENSOR_HYROMETER_OUTPUT_SCALE)





/*************************************************************************************************/
zos_result_t sensor_hygrometer_init(const hygrometer_config_t *config)
{
    zos_result_t result = ZOS_ERROR;
    si7021_config_t si7021_config = {RES_RH_12BIT_TEMP_14_BIT, SI7021_HEATER_DISABLE, SI7021_HEATER_CUR_OFF};

    if(si7021_init(&si7021_config) == ZOS_SUCCESS)
    {
        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
zos_result_t sensor_hygrometer_has_new_data(zos_bool_t *has_data)
{
    *has_data = ZOS_TRUE;
    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t sensor_hygrometer_read(uint16_t *raw_data, zos_bool_t use_last_value)
{
    return si7021_humidity_read(raw_data, use_last_value);
}

/*************************************************************************************************
 * Input raw data from sensor, output humidity x HUM_OUTPUT_SCALE
 */
uint32_t sensor_hygrometer_convert(uint16_t raw_value)
{
    return (uint32_t)(((raw_value * SI7021_HUM_SCALE_MULTIPLIER)/SI7021_HUM_SCALE_DIVIDER) - SI7021_HUM_SCALE_OFFSET);
}



#endif
