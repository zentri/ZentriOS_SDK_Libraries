/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "mpu9250.h"




static zos_result_t MPU9250_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t MPU9250_WriteReg(uint8_t WriteAddr, uint8_t Data);
static zos_result_t AK8963_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t AK8963_WriteReg(uint8_t WriteAddr, uint8_t Data);




static const zos_i2c_device_t ZOS_I2C_MPU9250 =
{
    .address = (uint16_t)MPU9250_SLAVE_ADDRESS,
    .speed = I2C_CLOCK_HIGH_SPEED,
    .retries = (uint16_t)1U,
    .flags = I2C_FLAG_HEXIFY,
    .read_timeout = 40
};

static const zos_i2c_device_t ZOS_I2C_AK8963 =
{
    .address = (uint16_t)AK8963_SLAVE_ADDRESS,
    .speed = I2C_CLOCK_HIGH_SPEED,
    .retries = (uint16_t)1U,
    .flags = I2C_FLAG_HEXIFY,
    .read_timeout = 40
};


/*******************************************************************************
* Function Name  : MPU9250_MagnSetI2CBypass
* Description    : Sets the MPU9250 I2C Bypass Enable
* Input          : MPU9250_BYPASS_DISABLED/MPU9250_BYPASS_ENABLED
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_MagnSetI2CBypass(MPU9250_I2C_Bypass_t bypass)
{
  uint8_t value;

  if( MPU9250_ReadReg(MPU9250_I2C_MASTER_BYPASS, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xFD;
  value |= (bypass << MPU9250_BYPASS_EN);

  if( MPU9250_WriteReg(MPU9250_I2C_MASTER_BYPASS, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_AK8963_MagnSetOprMode
* Description    : Sets the AK8963 Operation Mode
* Input          : SINGLE/CONTINOUS_MEASUREMENT
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AK8963_MagnSetOprMode(MPU9250_AK8963_MagnOperationMode_t mode)
{
  uint8_t value;

  if( AK8963_ReadReg(MPU9250_AK8963_CTRL, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xF0;
  value |= mode;

  if( AK8963_WriteReg(MPU9250_AK8963_CTRL, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_AK8963_MagnReadSensAdj
* Description    : Reads the AK8963 sensor sensitivity adjustment values from ROM
* Input          : None
* Output         : X/Y/Z adjustment values
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AK8963_MagnReadSensAdj(MPU9250_AK8963_MagnSensAdjust_t* adjust_data)
{
    uint8_t value;

    // Registers 10H to 12H can be read only in Fuse access mode
    MPU9250_AK8963_MagnSetOprMode(MPU9250_AK8963_FUSE_ROM);

    // Read adjustment data registers

    if( AK8963_ReadReg(MPU9250_AK8963_MAGN_ASAX, &value) != ZOS_SUCCESS )
       return ZOS_ERROR;

     adjust_data->MPU9250_AK8963_MAGN_ADJUST_X = value;

     if( AK8963_ReadReg(MPU9250_AK8963_MAGN_ASAY, &value) != ZOS_SUCCESS )
       return ZOS_ERROR;

     adjust_data->MPU9250_AK8963_MAGN_ADJUST_Y = value;

     if( AK8963_ReadReg(MPU9250_AK8963_MAGN_ASAZ, &value) != ZOS_SUCCESS )
       return ZOS_ERROR;

     adjust_data->MPU9250_AK8963_MAGN_ADJUST_Z = value;


     // After reading; power down mode:
     MPU9250_AK8963_MagnSetOprMode(MPU9250_AK8963_PWR_DOWN);

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_AK8963_MagnGetOutBitSize
* Description    : Reads the MPU9250 Output Bit Size Config
* Input          : None
* Output         : MPU9250_AK8963_MAGN_OUTPUT_14/MPU9250_AK8963_MAGN_OUTPUT_16
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AK8963_MagnGetOutBitSize(MPU9250_AK8963_MagnOutBitSize_t* bit_size)
{
  uint8_t val;
  if( AK8963_ReadReg(MPU9250_AK8963_CTRL, &val) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if ((val & MPU9250_AK8963_OUTPUT_SIZE) > 0)
  {
      *bit_size = MPU9250_AK8963_MAGN_OUTPUT_16;
  }
  else
  {
      *bit_size = MPU9250_AK8963_MAGN_OUTPUT_14;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_AK8963_MagnSetOutBitSize
* Description    : Sets the MPU9250 FullScale
* Input          : MPU9250_AK8963_MAGN_OUTPUT_14/MPU9250_AK8963_MAGN_OUTPUT_16
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AK8963_MagnSetOutBitSize(MPU9250_AK8963_MagnOutBitSize_t bit_size)
{
  uint8_t value;

  if( AK8963_ReadReg(MPU9250_AK8963_CTRL, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xEF;
  value |= (bit_size << MPU9250_AK8963_OUTPUT);

  if( AK8963_WriteReg(MPU9250_AK8963_CTRL, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_AK8963_MagnGetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AK8963_MagnGetStatusReg(uint8_t* val)
{
    if( AK8963_ReadReg(MPU9250_AK8963_STATUS, val) != ZOS_SUCCESS )
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_AK8963_MagnGetAxesRaw
* Description    : Read the Magnetometer Values Output Registers
* Input          : buffer to empty by AxesRaw_t Typedef
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AK8963_MagnGetAxesRaw(MPU9250_AK8963_MagnAxesRaw_t* buff)
{
/*
  int16_t value;
  uint8_t *valueL = (uint8_t *)(&value);
  uint8_t *valueH = ((uint8_t *)(&value)+1);

  if( AK8963_ReadReg(MPU9250_AK8963_MAGN_XOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( AK8963_ReadReg(MPU9250_AK8963_MAGN_XOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  buff->MPU9250_AK8963_MAGN_AXIS_X = value;

  if( AK8963_ReadReg(MPU9250_AK8963_MAGN_YOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( AK8963_ReadReg(MPU9250_AK8963_MAGN_YOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  buff->MPU9250_AK8963_MAGN_AXIS_Y = value;

  if( AK8963_ReadReg(MPU9250_AK8963_MAGN_ZOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( AK8963_ReadReg(MPU9250_AK8963_MAGN_ZOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  buff->MPU9250_AK8963_MAGN_AXIS_Z = value;
*/

  uint8_t data[6];

  if (zn_i2c_master_read_reg(&ZOS_I2C_AK8963, MPU9250_AK8963_MAGN_XOUT_L, data, 6) != ZOS_SUCCESS)
      return ZOS_ERROR;


  buff->MPU9250_AK8963_MAGN_AXIS_X = (data[1] << 8) | data[0];

  buff->MPU9250_AK8963_MAGN_AXIS_Y = (data[3] << 8) | data[2];;

  buff->MPU9250_AK8963_MAGN_AXIS_Z = (data[5] << 8) | data[4];

  return ZOS_SUCCESS;
}

/******************************************************
*          Internal Function Definitions
******************************************************/

/*******************************************************************************
* Function Name     : MPU9250_ReadReg
* Description       : Generic I2C reading function
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t MPU9250_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_MPU9250, Reg, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : MPU9250_WriteReg
* Description       : Generic I2C writing function
* Input             : Register Address, Data to be written
* Output            : None
* Return            : None
*******************************************************************************/
static zos_result_t MPU9250_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_MPU9250, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}



/*******************************************************************************
* Function Name     : AK8963_ReadReg
* Description       : Generic I2C (AK8963 die) reading function
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t AK8963_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_AK8963, Reg, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : AK8963_WriteReg
* Description       : Generic I2C (AK8963 die) writing function
* Input             : Register Address, Data to be written
* Output            : None
* Return            : None
*******************************************************************************/
static zos_result_t AK8963_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_AK8963, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

