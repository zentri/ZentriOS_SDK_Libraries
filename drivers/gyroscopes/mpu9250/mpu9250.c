/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "mpu9250_gyro.h"
#include "zos.h"


static const zos_i2c_device_t ZOS_I2C_MPU9250 =
{
    .address = (uint16_t)MPU9250_SLAVE_ADDRESS,
    .speed = I2C_CLOCK_HIGH_SPEED,
    .retries = (uint16_t)1U,
    .flags = I2C_FLAG_HEXIFY,
    .read_timeout = 40
};


static zos_result_t MPU9250_Gyro_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t MPU9250_Gyro_WriteReg(uint8_t WriteAddr, uint8_t Data);



/*******************************************************************************
* Function Name  : MPU9250_SetAxis
* Description    : Enable/Disable MPU9250 Axis
* Input          : MPU9250_X_ENABLE/DISABLE | MPU9250_Y_ENABLE/DISABLE | MPU9250_Z_ENABLE/DISABLE
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_GyroSetAxis(uint8_t enableX, uint8_t enableY, uint8_t enableZ)
{
  uint8_t value;

  if( MPU9250_Gyro_ReadReg(MPU9250_PWR_MGMT_2, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xC7; // initial reset of DISABLE_XG, DISABLE_YG, DISABLE_ZG bits

  // enableX > 0 means X should be enabled: DISABLE_XG = 0
  if(enableX > 0)
  {
      value |= (0 << MPU9250_ENABLE_XG);
  }
  else
  {
      value |= (1 << MPU9250_ENABLE_XG);
  }


  // enableY > 0 means Y should be enabled: DISABLE_YG = 0
  if(enableY > 0)
  {
      value |= (0 << MPU9250_ENABLE_YG);
  }
  else
  {
      value |= (1 << MPU9250_ENABLE_YG);
  }


  // enableZ > 0 means Z should be enabled: DISABLE_ZG = 0
  if(enableZ > 0)
  {
      value |= (0 << MPU9250_ENABLE_ZG);
  }
  else
  {
      value |= (1 << MPU9250_ENABLE_ZG);
  }


  if( MPU9250_Gyro_WriteReg(MPU9250_PWR_MGMT_2, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_SetPowerMgmt
* Description    : Sets the MPU9250 Power Management
* Input          : RESET, SLEEP, and CYCLE choice
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_GyroSetPowerMgmt(MPU9250_Reset_t reset, MPU9250_Sleep_t sleep, MPU9250_Cycle_t cycle)
{
  uint8_t value;

  if( MPU9250_Gyro_ReadReg(MPU9250_PWR_MGMT_1, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0x1F;
  value |= (reset << MPU9250_RESET);
  value |= (sleep << MPU9250_SLEEP);
  value |= (cycle << MPU9250_CYCLE);

  if( MPU9250_Gyro_WriteReg(MPU9250_PWR_MGMT_1, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_SetFullScale
* Description    : Sets the MPU9250 FullScale
* Input          : GYRO_FULLSCALE_250/GYRO_FULLSCALE_500/GYRO_FULLSCALE_1000/GYRO_FULLSCALE_2000
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_GyroSetFullScale(MPU9250_GyroFullscale_t fs)
{
  uint8_t value;

  if( MPU9250_Gyro_ReadReg(MPU9250_GYRO_CFG, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xE7;
  value |= (fs << MPU9250_FSS);

  if( MPU9250_Gyro_WriteReg(MPU9250_GYRO_CFG, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_GetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_GyroGetStatusReg(uint8_t* val)
{
    if( MPU9250_Gyro_ReadReg(MPU9250_INTERRUPT_STATUS, val) != ZOS_SUCCESS )
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}


/*******************************************************************************
* Function Name  : MPU9250_GetFullScale
* Description    : Reads the MPU9250 FullScale Register
* Input          : GYRO_FULLSCALE_250/GYRO_FULLSCALE_500/GYRO_FULLSCALE_1000/GYRO_FULLSCALE_2000
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_GyroGetFullScale(MPU9250_GyroFullscale_t* fs)
{
  uint8_t val;
  if( MPU9250_Gyro_ReadReg(MPU9250_GYRO_CFG, &val) != ZOS_SUCCESS )
    return ZOS_ERROR;

  val &= 0x18;
  *fs = (val >> MPU9250_FSS);

  return ZOS_SUCCESS;
}
/*******************************************************************************
* Function Name  : MPU9250_GyroGetAxesRaw
* Description    : Read the Gyroscope Values Output Registers
* Input          : buffer to empty by AxesRaw_t Typedef
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_GyroGetAxesRaw(MPU9250_GyroAxesRaw_t* buff)
{
  int16_t value;
  uint8_t *valueL = (uint8_t *)(&value);
  uint8_t *valueH = ((uint8_t *)(&value)+1);

  if( MPU9250_Gyro_ReadReg(MPU9250_GYRO_XOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( MPU9250_Gyro_ReadReg(MPU9250_GYRO_XOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  buff->MPU9250_GYRO_AXIS_X = value;

  if( MPU9250_Gyro_ReadReg(MPU9250_GYRO_YOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( MPU9250_Gyro_ReadReg(MPU9250_GYRO_YOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  buff->MPU9250_GYRO_AXIS_Y = value;

  if( MPU9250_Gyro_ReadReg(MPU9250_GYRO_ZOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( MPU9250_Gyro_ReadReg(MPU9250_GYRO_ZOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  buff->MPU9250_GYRO_AXIS_Z = value;

  return ZOS_SUCCESS;
}

/******************************************************
*          Internal Function Definitions
******************************************************/

/*******************************************************************************
* Function Name     : MPU9250_Gyro_ReadReg
* Description       : Generic I2C reading function
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t MPU9250_Gyro_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_MPU9250, Reg, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : MPU9250_Gyro_WriteReg
* Description       : Generic I2C writing function
* Input             : Register Address, Data to be written
* Output            : None
* Return            : None
*******************************************************************************/
static zos_result_t MPU9250_Gyro_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_MPU9250, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

