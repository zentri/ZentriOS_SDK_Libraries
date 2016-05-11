/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "../../../sensor/types/capacitativebuttons/capacitativebuttons.h"
#include "../../capacitativebuttons/pmodcdc1/pmodcdc1.h"
#include "zos.h"

/*************************************************************************************************/
zos_result_t sensor_capacitativebuttons_init(const capacitativebuttons_config_t *config)
{
	zos_bool_t ret = ZOS_ERROR;
	if((PMODCDC1_ChannelState(1, config->ch1_enable) == ZOS_SUCCESS) &&
		(PMODCDC1_ChannelState(2, config->ch2_enable) == ZOS_SUCCESS) &&
		(PMODCDC1_SetThresholdMode(config->output_comparator_mode, config->threshold_mode) == ZOS_SUCCESS)  &&
		(PMODCDC1_SetPowerMode(config->op_mode) == ZOS_SUCCESS) &&
		(PMODCDC1_SetRange(1, config->ch1_input_range) == ZOS_SUCCESS) &&
		(PMODCDC1_SetRange(2, config->ch2_input_range) == ZOS_SUCCESS))
    {
    	ret = ZOS_SUCCESS;
    	// In adaptive threshold mode
    	if(config->threshold_mode == ADAPTIVE_THRESHOLD_MODE)
    	{
    		// Set sensitivity
    		if(config->ch1_sensitivity > 0.0)
    		{
    			ret = PMODCDC1_SetSensitivity(1, config->ch1_sensitivity);
    		}
    		if(ret == ZOS_SUCCESS && config->ch2_sensitivity > 0.0)
    		{
    			ret = PMODCDC1_SetSensitivity(2, config->ch2_sensitivity);
    		}
    		// Set Timeout
			if(ret == ZOS_SUCCESS &&  config->ch1_adaptive_timeout > 0)
			{
				ret = PMODCDC1_SetTimeout(1, config->ch1_adaptive_timeout);
			}
			if(ret == ZOS_SUCCESS &&  config->ch2_adaptive_timeout > 0)
			{
				ret = PMODCDC1_SetTimeout(2, config->ch2_adaptive_timeout);
			}
    	}
    	else
    	{
    		// In Fixed threshold mode
    		// Set Threshold
    		if(ret == ZOS_SUCCESS && config->ch1_threshold > 0.0)
			{
				ret = PMODCDC1_SetThreshold(1, config->ch1_threshold);
			}
			if(ret == ZOS_SUCCESS && config->ch2_threshold > 0.0)
			{
				ret = PMODCDC1_SetThreshold(2, config->ch2_threshold);
			}
    	}
    	// Check Chip ID
    	if(ret == ZOS_SUCCESS)
    	{
    		ret = PMODCDC1_Init();
    	}
    }
    return ret;
}

/*************************************************************************************************/
zos_result_t sensor_capacitativebuttons_has_new_data(zos_bool_t *has_data)
{
    zos_result_t result = ZOS_SUCCESS;
	*has_data = ZOS_TRUE;
	return result;
}

/*************************************************************************************************/
/* Get latest data from pmodcdc1 sensor */
zos_result_t sensor_capacitativebuttons_get_data(capacitativebuttons_data_t *data)
{
    zos_result_t result = ZOS_SUCCESS;
    PMODCDC1_data_t read_data = { 0 };

    if (PMODCDC1_WaitReadChannelCapacitance(1, (float*)&read_data.adjusted_ch1_cap) != ZOS_SUCCESS)
    {
        result = ZOS_ERROR;
    }
    else if (PMODCDC1_WaitReadChannelCapacitance(2, (float*)&read_data.adjusted_ch2_cap) != ZOS_SUCCESS)
    {
        result = ZOS_ERROR;
    }
    data->adjusted_ch1_cap = read_data.adjusted_ch1_cap;
    data->adjusted_ch2_cap = read_data.adjusted_ch2_cap;
    return result;
}
