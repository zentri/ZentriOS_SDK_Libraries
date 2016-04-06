/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#ifdef SENSOR_LIB_THERMOMETER


#include "sensor/types/thermometer/thermometer.h"



/*************************************************************************************************
 * This returns normalized thermometer data. If this function is used then the
 * thermometer driver sensor_api  must implement:
 * - sensor_thermometer_read() : return value sesnor data
 * - sensor_thermometer_convert() :  convert raw data to normalized value
 *
 * Note: This API is 'weak' and may be overridden by a application/library.
 *       If the API is overridden, then sensor_thermometer_read() and sensor_thermometer_convert()
 *       need not be implemented.
 */
WEAK zos_result_t sensor_thermometer_get_data(thermometer_data_t *data)
{
    zos_result_t result;
    uint16_t raw_data;

    if (!ZOS_FAILED(result, sensor_thermometer_read(&raw_data)))
    {

        const int32_t temp = sensor_thermometer_convert(raw_data);
        data->raw = temp;
        data->value.whole = temp / SENSOR_THERMOMETER_OUTPUT_SCALE;
        data->value.fraction = temp % SENSOR_THERMOMETER_OUTPUT_SCALE;
        if(temp < 0)
        {
            data->value.fraction *= -1;
        }
    }
    else
    {
        memset(data, 0xFF, sizeof(thermometer_data_t));
    }

    return result;
}


#endif
