/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#ifdef SENSOR_LIB_HYGROMETER

#include "sensor/types/hygrometer/hygrometer.h"



/*************************************************************************************************
 * This returns normalized hygrometer data. If this function is used then the
 * hygrometer driver sensor_api  must implement:
 * - sensor_hygrometer_read() : return value sesnor data
 * - sensor_hygrometer_convert() :  convert raw data to normalized value
 *
 * Note: This API is 'weak' and may be overridden by a application/library.
 *       If the API is overridden, then sensor_hygrometer_read() and sensor_hygrometer_convert()
 *       need not be implemented.
 */
WEAK zos_result_t sensor_hygrometer_get_data(hygrometer_data_t *data)
{
    zos_result_t result = ZOS_ERROR;

    uint16_t raw_data;
    if (!ZOS_FAILED(result, sensor_hygrometer_read(&raw_data, ZOS_FALSE)))
    {
        const uint32_t humidity = sensor_hygrometer_convert(raw_data);

        //cap value at 100.0% (some sensors can output >100%)
        if (humidity > 100*SENSOR_HYROMETER_OUTPUT_SCALE)
        {
            data->raw = 100*SENSOR_HYROMETER_OUTPUT_SCALE;
        }
        else
        {
            data->raw = humidity;
        }

        data->value.whole = humidity/SENSOR_HYROMETER_OUTPUT_SCALE;
        data->value.fraction = humidity%SENSOR_HYROMETER_OUTPUT_SCALE;

        result = ZOS_SUCCESS;
    }

    return result;
}

#endif
