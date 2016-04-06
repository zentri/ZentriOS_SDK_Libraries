/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "zos.h"
#include "sensor/types/airquality/airquality.h"
#include "vz89.h"

/*************************************************************************************************/
zos_result_t sensor_airquality_init(const airquality_config_t *config)
{
    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t sensor_airquality_has_new_data(zos_bool_t *has_data)
{
    *has_data = ZOS_TRUE;
    return ZOS_SUCCESS;
}

/*************************************************************************************************/
/* Get latest data from air quality sensor */
zos_result_t sensor_airquality_get_data(airquality_data_t *data)
{
    zos_result_t result = ZOS_ERROR;
    sensor_data_t raw_quality;

    result = vz89_airquality_read(&raw_quality);
    if ( result != 0 )
    {
        // Error reading value. Do nothing
    }
    else if (raw_quality.voc_long < 13 || raw_quality.voc_long > 242)
    {
        // ZOS_LOG("Invalid tVOC readings (%u): values should be between 13 and 242!", raw_quality.voc_long);
    }
    else if (raw_quality.co2 < 13 || raw_quality.co2 > 242)
    {
        // ZOS_LOG("Invalid CO2 readings (%u): values should be between 13 and 242!", raw_quality.co2);
    }
    else
    {
        // D1: CO2_equ: [13...242] -> CO2_equ [ppm] = (D1 - 13) * (1600/229) + 400 =~ (D1 - 13) * 7 + 400
        data->co2_equ = (7 * (raw_quality.co2 - 13)) + 400;

        // D3: tVOC: [13...242] -> tVOC [ppb] = (D3 - 13) * (1000/229) =~ (D3 - 13) * 7
        data->tvoc = 7 * (raw_quality.voc_long - 13);
    }
    return result;
}
