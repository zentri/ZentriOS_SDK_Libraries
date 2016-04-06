/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "mpu9250_accel.h"
#include "zos.h"

/******************************************************
 *                    Constants
 ******************************************************/
static const zos_i2c_device_t ZOS_I2C_MPU9250 =
{
    .address = (uint16_t)MPU9250_SLAVE_ADDRESS,
    .speed = I2C_CLOCK_HIGH_SPEED,
    .retries = (uint16_t)3U,
    .flags = I2C_FLAG_HEXIFY,
    .read_timeout = 40
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
static zos_result_t MPU9250_Accel_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t MPU9250_Accel_WriteReg(uint8_t WriteAddr, uint8_t Data);


/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *          External Function Definitions
 ******************************************************/

/*******************************************************************************
* Function Name  : MPU9250_SetODR
* Description    : Sets MPU9250 Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccSetODR(MPU9250_ODR_t odr_value)
{
  uint8_t value;

  if( MPU9250_Accel_ReadReg(MPU9250_LP_ACCEL_ODR, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xF0;
  value |= odr_value;

  if( MPU9250_Accel_WriteReg(MPU9250_LP_ACCEL_ODR, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_AccSetLowPassFilter
* Description    : Sets MPU9250 LPF configurations
* Input          : FCHOICE, DLPFCFG
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccSetLowPassFilter(uint8_t fchoice_val, uint8_t dlpf_cfg_val)
{
  uint8_t value;

  if( MPU9250_Accel_ReadReg(MPU9250_ACCEL_CFG_2, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xF0;
  value |= (fchoice_val << MPU9250_ACCEL_FCHOICE);
  value |= (dlpf_cfg_val << MPU9250_ACCEL_DLPF_CFG);

  if( MPU9250_Accel_WriteReg(MPU9250_ACCEL_CFG_2, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_AccSetInterrupts
* Description    : Sets MPU9250 Interrupt Enabled
* Input          : Whole mask of interrupts to be enabled (register value)
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccSetInterrupts(uint8_t int_en)
{
  if( MPU9250_Accel_WriteReg(MPU9250_INT_ENABLE, int_en) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}


/*******************************************************************************
* Function Name  : MPU9250_AccSetIntelligenceControl
* Description    : Sets MPU9250 Hardware Intelligence Control
* Input          : Intelligence Enabled/Mode
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccSetIntelligenceControl(uint8_t intelligence_en, uint8_t intelligence_mode)
{
  uint8_t value;

  if( MPU9250_Accel_ReadReg(MPU9250_ACCEL_INTEL_CTRL, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0x3F;
  value |= (intelligence_en << MPU9250_ACCEL_INTEL_EN);
  value |= (intelligence_mode << MPU9250_ACCEL_INTEL_MODE);

  if( MPU9250_Accel_WriteReg(MPU9250_ACCEL_INTEL_CTRL, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_AccSetWomThr
* Description    : Sets MPU9250 Wake on Motion Threshold
* Input          : WOM_THR
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccSetWomThr(uint8_t thr)
{
  if( MPU9250_Accel_WriteReg(MPU9250_WOM_THR, thr) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_SetAccAxis
* Description    : Enable/Disable MPU9250 Accelerometer Axis
* Input          : MPU9250_X_ENABLE/DISABLE | MPU9250_Y_ENABLE/DISABLE | MPU9250_Z_ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccSetAccAxis(uint8_t enableX, uint8_t enableY, uint8_t enableZ)
{
  uint8_t value;

  if( MPU9250_Accel_ReadReg(MPU9250_PWR_MGMT_2, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xC7; // initial reset of DISABLE_XA, DISABLE_YA, DISABLE_ZA bits

  // enableX > 0 means X should be enabled: DISABLE_XA = 0
  if(enableX > 0)
  {
      value |= (0 << MPU9250_ENABLE_XA);
  }
  else
  {
      value |= (1 << MPU9250_ENABLE_XA);
  }


  // enableY > 0 means Y should be enabled: DISABLE_YA = 0
  if(enableY > 0)
  {
      value |= (0 << MPU9250_ENABLE_YA);
  }
  else
  {
      value |= (1 << MPU9250_ENABLE_YA);
  }


  // enableZ > 0 means Z should be enabled: DISABLE_ZA = 0
  if(enableZ > 0)
  {
      value |= (0 << MPU9250_ENABLE_ZA);
  }
  else
  {
      value |= (1 << MPU9250_ENABLE_ZA);
  }


  if( MPU9250_Accel_WriteReg(MPU9250_PWR_MGMT_2, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_SetGyroAxis
* Description    : Enable/Disable MPU9250 Gyroscope Axis
* Input          : MPU9250_X_ENABLE/DISABLE | MPU9250_Y_ENABLE/DISABLE | MPU9250_Z_ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccSetGyroAxis(uint8_t enableX, uint8_t enableY, uint8_t enableZ)
{
  uint8_t value;

  if( MPU9250_Accel_ReadReg(MPU9250_PWR_MGMT_2, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xF8; // initial reset of DISABLE_XG, DISABLE_YG, DISABLE_ZA bits

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


  if( MPU9250_Accel_WriteReg(MPU9250_PWR_MGMT_2, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_SetPowerMgmt
* Description    : Sets the MPU9250 Power Management
* Input          : SLEEP, CYCLE, and STANDBY choice
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccSetPowerMgmt(MPU9250_Sleep_t sleep, MPU9250_Cycle_t cycle, MPU9250_Standby_t standby)
{
  uint8_t value;

  if( MPU9250_Accel_ReadReg(MPU9250_PWR_MGMT_1, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0x1F;
  value |= (sleep << MPU9250_SLEEP);
  value |= (cycle << MPU9250_CYCLE);
  value |= (standby << MPU9250_STANDBY);

  if( MPU9250_Accel_WriteReg(MPU9250_PWR_MGMT_1, value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : MPU9250_SetFullScale
* Description    : Sets the MPU9250 FullScale
* Input          : ACC_FULLSCALE_2/ACC_FULLSCALE_4/ACC_FULLSCALE_8/ACC_FULLSCALE_16
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccSetFullScale(MPU9250_AccFullscale_t fs)
{
  uint8_t value;

  if( MPU9250_Accel_ReadReg(MPU9250_ACCEL_CFG, &value) != ZOS_SUCCESS )
    return ZOS_ERROR;

  value &= 0xE7;
  value |= (fs << MPU9250_FSS);

  if( MPU9250_Accel_WriteReg(MPU9250_ACCEL_CFG, value) != ZOS_SUCCESS )
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
zos_result_t MPU9250_AccGetStatusReg(uint8_t* val)
{
    if( MPU9250_Accel_ReadReg(MPU9250_INTERRUPT_STATUS, val) != ZOS_SUCCESS )
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}


/*******************************************************************************
* Function Name  : MPU9250_GetFullScale
* Description    : Reads the MPU9250 FullScale Register
* Input          : ACC_FULLSCALE_2/ACC_FULLSCALE_4/ACC_FULLSCALE_8/ACC_FULLSCALE_16
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccGetFullScale(MPU9250_AccFullscale_t* fs)
{
  uint8_t val;
  if( MPU9250_Accel_ReadReg(MPU9250_ACCEL_CFG, &val) != ZOS_SUCCESS )
    return ZOS_ERROR;

  val &= 0x18;
  *fs = (val >> MPU9250_FSS);

  return ZOS_SUCCESS;
}
/*******************************************************************************
* Function Name  : MPU9250_AccGetAxesRaw
* Description    : Read the Acceleration Values Output Registers
* Input          : buffer to empty by AxesRaw_t Typedef
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t MPU9250_AccGetAxesRaw(MPU9250_AccAxesRaw_t* buff)
{
  int16_t value;
  uint8_t *valueL = (uint8_t *)(&value);
  uint8_t *valueH = ((uint8_t *)(&value)+1);

  if( MPU9250_Accel_ReadReg(MPU9250_ACCEL_XOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( MPU9250_Accel_ReadReg(MPU9250_ACCEL_XOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  buff->MPU9250_ACC_AXIS_X = value;

  if( MPU9250_Accel_ReadReg(MPU9250_ACCEL_YOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( MPU9250_Accel_ReadReg(MPU9250_ACCEL_YOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  buff->MPU9250_ACC_AXIS_Y = value;

  if( MPU9250_Accel_ReadReg(MPU9250_ACCEL_ZOUT_L, valueL) != ZOS_SUCCESS )
    return ZOS_ERROR;

  if( MPU9250_Accel_ReadReg(MPU9250_ACCEL_ZOUT_H, valueH) != ZOS_SUCCESS )
    return ZOS_ERROR;

  buff->MPU9250_ACC_AXIS_Z = value;

  return ZOS_SUCCESS;
}

/******************************************************
*          Internal Function Definitions
******************************************************/

/*******************************************************************************
* Function Name     : MPU9250_Accel_ReadReg
* Description       : Generic I2C reading function
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t MPU9250_Accel_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_MPU9250, Reg, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : MPU9250_Accel_WriteReg
* Description       : Generic I2C writing function
* Input             : Register Address, Data to be written
* Output            : None
* Return            : None
*******************************************************************************/
static zos_result_t MPU9250_Accel_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_MPU9250, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

