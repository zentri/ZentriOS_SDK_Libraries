/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "fxos8700cq.h"

static zos_result_t FXOS8700CQ_Magn_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t FXOS8700CQ_Magn_WriteReg(uint8_t WriteAddr, uint8_t Data);

static const zos_i2c_device_t ZOS_I2C_FXOS8700CQ =
{
    .port    = PLATFORM_STD_I2C,
    .address = FXOS8700CQ_SLAVE_ADDRESS,
    .speed   = I2C_CLOCK_STANDARD_SPEED,
    .retries = (uint16_t)3U,
    .flags   = I2C_FLAG_HEXIFY
};

/*******************************************************************************
* Function Name  : FXOS8700CQ_MagnSetHybridMode
* Description    : Sets the FXOS8700CQ Hybrid Mode (accelerometer + magnetometer)
* Input          : ACTIVE/ACTIVE choice
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_MagnSetHybridMode(FXOS8700CQ_HybridMode_t mode)
{
  uint8_t value;

  if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_M_CTRL_REG1, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xFC;
  value |= (mode << FXOS8700CQ_HYBRID);

  if( FXOS8700CQ_Magn_WriteReg(FXOS8700CQ_M_CTRL_REG1, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXOS8700CQ_MagnSetOperatingMode
* Description    : Sets the FXOS8700CQ Operating Mode
* Input          : STANDBY/ACTIVE choice
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_MagnSetOperatingMode(FXOS8700CQ_OperatingMode_t mode)
{
  uint8_t value;

  if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_CTRL_REG1, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xFE;
  value |= (mode << FXOS8700CQ_OPERATING_MODE);

  if( FXOS8700CQ_Magn_WriteReg(FXOS8700CQ_CTRL_REG1, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXOS8700CQ_MagnSetAutoIncrement
* Description    : Sets the FXOS8700CQ Auto Increment Mode
* Input          : ENABLE/DISABLE Auto Increment
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_MagnSetAutoIncrement(zos_bool_t auto_inc)
{
  uint8_t value;

  if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_M_CTRL_REG2, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xDF;
  value |= (auto_inc << FXOS8700CQ_AUTO_INC);

  if( FXOS8700CQ_Magn_WriteReg(FXOS8700CQ_M_CTRL_REG2, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXOS8700CQ_MagnSetControlParams
* Description    : Sets the FXOS8700CQ control parameters
* Input          : Auto Calibration / One Shot Reset / One Shot Measurement
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_MagnSetControlParams(zos_bool_t auto_calib, zos_bool_t os_rest, zos_bool_t os_meas)
{
  uint8_t value;

  if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_M_CTRL_REG1, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0x1F;
  value |= (auto_calib << FXOS8700CQ_AUTO_CALIB);
  value |= (os_rest << FXOS8700CQ_ONE_SHOT_RESET);
  value |= (os_meas << FXOS8700CQ_ONE_SHOT_MEASUREMENT);

  if( FXOS8700CQ_Magn_WriteReg(FXOS8700CQ_M_CTRL_REG1, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXOS8700CQ_MagnSetOversampleRatio
* Description    : Sets the FXOS8700CQ magnetometer over sampling ratio
* Input          : OSR
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_MagnSetOversampleRatio(FXOS8700CQ_MagnOSR_t osr)
{
  uint8_t value;

  if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_M_CTRL_REG1, &value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  value &= 0xE3;
  value |= (osr << FXOS8700CQ_OSR);

  if( FXOS8700CQ_Magn_WriteReg(FXOS8700CQ_M_CTRL_REG1, value) != ZOS_SUCCESS )
  {
      return ZOS_ERROR;
  }

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXOS8700CQ_MagnGetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_MagnGetStatusReg(uint8_t* val)
{
    if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_STATUS, val) != ZOS_SUCCESS )
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : FXOS8700CQ_MagnGetAxesRaw
* Description    : Read the Magnetometer Values Output Registers
* Input          : buffer to empty by AxesRaw_t Typedef
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t FXOS8700CQ_MagnGetAxesRaw(FXOS8700CQ_MagnAxesRaw_t* buff)
{
    uint8_t valueL, valueH;

    if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_MAGN_XOUT_H, &valueH) != ZOS_SUCCESS )
      return ZOS_ERROR;

    if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_MAGN_XOUT_L, &valueL) != ZOS_SUCCESS )
      return ZOS_ERROR;

    buff->FXOS8700CQ_MAGN_AXIS_X = (valueH << 8) | valueL;

    if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_MAGN_YOUT_H, &valueH) != ZOS_SUCCESS )
      return ZOS_ERROR;

    if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_MAGN_YOUT_L, &valueL) != ZOS_SUCCESS )
      return ZOS_ERROR;

    buff->FXOS8700CQ_MAGN_AXIS_Y = (valueH << 8) | valueL;

    if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_MAGN_ZOUT_H, &valueH) != ZOS_SUCCESS )
      return ZOS_ERROR;

    if( FXOS8700CQ_Magn_ReadReg(FXOS8700CQ_MAGN_ZOUT_L, &valueL) != ZOS_SUCCESS )
      return ZOS_ERROR;


    buff->FXOS8700CQ_MAGN_AXIS_Z = (valueH << 8) | valueL;

    return ZOS_SUCCESS;
}

/******************************************************
*          Internal Function Definitions
******************************************************/

/*******************************************************************************
* Function Name     : FXOS8700CQ_Magn_ReadReg
* Description       : Generic I2C reading function
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t FXOS8700CQ_Magn_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_FXOS8700CQ, Reg, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : FXOS8700CQ_Magn_WriteReg
* Description       : Generic I2C writing function
* Input             : Register Address, Data to be written
* Output            : None
* Return            : None
*******************************************************************************/
static zos_result_t FXOS8700CQ_Magn_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_FXOS8700CQ, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

