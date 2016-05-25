
/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "ad7156.h"

static zos_result_t AD7156_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t AD7156_ReadRegWithSize(uint8_t Reg, uint8_t* Data, uint16_t size);
static zos_result_t AD7156_WriteReg(uint8_t WriteAddr, uint8_t Data);
static zos_result_t AD7156_WriteRegWithSize(uint8_t WriteAddr, uint8_t *Data, uint16_t size);

static const zos_i2c_device_t ZOS_I2C_AD7156 =
{
    .address = (uint16_t)AD7156_I2C_ADDR,
    .speed = I2C_CLOCK_HIGH_SPEED,
    .retries = (uint16_t)1U,
    .flags = I2C_FLAG_HEXIFY,
    .read_timeout = 40
};

/******************************************************************************/
/************************ Variables Declarations ******************************/
/******************************************************************************/
float ad7156Channel1Range = 2;
float ad7156Channel2Range = 2;
/******************************************************
*          Internal Function Definitions
******************************************************/
/******************************************************************************
* Function Name     : AD7156_ReadReg
* Description       : Generic I2C reading function
* Input             : Reg - Register Address
*
* Output            : Data Read
* Return         	: Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
static zos_result_t AD7156_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_AD7156, Reg, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}


/*******************************************************************************
* Function Name     : AD7156_WriteReg
* Description       : Generic I2C writing function
* Input             : WriteAddr - Register Address,
* 					  Data - Data to be written
* Output            : None
* Return         	: Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
static zos_result_t AD7156_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_AD7156, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}
/*******************************************************************************
* Function Name     : AD7156_ReadRegWithSize
* Description       : Generic I2C reading function
* Input             : Register Address, Buffer to hold data, Size to read
* Output            : Data Read
* Return         	: Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
static zos_result_t AD7156_ReadRegWithSize(uint8_t Reg, uint8_t* Data, uint16_t size)
{
    if (zn_i2c_master_read_reg(&ZOS_I2C_AD7156, Reg, Data, size) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : AD7156_WriteRegWithSize
* Description       : Generic I2C writing function
* Input             : Register Address, Data to be written, Size of data
* Output            : None
* Return         	: Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
static zos_result_t AD7156_WriteRegWithSize(uint8_t WriteAddr, uint8_t *Data, uint16_t size)
{
    if (zn_i2c_master_write_reg(&ZOS_I2C_AD7156, WriteAddr, Data, size) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_Init
* Description    : Initialize AD7156 in a ready state
* Input          : None
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_Init(void)
{
	zos_result_t ret = ZOS_SUCCESS;
	uint8_t chipid   = 0;

	// Check Chip ID
	ret = AD7156_ReadReg(AD7156_REG_CHIP_ID, &chipid);
	if(ret == ZOS_SUCCESS)
	{
		if(chipid != AD7156_DEFAULT_ID)
		{
			ret = ZOS_ERROR;
		}
	}
	return ret;
}
/*******************************************************************************
* Function Name  : AD7156_Reset
* Description    : Reset the device
* Input          : None
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_Reset(void)
{
    if(AD7156_WriteReg(AD7156_RESET_CMD, 1) != ZOS_SUCCESS)
    {
    	return ZOS_ERROR;
    }
    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : PMODGYRO_MagnGetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_GetStatusReg(uint8_t* val)
{
	// Write register with any value in order to move pointer to this register
	if (AD7156_ReadReg(AD7156_REG_STATUS, val) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
	return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_SetPowerMode
* Description    : Sets the converter mode of operation.
* Input          : Mode of operation option
*  				   Example: AD7156_CONV_MODE_IDLE - Idle
*                           AD7156_CONV_MODE_CONT_CONV  - Continuous conversion
*                           AD7156_CONV_MODE_SINGLE_CONV - Single conversion
*                           AD7156_CONV_MODE_PWR_DWN - Power-down
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_SetPowerMode(uint8_t pwrMode)
{
	uint8_t oldConfigReg = 0;
	uint8_t newConfigReg = 0;

    if(AD7156_ReadReg(AD7156_REG_CONFIG, &oldConfigReg) != ZOS_SUCCESS)
    {
    	return ZOS_ERROR;
    }
    oldConfigReg &= ~AD7156_CONFIG_MD(0x3);
    newConfigReg = oldConfigReg| AD7156_CONFIG_MD(pwrMode);
    if(AD7156_WriteReg(AD7156_REG_CONFIG, newConfigReg)!= ZOS_SUCCESS)
    {
    	return ZOS_ERROR;
    }
    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_ChannelState
* Description    : Enables or disables conversion on the selected channel.
* Input          : channel - Channel option.
* 				    			Example: AD7156_CHANNEL1
*                           			 AD7156_CHANNEL2
* 				   enableConv - The state of channel activity.
* 								Example: 0 - disable conversion on selected channel.
*                            			 1 - enable conversion on selected channel.
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_ChannelState(uint8_t channel, uint8_t enableConv)
{
    uint8_t oldConfigReg = 0;
    uint8_t newConfigReg = 0;
    uint8_t channelMask  = 0;

    channelMask = (channel == 1) ? AD7156_CONFIG_EN_CH1 : AD7156_CONFIG_EN_CH2;

    if(AD7156_ReadReg(AD7156_REG_CONFIG, &oldConfigReg) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    oldConfigReg &= ~channelMask;
    newConfigReg = oldConfigReg | (channelMask * enableConv);
    if(AD7156_WriteReg(AD7156_REG_CONFIG, newConfigReg)!= ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_SetRange
* Description    : Sets the input range of the specified channel.
* Input          : channel - Channel option:
* 				    		 Example: AD7156_CHANNEL1
*                            		  AD7156_CHANNEL2
* 				   range - Input range option.
*                   	   Example: AD7156_CDC_RANGE_2_PF   - 2pF input range.
*                            		AD7156_CDC_RANGE_0_5_PF - 0.5pF input range.
*                            		AD7156_CDC_RANGE_1_PF   - 1pF input range.
*                            		AD7156_CDC_RANGE_4_PF   - 4pF input range.
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_SetRange(unsigned channel, uint8_t range)
{
    uint8_t oldSetupReg = 0;
    uint8_t newSetupReg = 0;
    uint8_t regAddress  = 0;

    regAddress = (channel == 1) ? AD7156_REG_CH1_SETUP : AD7156_REG_CH2_SETUP;
    if(AD7156_ReadReg(regAddress, &oldSetupReg) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    oldSetupReg &= ~AD7156_CH1_SETUP_RANGE(0x3);
    newSetupReg = oldSetupReg | AD7156_CH1_SETUP_RANGE(range);

    if(AD7156_WriteReg(regAddress, newSetupReg)!= ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    /* Update global variables that hold range information. */
    if(channel == 1)
    {
    	AD7156_GetRange(channel, &ad7156Channel1Range);
    }
    else
    {
        AD7156_GetRange(channel, &ad7156Channel2Range);
    }
    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_GetRange
* Description    : Reads the range bits from the device and returns the range in pF.
* Input          : channel - Channel option:
* 				   			 Example:   AD7156_CHANNEL1
*                           			AD7156_CHANNEL2
* Output         : The capacitive input range(pF).
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_GetRange(unsigned channel, float *range)
{
    uint8_t setupReg    = 0;
    uint8_t regAddress  = 0;

    regAddress = (channel == 1) ? AD7156_REG_CH1_SETUP : AD7156_REG_CH2_SETUP;
    if(AD7156_ReadReg(regAddress, &setupReg) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    setupReg = (setupReg & AD7156_CH1_SETUP_RANGE(0x3)) >> 6;
    switch(setupReg)
    {
        case AD7156_CDC_RANGE_2_PF:
            *range =  2.0;
            break;
        case AD7156_CDC_RANGE_0_5_PF:
            *range = 0.5;
            break;
        case AD7156_CDC_RANGE_1_PF:
            *range =  1.0;
            break;
        case AD7156_CDC_RANGE_4_PF:
            *range =  4.0;
            break;
    }
    /* Update global variables that hold range information. */
    if(channel == 1)
    {
        ad7156Channel1Range = *range;
    }
    else
    {
        ad7156Channel2Range = *range;
    }

    return ZOS_SUCCESS;
}
/*******************************************************************************
* Function Name  : AD7156_SetThresholdMode
* Description    : RSelects the threshold mode of operation.
* Input          : thrMode  - Output comparator mode.
*                  			Example:  	AD7156_THR_MODE_NEGATIVE
*                            			AD7156_THR_MODE_POSITIVE
*                            			AD7156_THR_MODE_IN_WINDOW
*                            			AD7156_THR_MODE_OU_WINDOW
*                  thrFixed - Selects the threshold mode.
*                  			  Example:  AD7156_ADAPTIVE_THRESHOLD
*                            			AD7156_FIXED_THRESHOLD
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_SetThresholdMode(uint8_t thrMode, uint8_t thrFixed)
{
    uint8_t oldConfigReg = 0;
    uint8_t newConfigReg = 0;

    if(AD7156_ReadReg(AD7156_REG_CONFIG, &oldConfigReg) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    oldConfigReg &= ~(AD7156_CONFIG_THR_FIXED | AD7156_CONFIG_THR_MD(0x3));
    newConfigReg = oldConfigReg |
                   (AD7156_CONFIG_THR_FIXED * thrFixed) |
                   (AD7156_CONFIG_THR_MD(thrMode));
    if(AD7156_WriteReg(AD7156_REG_CONFIG, newConfigReg)!= ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_SetThreshold
* Description    : Writes to the threshold register when threshold fixed mode is
* 				   enabled.
* Input          : channel - Channel option.
*                 			 Example:   AD7156_CHANNEL1
*                            			AD7156_CHANNEL2
*                  pFthr   - The threshold value in picofarads(pF).
*                  The value must not be out of the selected input range.
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_SetThreshold(uint8_t channel, float pFthr)
{
    uint8_t  thrRegAddress  = 0;
    uint16_t rawThr         = 0;
    float  range            = 0;

    thrRegAddress = (channel == 1) ? AD7156_REG_CH1_SENS_THRSH_H :
                                     AD7156_REG_CH2_SENS_THRSH_H;
    if(AD7156_GetRange(channel, &range)!= ZOS_SUCCESS)
    {
    	return ZOS_ERROR;
    }
    rawThr = (uint16_t)((pFthr * 0xA000 / range) + 0x3000);
    if(rawThr > 0xD000)
    {
        rawThr = 0xD000;
    }
    else if(rawThr < 0x3000)
    {
        rawThr = 0x3000;
    }
    if(AD7156_WriteRegWithSize(thrRegAddress, (uint8_t*)&rawThr, 2)!= ZOS_SUCCESS)
  	{
  		return ZOS_ERROR;
  	}
    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_SetSensitivity
* Description    : Writes a value(pF) to the sensitivity register. This functions
* 				   should be used when adaptive threshold mode is selected.
* Input          : channel - Channel option.
*                  	 		 Example:  AD7156_CHANNEL1
*                              		   AD7156_CHANNEL2
*                  pFsensitivity - The sensitivity value in picofarads(pF).
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_SetSensitivity(uint8_t channel, float pFsensitivity)
{
    uint8_t  sensitivityRegAddr = 0;
    uint16_t rawSensitivity     = 0;
    float range = 0;

    sensitivityRegAddr = (channel == 1) ? AD7156_REG_CH1_SENS_THRSH_H :
                                          AD7156_REG_CH2_SENS_THRSH_H;
    range = (channel == 1) ? ad7156Channel1Range : ad7156Channel2Range;
    rawSensitivity = (uint16_t)(pFsensitivity * 0xA00 / range);
    rawSensitivity = (rawSensitivity << 4) & 0x0FF0;
    if(AD7156_WriteRegWithSize(sensitivityRegAddr, (uint8_t*)&rawSensitivity, 2)!= ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_SetTimeout
* Description    : Writes Timeout.
* Input          : channel - Channel option.
*                  	 		 Example:  AD7156_CHANNEL1
*                              		   AD7156_CHANNEL2
*                  	 		 Example:  AD7156_CHANNEL1
*                              		   AD7156_CHANNEL2
*                  AprRecTimeout - Timeout MSB 4bit Apr timeout and LSB 4 bits Rec Timeout.
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_SetTimeout(uint8_t channel, uint8_t AprRecTimeout)
{
    uint8_t  TimeoutRegAddr = 0;

    TimeoutRegAddr = (channel == 1) ? AD7156_REG_CH1_TMO_THRSH_L :
    								 AD7156_REG_CH2_TMO_THRSH_L;
    if(AD7156_WriteReg(TimeoutRegAddr, AprRecTimeout)!= ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    return ZOS_SUCCESS;
}
/*******************************************************************************
* Function Name  : AD7156_EnableHysteresis
* Description    : Enable or Disable Hysteresis in adaptive threshold mode
* Input          : channel - Channel option.
*                  	 		 Example:  AD7156_CHANNEL1
*                              		   AD7156_CHANNEL2
*                  	 		 Example:  AD7156_CHANNEL1
*                              		   AD7156_CHANNEL2
*                  HystEn - Enable or Disable.
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_EnableHysteresis(uint8_t channel, zos_bool_t HystEn)
{
    uint8_t oldSetupReg = 0;
    uint8_t newSetupReg = 0;
    uint8_t regAddress  = 0;
    uint8_t cfgValue = 0;

    if(HystEn == ZOS_TRUE)
    {
    	cfgValue = 1;
    }
    regAddress = (channel == 1) ? AD7156_REG_CH1_SETUP : AD7156_REG_CH2_SETUP;
    if(AD7156_ReadReg(regAddress, &oldSetupReg) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    oldSetupReg &= ~AD7156_CH1_SETUP_HYST1;
    newSetupReg = oldSetupReg | (cfgValue << AD7156_nHyst);
    if(AD7156_WriteReg(regAddress, newSetupReg)!= ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_ReadChannelData
* Description    : Reads a 12-bit sample from the selected channel.
* Input          : channel - Channel option.
*                  			 Example:  AD7156_CHANNEL1
*                            		   AD7156_CHANNEL2
* Output         : Conversion result form the selected channel
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_ReadChannelData(uint8_t channel, uint16_t *chResult)
{
    uint8_t  regData[2] = {0, 0};
    uint8_t  chAddress  = 0;

    if(channel == 1)
    {
        chAddress = AD7156_REG_CH1_DATA_H;
    }
    else
    {
        chAddress = AD7156_REG_CH2_DATA_H;
    }
    if(AD7156_ReadRegWithSize(chAddress, regData, 2)!= ZOS_SUCCESS)
    {
    	*chResult = 0;
    	return ZOS_ERROR;
    }
    else
    {

    	*chResult = (regData[0] << 8) + regData[1];
    }
    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_WaitReadChannelData
* Description    : Waits for a finished CDC conversion and reads a 12-bit sample
*        		   from the selected channel.
* Input          : channel - Channel option.
*                  			 Example:  AD7156_CHANNEL1
*                            		   AD7156_CHANNEL2
* Output         : Conversion result form the selected channel
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_WaitReadChannelData(uint8_t channel, uint16_t *chResult)
{
    uint8_t  regData[2] = {0, 0};
    uint8_t  status     = 0;
    uint8_t  chRdyMask  = 0;
    uint8_t  chAddress  = 0;

    if(channel == 1)
    {
        chRdyMask = AD7156_STATUS_RDY1;
        chAddress = AD7156_REG_CH1_DATA_H;
    }
    else
    {
        chRdyMask = AD7156_STATUS_RDY2;
        chAddress = AD7156_REG_CH2_DATA_H;
    }
    do
    {
    	if(AD7156_ReadReg(AD7156_REG_STATUS, &status)!= ZOS_SUCCESS)
    	{
    		return ZOS_ERROR;
    	}
    }while((status & chRdyMask) != 0);

    if(AD7156_ReadRegWithSize(chAddress, regData, 2) != ZOS_SUCCESS)
    {
    	return ZOS_ERROR;
    }
    *chResult = (regData[0] << 8) + regData[1];
    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_ReadChannelCapacitance
* Description    : Reads a sample the selected channel and converts the data to
*        		   picofarads(pF)
* Input          : channel - Channel option.
*                  			 Example:  AD7156_CHANNEL1
*                            		   AD7156_CHANNEL2
* Output         : Conversion result form the selected channel as picofarads(pF).
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t AD7156_ReadChannelCapacitance(uint8_t channel, float *pFdata)
{
    uint16_t rawCh = 0;
    float chRange = 0;

    chRange = (channel == 1) ? ad7156Channel1Range : ad7156Channel2Range;
    if(AD7156_ReadChannelData(channel, &rawCh) != ZOS_SUCCESS)
    {
    	return ZOS_ERROR;
    }

    if(rawCh < 0x3000)
    {
        rawCh= 0x3000;
    }
    else if(rawCh > 0xD000)
    {
        rawCh = 0xD000;
    }
    *pFdata = (((rawCh) - 0x3000) * chRange) / 0xA000;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : AD7156_WaitReadChannelCapacitance
* Description    : Waits for a finished CDC conversion the selected channel, reads a
*        		   sample and converts the data to picofarads(pF).
* Input          : channel - Channel option.
*                  			 Example:  AD7156_CHANNEL1
*                            		   AD7156_CHANNEL2
* Output         : Conversion result form the selected channel as picofarads(pF).
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
* *******************************************************************************/
/*
Data 				Input Capacitance
0x0000				Under range (below 0 pF)
0x3000				Zero scale (0 pF)
0x5800				Quarter scale (+0.5 pF)—auto-DAC step down
0x8000				Midscale (+1 pF)
0xA800				Three-quarter scale (+1.5 pF)—auto-DAC step up
0xD000				Full scale (+2 pF)
0xFFF0				Over range (above +2 pF)
*/
zos_result_t AD7156_WaitReadChannelCapacitance(uint8_t channel, float *pFdata)
{
    uint16_t rawCh = 0;
    float chRange = 0;

    chRange = (channel == 1) ? ad7156Channel1Range : ad7156Channel2Range;
    if(AD7156_WaitReadChannelData(channel, &rawCh) != ZOS_SUCCESS)
    {
    	return ZOS_ERROR;
    }
    if(rawCh < 0x3000)
    {
        rawCh= 0x3000;
    }
    else if(rawCh > 0xD000)
    {
        rawCh = 0xD000;
    }
    *pFdata = (((rawCh) - 0x3000) * chRange) / 0xA000;
    return ZOS_SUCCESS;
}
