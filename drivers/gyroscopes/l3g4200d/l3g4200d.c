/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "l3g4200d.h"

static zos_result_t L3G4200D_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t L3G4200D_WriteReg(uint8_t WriteAddr, uint8_t Data);

static const zos_i2c_device_t ZOS_I2C_L3G4200D =
{
    .address = (uint16_t)L3G4200D_SLAVE_ADDRESS,
    .speed = I2C_CLOCK_HIGH_SPEED,
    .retries = (uint16_t)1U,
    .flags = I2C_FLAG_HEXIFY,
    .read_timeout = 40
};

/*******************************************************************************
* Function Name  : L3G4200D_Init
* Description    : Initialize L3G4200D in a ready state
* Input          : None
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t L3G4200D_Init()
{
	zos_result_t result = ZOS_SUCCESS;
	uint8_t initvalue = 0;

	// Turn on the sensor and enable X, Y, and Z (ODR = 100 Hz)
	initvalue = PMOD_GYRO_REG1_PD | PMOD_GYRO_REG1_ZEN | PMOD_GYRO_REG1_YEN | PMOD_GYRO_REG1_XEN;

	if(L3G4200D_WriteReg(PMOD_GYRO_CTRL_REG3, 0) != ZOS_SUCCESS)
	{
		result = ZOS_ERROR;
	}
	else if(L3G4200D_WriteReg(PMOD_GYRO_CTRL_REG1, initvalue) != ZOS_SUCCESS)
	{
		result = ZOS_ERROR;
	}
	return result;
}

/*******************************************************************************
* Function Name  : L3G4200D_SetFullScale
* Description    : Sets the FXAS21002C FullScale
* Input          : GYRO_FULLSCALE_250/GYRO_FULLSCALE_500/GYRO_FULLSCALE_1000/GYRO_FULLSCALE_2000
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t L3G4200D_SetFullScale(L3G4200D_Fullscale_t fs)
{
  uint8_t value;

  if( L3G4200D_ReadReg(PMOD_GYRO_CTRL_REG4, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xCF;
  value |= (fs << 4);

  if( L3G4200D_WriteReg(PMOD_GYRO_CTRL_REG4, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}
/*******************************************************************************
* Function Name  : L3G4200D_ReadTemperature
* Description    : Read the temperature from sensor
* Input          : Pointer to uint8_t integer to hold temperature
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
/*************************************************************************************************/
/* Read the temperature (get last measurement or take new sample) */
zos_result_t L3G4200D_ReadTemperature(uint8_t *temperature)
{
	uint8_t temp;
	if (L3G4200D_ReadReg(PMOD_GYRO_OUT_TEMP, &temp) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
	*temperature = (uint8_t)temp;
    return ZOS_SUCCESS;;
}

/*******************************************************************************
* Function Name  : L3G4200D_GetAxesRaw
* Description    : Read the Gyroscope Values Output Registers
* Input          : buffer to empty by AxesRaw_t Typedef
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t L3G4200D_GetAxesRaw(L3G4200D_AxesRaw_t* buff)
{
	uint8_t data[2] ={0,0};
	if (L3G4200D_ReadReg(PMOD_GYRO_OUT_X_L, &data[0]) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
	if (L3G4200D_ReadReg(PMOD_GYRO_OUT_X_H, &data[1]) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
	buff->L3G4200D_GYRO_AXIS_X = data[1]<<8 | data[0] ;
	if (L3G4200D_ReadReg(PMOD_GYRO_OUT_Y_L, &data[0]) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
	if (L3G4200D_ReadReg(PMOD_GYRO_OUT_Y_H, &data[1]) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
	buff->L3G4200D_GYRO_AXIS_Y = data[1]<<8 | data[0] ;
	if (L3G4200D_ReadReg(PMOD_GYRO_OUT_Z_L, &data[0]) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
	if (L3G4200D_ReadReg(PMOD_GYRO_OUT_Z_H, &data[1]) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
	buff->L3G4200D_GYRO_AXIS_Z = data[1]<<8 | data[0] ;

	return ZOS_SUCCESS;
}
/*******************************************************************************
* Function Name  : L3G4200D_GetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t L3G4200D_GetStatusReg(uint8_t* val)
{
	// Write register with any value in order to move pointer to this register
	if (L3G4200D_ReadReg(PMOD_GYRO_STATUS_REG, val) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
	return ZOS_SUCCESS;
}

/******************************************************
*          Internal Function Definitions
******************************************************/
/*******************************************************************************
* Function Name     : L3G4200D_ReadReg
* Description       : Generic I2C reading function
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t L3G4200D_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_L3G4200D, Reg, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : L3G4200D_WriteReg
* Description       : Generic I2C writing function
* Input             : Register Address, Data to be written
* Output            : None
* Return            : None
*******************************************************************************/
static zos_result_t L3G4200D_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_L3G4200D, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}
