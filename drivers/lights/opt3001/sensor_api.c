/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "opt3001.h"
#include "zos.h"
#include "sensor/types/light/light.h"

/*************************************************************************************************/
zos_result_t sensor_light_init(const light_config_t *config)
{
    return opt3001_init();
}

/*************************************************************************************************/
zos_result_t sensor_light_has_new_data(zos_bool_t *has_data)
{
    *has_data = ZOS_TRUE;
    return ZOS_SUCCESS;
}

/*************************************************************************************************/
/* Get latest data from light sensor */
zos_result_t sensor_light_get_data(light_data_t *data)
{
    zos_result_t result = ZOS_ERROR;
    uint16_t raw_lux;
    uint16_t exponent = 0;
    uint16_t mantissa = 0;

    result = opt3001_light_read(&raw_lux);

    // extract result & exponent data from raw readings
    mantissa = raw_lux&0x0FFF;
    exponent = (raw_lux>>12)&0x000F;

    // convert raw readings to LUX
    switch(exponent)
    {
        case 0: // step = 0.01 lux
            data->lux = mantissa / 100;
            break;
        case 1: // step = 0.02 lux
            data->lux = mantissa / 50;
            break;
        case 2: // step = 0.04 lux
            data->lux = mantissa / 25;
            break;
        case 3: // step = 0.08 lux
            data->lux = (2 * mantissa) / 25;
            break;
        case 4: // step = 0.16 lux
            data->lux = (4 * mantissa) / 25;
            break;
        case 5: // step = 0.32 lux
            data->lux = (8 * mantissa) / 25;
            break;
        case 6: // step = 0.64 lux
            data->lux = (16 * mantissa) / 25;
            break;
        case 7: // step = 1.28 lux
            data->lux = mantissa + ((7 * mantissa) / 25);
            break;
        case 8: // step = 2.56 lux
            data->lux = (2 * mantissa) + ((14 * mantissa) / 25);
            break;
        case 9: // step = 5.12 lux
            data->lux = (5 * mantissa) + ((3 * mantissa) / 25);
            break;
        case 10: // step = 10.24 lux
            data->lux = (10 * mantissa) + ((6 * mantissa) / 25);
            break;
        case 11: // step = 20.48 lux
            data->lux = (20 * mantissa) + ((12 * mantissa) / 25);
            break;
    }
    return result;
}
