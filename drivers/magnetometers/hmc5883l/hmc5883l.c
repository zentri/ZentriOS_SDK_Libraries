/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "hmc5883l.h"

static zos_result_t HMC5883L_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t HMC5883L_WriteReg(uint8_t WriteAddr, uint8_t Data);
static int getData(int reg1, int reg2);

static const zos_i2c_device_t ZOS_I2C_HMC5883L =
{
    .address = (uint16_t)HMC5883L_SLAVE_ADDRESS,
    .speed = I2C_CLOCK_HIGH_SPEED,
    .retries = (uint16_t)1U,
    .flags = I2C_FLAG_HEXIFY,
    .read_timeout = 40
};

/*******************************************************************************
* Function Name  : HMC5883L_MagnInit
* Description    : Sets the MPU9250 I2C Bypass Enable
* Input          : MPU9250_BYPASS_DISABLED/MPU9250_BYPASS_ENABLED
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t HMC5883L_MagnInit(void)
{
	zos_result_t result = ZOS_SUCCESS;

	if(HMC5883L_WriteReg(HMC5883L_CONFIG_REG_A, HMC5883L_DEFAULT_CONFIG_VALUE)!= ZOS_SUCCESS)
	{
		result = ZOS_ERROR;
	}
	else if(HMC5883L_WriteReg(HMC5883L_CONFIG_REG_B, HMC5883L_DEVICE_GAIN_DEFAULT)!=ZOS_SUCCESS)
	{
		result = ZOS_ERROR;
	}
	else if(HMC5883L_WriteReg(HMC5883L_CONFIG_MODE, HMC5883L_OPERATION_MODE_CONTINUOUS)!=ZOS_SUCCESS)
	{
		result = ZOS_ERROR;
	}
	return result;
}

/*******************************************************************************
* Function Name  : HMC5883L_MagnGetAxesRaw
* Description    : Read the Magnetometer Values Output Registers
* Input          : buffer to empty by AxesRaw_t Typedef
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t HMC5883L_MagnGetAxesRaw(HMC5883L_MagnAxesRaw_t* buff)
{
	uint8_t data[6];
	if (zn_i2c_master_read_reg(&ZOS_I2C_HMC5883L, HMC5883L_DATA_XOUT_H, data, 6) != ZOS_SUCCESS)
	{
		return ZOS_ERROR;
	}
	buff->HMC5883L_MAGN_AXIS_X = (int16_t)getData(data[0], data[1]);
	buff->HMC5883L_MAGN_AXIS_Z = (int16_t)getData(data[2], data[3]);
	buff->HMC5883L_MAGN_AXIS_Y = (int16_t)getData(data[4], data[5]);
	return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : twosToBin
* Description    : This function converts a negative twos compliment value and
* 				   performs a bitwise flip and subtracts one to return the
* 				   positive integer value.
* Input          : input
* Output         : None
* Return         : 16 bit unsigned integer with the positive value of the
* 				   negative twos compliment
*******************************************************************************/
static int twosToBin(int input)
{
	//flip all 11 bits
	input = input ^ 0x07ff;
	//subtract 1 to get binary
	input = input - 1;
	return input;
}

/*******************************************************************************
* Function Name  : HMC5883L_MagnGetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t HMC5883L_MagnGetStatusReg(uint8_t* val)
{
	// Write register with any value in order to move pointer to this register
	HMC5883L_WriteReg(HMC5883L_STATUS, 0);
	if (HMC5883L_ReadReg(HMC5883L_STATUS, val) != ZOS_SUCCESS)
		return ZOS_ERROR;

	 return ZOS_SUCCESS;
}

/******************************************************
*          Internal Function Definitions
******************************************************/
/*******************************************************************************
* Function Name  : getData
* Description    : Get Data is used to get the 11 bit x,y,z that come from 2
* 				   registers from the compass. The function spits out a signed
* 				   integer value.
* Input          : reg1 - first register to read from. the high data value which
* 				   contains the 3 MSBs
* 				   reg2 - second register to read from. the low data value which
* 				   contains the 8 LSBs
* Output         : None
* Return         : Int result - an integer value of the combined data in the two
* 				   registers
*******************************************************************************/
static int getData(int reg1, int reg2)
{
	//create variable to be returned
	int buffer = 0;
	int result = 0;
	int sign = 0;
	//store Data high into result
	buffer = reg1;
	//Shift over 8 to make room for lower bits
	buffer = buffer << 8;
	//store Data low into 8 LSB
	buffer = buffer | reg2;
	// If negative
	if(buffer >= 0x8000)
	{
		sign = 1;
		buffer = twosToBin(buffer);
	}
	//Get rid of first 4 bits
	result = buffer & 0x07ff;
	if(sign == 1)
	{
		result = 0 - result;
	}
	return result;
}
/*******************************************************************************
* Function Name     : HMC5883L_ReadReg
* Description       : Generic I2C reading function
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t HMC5883L_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_HMC5883L, Reg, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : HMC5883L_WriteReg
* Description       : Generic I2C writing function
* Input             : Register Address, Data to be written
* Output            : None
* Return            : None
*******************************************************************************/
static zos_result_t HMC5883L_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_HMC5883L, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}
