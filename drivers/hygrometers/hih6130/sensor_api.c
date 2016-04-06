/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#if defined(SENSOR_LIB_HYGROMETER) && defined(DRIVER_HYGROMETER_HIH6130)

#include "zos.h"
#include "sensor/types/hygrometer/hygrometer.h"
#include "hih6130.h"


#define HIH6130_HUM_MAX_OUTPUT      (uint32_t)((100)*SENSOR_HYROMETER_OUTPUT_SCALE)
#define HIH6130_HUM_MAX_CODE        (uint32_t)((16384-1))




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
    return hih6130_humidity_read(raw_data, use_last_value);
}

/*************************************************************************************************
 * Input raw data from sensor, output humidity x HUM_OUTPUT_SCALE
 */
uint32_t sensor_hygrometer_convert(uint16_t raw_value)
{
    return (uint32_t)((raw_value * HIH6130_HUM_MAX_OUTPUT) / HIH6130_HUM_MAX_CODE);
}


#endif
