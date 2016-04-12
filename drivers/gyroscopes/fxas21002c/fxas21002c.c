/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "fxas21002c.h"
#include "zos.h"


static const zos_i2c_device_t ZOS_I2C_FXAS21002C =
{
    .port    = PLATFORM_STD_I2C,
    .address = FXAS21002C_SLAVE_ADDRESS,
    .speed   = I2C_CLOCK_STANDARD_SPEED,
    .retries = (uint16_t)3U,
    .flags   = I2C_FLAG_HEXIFY
};


static zos_result_t FXAS21002C_Gyro_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t FXAS21002C_Gyro_WriteReg(uint8_t WriteAddr, uint8_t Data);

/*******************************************************************************
* Function Name  : FXAS21002C_GyroSetFullScale
* Description    : Sets the FXAS21002C FullScale
* Input          : GYRO_FULLSCALE_250/GYRO_FULLSCALE_500/GYRO_FULLSCALE_1000/GYRO_FULLSCALE_2000
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXAS21002C_GyroSetFullScale(FXAS21002C_GyroFullscale_t fs)
{
  uint8_t value;

  if( FXAS21002C_Gyro_ReadReg(FXAS21002C_CTRL_REG0, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xFC;
  value |= (fs << FXAS21002C_FSS);

  if( FXAS21002C_Gyro_WriteReg(FXAS21002C_CTRL_REG0, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXAS21002C_GyroSetODR
* Description    : Sets FXAS21002C Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXAS21002C_GyroSetODR(FXAS21002C_ODR_t odr_value)
{
  uint8_t value;

  if( FXAS21002C_Gyro_ReadReg(FXAS21002C_CTRL_REG1, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xE3;
  value |= (odr_value << FXAS21002C_ODR);

  if( FXAS21002C_Gyro_WriteReg(FXAS21002C_CTRL_REG1, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXAS21002C_GyroSetOperatingMode
* Description    : Sets the FXAS21002C Operating Mode
* Input          : STANDBY/ACTIVE choice
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXAS21002C_GyroSetOperatingMode(FXAS21002C_OperatingMode_t mode)
{
  uint8_t value;

  if( FXAS21002C_Gyro_ReadReg(FXAS21002C_CTRL_REG1, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xFD;
  value |= (mode << FXAS21002C_OPERATING_MODE);

  if( FXAS21002C_Gyro_WriteReg(FXAS21002C_CTRL_REG1, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXAS21002C_GyroGetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXAS21002C_GyroGetStatusReg(uint8_t* val)
{
    if( FXAS21002C_Gyro_ReadReg(FXAS21002C_STATUS, val) != ZOS_SUCCESS )
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}


/*******************************************************************************
* Function Name  : FXAS21002C_GyroGetAxesRaw
* Description    : Read the Gyroscope Values Output Registers
* Input          : buffer to empty by AxesRaw_t Typedef
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXAS21002C_GyroGetAxesRaw(FXAS21002C_GyroAxesRaw_t* buff)
{
    uint8_t valueL, valueH;

    if( FXAS21002C_Gyro_ReadReg(FXAS21002C_GYRO_XOUT_H, &valueH) != ZOS_SUCCESS )
      return ZOS_ERROR;

    if( FXAS21002C_Gyro_ReadReg(FXAS21002C_GYRO_XOUT_L, &valueL) != ZOS_SUCCESS )
      return ZOS_ERROR;

    buff->FXAS21002C_GYRO_AXIS_X = (valueH << 8) | valueL;

    if( FXAS21002C_Gyro_ReadReg(FXAS21002C_GYRO_YOUT_H, &valueH) != ZOS_SUCCESS )
      return ZOS_ERROR;

    if( FXAS21002C_Gyro_ReadReg(FXAS21002C_GYRO_YOUT_L, &valueL) != ZOS_SUCCESS )
      return ZOS_ERROR;

    buff->FXAS21002C_GYRO_AXIS_Y = (valueH << 8) | valueL;

    if( FXAS21002C_Gyro_ReadReg(FXAS21002C_GYRO_ZOUT_H, &valueH) != ZOS_SUCCESS )
      return ZOS_ERROR;

    if( FXAS21002C_Gyro_ReadReg(FXAS21002C_GYRO_ZOUT_L, &valueL) != ZOS_SUCCESS )
      return ZOS_ERROR;


    buff->FXAS21002C_GYRO_AXIS_Z = (valueH << 8) | valueL;

    return ZOS_SUCCESS;
}

/******************************************************
*          Internal Function Definitions
******************************************************/

/*******************************************************************************
* Function Name     : FXAS21002C_Gyro_ReadReg
* Description       : Generic I2C reading function
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t FXAS21002C_Gyro_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_FXAS21002C, Reg, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : FXAS21002C_Gyro_WriteReg
* Description       : Generic I2C writing function
* Input             : Register Address, Data to be written
* Output            : None
* Return            : None
*******************************************************************************/
static zos_result_t FXAS21002C_Gyro_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_FXAS21002C, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

