/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "zos.h"
#include "sensor/types/hygrometer/hygrometer.h"
#include "hih5030.h"


#define HIH5030_HUM_SCALE_MULTIPLIER (uint32_t)(SENSOR_HYROMETER_OUTPUT_SCALE)
#define HIH5030_HUM_SCALE_DIVIDER    (uint32_t)(26)
#define HIH5030_HUM_SCALE_OFFSET     (uint32_t)(6204)



/*************************************************************************************************/
zos_result_t sensor_hygrometer_init(const hygrometer_config_t *config)
{
    return ZOS_SUCCESS;
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
    return hih5030_humidity_read(raw_data);
}

/*************************************************************************************************
 * Input raw data from sensor, output humidity x HUM_OUTPUT_SCALE
 */
uint32_t sensor_hygrometer_convert(uint16_t raw_value)
{
    return (uint32_t)(((raw_value * HIH5030_HUM_SCALE_MULTIPLIER) - HIH5030_HUM_SCALE_OFFSET) / HIH5030_HUM_SCALE_DIVIDER);
}

