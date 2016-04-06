/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "hih5030.h"


const zos_adc_config_t const adc_config =
{
    .resolution = 12,
    .sampling_cycle = 12,
    .gain = 1
};


/*************************************************************************************************/
/* Measure and read the relative humidity */
zos_result_t hih5030_humidity_read(uint16_t* humidity)
{
    zos_result_t result = ZOS_SUCCESS;

    if (zn_adc_direct_init(PLATFORM_HYGROMETER_ADC, &adc_config)       != ZOS_SUCCESS ||
        zn_adc_direct_sample(PLATFORM_HYGROMETER_ADC, humidity) != ZOS_SUCCESS)
    {
        result = ZOS_ERROR;
    }

    return result;
}

