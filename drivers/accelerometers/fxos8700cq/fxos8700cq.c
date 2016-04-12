/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "fxos8700cq.h"
#include "zos.h"

/******************************************************
 *                    Constants
 ******************************************************/
static const zos_i2c_device_t ZOS_I2C_FXOS8700CQ =
{
    .port    = PLATFORM_STD_I2C,
    .address = FXOS8700CQ_SLAVE_ADDRESS,
    .speed   = I2C_CLOCK_STANDARD_SPEED,
    .retries = (uint16_t)3U,
    .flags   = I2C_FLAG_HEXIFY
};

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
static zos_result_t FXOS8700CQ_Accel_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t FXOS8700CQ_Accel_WriteReg(uint8_t WriteAddr, uint8_t Data);


/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *          External Function Definitions
 ******************************************************/

/*******************************************************************************
* Function Name  : FXOS8700CQ_AccSetODR
* Description    : Sets FXOS8700CQ Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_AccSetODR(FXOS8700CQ_ODR_t odr_value)
{
  uint8_t value;

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_CTRL_REG1, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xC7;
  value |= (odr_value << FXOS8700CQ_ODR);

  if( FXOS8700CQ_Accel_WriteReg(FXOS8700CQ_CTRL_REG1, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXOS8700CQ_AccSetOperatingMode
* Description    : Sets the FXOS8700CQ Operating Mode
* Input          : STANDBY/ACTIVE choice
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_AccSetOperatingMode(FXOS8700CQ_OperatingMode_t mode)
{
  uint8_t value;

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_CTRL_REG1, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xFE;
  value |= (mode << FXOS8700CQ_OPERATING_MODE);

  if( FXOS8700CQ_Accel_WriteReg(FXOS8700CQ_CTRL_REG1, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXOS8700CQ_AccSetHybridMode
* Description    : Sets the FXOS8700CQ Hybrid Mode (accelerometer + magnetometer)
* Input          : ACTIVE/ACTIVE choice
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_AccSetHybridMode(FXOS8700CQ_HybridMode_t mode)
{
  uint8_t value;

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_M_CTRL_REG1, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xFC;
  value |= (mode << FXOS8700CQ_HYBRID);

  if( FXOS8700CQ_Accel_WriteReg(FXOS8700CQ_M_CTRL_REG1, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}
/*******************************************************************************
* Function Name  : FXOS8700CQ_AccSetLowNoiseMode
* Description    : Sets the FXOS8700CQ Low Noise Mode
* Input          : ACTIVE/INACTIVE choice
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_AccSetLowNoiseMode(FXOS8700CQ_NoiseMode_t lnoise)
{
  uint8_t value;

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_CTRL_REG1, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xFB;
  value |= (lnoise << FXOS8700CQ_LOW_NOISE);

  if( FXOS8700CQ_Accel_WriteReg(FXOS8700CQ_CTRL_REG1, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}
/*******************************************************************************
* Function Name  : FXOS8700CQ_AccSetFullScale
* Description    : Sets the FXOS8700CQ FullScale
* Input          : ACC_FULLSCALE_2/ACC_FULLSCALE_4/ACC_FULLSCALE_8
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_AccSetFullScale(FXOS8700CQ_AccFullscale_t fs)
{
  uint8_t value;

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_XYZ_DATA_CFG, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xFC;
  value |= (fs << FXOS8700CQ_FSS);

  if( FXOS8700CQ_Accel_WriteReg(FXOS8700CQ_XYZ_DATA_CFG, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXOS8700CQ_GetStatusReg
* Description    : Read the status register
* Input          : Char to fill by Status Reg Value
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_AccGetStatusReg(uint8_t* val)
{
    if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_STATUS, val) != ZOS_SUCCESS )
    {
        return ZOS_ERROR;
    }

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXOS8700CQ_AccGetAxesRaw
* Description    : Read the Acceleration Values Output Registers
* Input          : buffer to empty by AxesRaw_t Typedef
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_AccGetAxesRaw(FXOS8700CQ_AccAxesRaw_t* buff)
{
  int16_t value;
  uint8_t *valueL = (uint8_t *)(&value);
  uint8_t *valueH = ((uint8_t *)(&value)+1);

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_ACCEL_XOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_ACCEL_XOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  // copy the 14 bit accelerometer byte data into 16 bit words
  buff->FXOS8700CQ_ACC_AXIS_X = value >> 2;

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_ACCEL_YOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_ACCEL_YOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  // copy the 14 bit accelerometer byte data into 16 bit words
  buff->FXOS8700CQ_ACC_AXIS_Y = value >> 2;

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_ACCEL_ZOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( FXOS8700CQ_Accel_ReadReg(FXOS8700CQ_ACCEL_ZOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  // copy the 14 bit accelerometer byte data into 16 bit words
  buff->FXOS8700CQ_ACC_AXIS_Z = value >> 2;

  return ZOS_SUCCESS;
}

/******************************************************
*          Internal Function Definitions
******************************************************/

/*******************************************************************************
* Function Name     : FXOS8700CQ_Accel_ReadReg
* Description       : Generic I2C reading function
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t FXOS8700CQ_Accel_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_FXOS8700CQ, Reg, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : FXOS8700CQ_Accel_WriteReg
* Description       : Generic I2C writing function
* Input             : Register Address, Data to be written
* Output            : None
* Return            : None
*******************************************************************************/
static zos_result_t FXOS8700CQ_Accel_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_FXOS8700CQ, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

