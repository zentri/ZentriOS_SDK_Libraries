/* Includes ------------------------------------------------------------------*/
#include "lsm6ds0.h"
#include "zos.h"
static zos_result_t LSM6DS0_ReadReg(uint8_t Reg, uint8_t* Data);
static zos_result_t LSM6DS0_WriteReg(uint8_t WriteAddr, uint8_t Data);

static const zos_i2c_device_t const ZOS_I2C_LSM6DS0 =
{
    .address    = LSM6DS0_I2C_ADDRESS,
    .speed      = I2C_CLOCK_STANDARD_SPEED,
    .retries    = (uint16_t)3U,
    .flags      = I2C_FLAG_HEXIFY
};

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name     : LSM6DS0_ReadReg
* Description       : Generic Reading function. It must be fullfilled with either
*                   : I2C or SPI reading functions                  
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t LSM6DS0_ReadReg(uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(&ZOS_I2C_LSM6DS0, Reg, Data) != ZOS_SUCCESS)
    {
        return ZOS_ERROR;
    }

    return ZOS_SUCCESS;
}


/*******************************************************************************
* Function Name     : LSM6DS0_WriteReg
* Description       : Generic Writing function. It must be fullfilled with either
*                   : I2C or SPI writing function
* Input         : Register Address, Data to be written
* Output        : None
* Return        : None
*******************************************************************************/
static zos_result_t LSM6DS0_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(&ZOS_I2C_LSM6DS0, WriteAddr, Data) != ZOS_SUCCESS)
    {
        return ZOS_ERROR;
    }
    return ZOS_SUCCESS;
}

/* Public functions -------------------------------------------------------_--*/

/*******************************************************************************
* Function Name  : Lsm6ds0_Init
* Description    : Initialize accelorator lsm6ds0 in a ready state
* Input          : None
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t LSM6DS0_Init(void)
{
    uint8_t val = 0x00;
    zos_result_t ret;

    ret = LSM6DS0_ReadReg(LSM6DS0_XG_WHO_AM_I_ADDR, &val);
    if( ret == ZOS_SUCCESS)
    {
        if(val != I_AM_LSM6DS0_XG)
        {
            ret = ZOS_ERROR;
        }
    }

    return ret;
}

/*******************************************************************************
* Function Name  : LSM6DS0_GetAccAxesRaw
* Description    : Read data from LSM6DS0 Accelerometer and calculate
                   acceleration in mgs
* Input          : Buffer to empty by AxesRaw_t Typedef
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t LSM6DS0_GetAccAxesRaw(AxesRaw_t *pData)
{
    int16_t value;
    uint8_t *valueL = (uint8_t *)(&value);
    uint8_t *valueH = ((uint8_t *)(&value)+1);

    if( LSM6DS0_ReadReg(LSM6DS0_XG_OUT_X_L_XL, valueL) != ZOS_SUCCESS )
      return ZOS_ERROR;

    if( LSM6DS0_ReadReg(LSM6DS0_XG_OUT_X_H_XL, valueH) != ZOS_SUCCESS )
      return ZOS_ERROR;

    pData->AXIS_X = value;

    if( LSM6DS0_ReadReg(LSM6DS0_XG_OUT_Y_L_XL, valueL) != ZOS_SUCCESS )
      return ZOS_ERROR;

    if( LSM6DS0_ReadReg(LSM6DS0_XG_OUT_Y_H_XL, valueH) != ZOS_SUCCESS )
      return ZOS_ERROR;

    pData->AXIS_Y = value;

    if( LSM6DS0_ReadReg(LSM6DS0_XG_OUT_Z_L_XL, valueL) != ZOS_SUCCESS )
      return ZOS_ERROR;

    if( LSM6DS0_ReadReg(LSM6DS0_XG_OUT_Z_H_XL, valueH) != ZOS_SUCCESS )
      return ZOS_ERROR;

    pData->AXIS_Z = value;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LSM6DS0_SetODR
* Description    : Sets LSM6DS0 Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t LSM6DS0_SetODR(LSM6DS0_ODR_t ov)
{
    uint8_t value;

    if( LSM6DS0_ReadReg(LSM6DS0_XG_CTRL_REG6_XL, &value) != ZOS_SUCCESS)
        return ZOS_ERROR;

    value &= 0x1F;
    value |= ov;
    if( LSM6DS0_WriteReg(LSM6DS0_XG_CTRL_REG6_XL, value) != ZOS_SUCCESS )
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LSM6DS0_SetAxis
* Description    : Enable/Disable LSM6DS0 Axis
* Input          : LSM6DS0_X_ENABLE/DISABLE | LSM6DS0_Y_ENABLE/DISABLE | LSM6DS0_Z_ENABLE/DISABLE
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t LSM6DS0_SetAxis(LSM6DS0_AXISenable_t axis)
{
    uint8_t value;

    if( LSM6DS0_ReadReg(LSM6DS0_XG_CTRL_REG5_XL, &value) != ZOS_SUCCESS )
        return ZOS_ERROR;

    value &= 0xC7;
    value |= axis;

    if( LSM6DS0_WriteReg(LSM6DS0_XG_CTRL_REG5_XL, value) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LSM6DS0_SetFullScale
* Description    : Sets the LSM6DS0 FullScale
* Input          : LSM6DS0_FULLSCALE_2/LSM6DS0_FULLSCALE_4/LSM6DS0_FULLSCALE_8/LSM6DS0_FULLSCALE_16
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t LSM6DS0_SetFullScale(LSM6DS0_Fullscale_t fs)
{
    uint8_t value;

    if( LSM6DS0_ReadReg(LSM6DS0_XG_CTRL_REG6_XL, &value) != ZOS_SUCCESS)
        return ZOS_ERROR;

    value &= 0xE7;
    value |= (fs<<3);

    if( LSM6DS0_WriteReg(LSM6DS0_XG_CTRL_REG6_XL, value) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LSM6DS0_SetBLE
* Description    : Set Endianess (MSB/LSB)
* Input          : BLE_LSB / BLE_MSB
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t LSM6DS0_SetBLE(LSM6DS0_Endianess_t ble)
{
    uint8_t value;

    if( LSM6DS0_ReadReg(LSM6DS0_XG_CTRL_REG8_XL, &value) != ZOS_SUCCESS)
        return ZOS_ERROR;

    value &= 0xFD;
    value |= (ble<<1);

    if( LSM6DS0_WriteReg(LSM6DS0_XG_CTRL_REG8_XL, value) != ZOS_SUCCESS )
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LSM6DS0_ResetDevice
* Description    : Spftware reset the device
* Input          : None
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t LSM6DS0_ResetDevice(void)
{
    uint8_t value;

    if( LSM6DS0_ReadReg(LSM6DS0_XG_CTRL_REG8_XL, &value) != ZOS_SUCCESS)
        return ZOS_ERROR;

    value |= 0x01;

    if( LSM6DS0_WriteReg(LSM6DS0_XG_CTRL_REG8_XL, value) != ZOS_SUCCESS )
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LSM6DS0_GetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
zos_result_t LSM6DS0_GetStatusReg(uint8_t* val)
{
    if( LSM6DS0_ReadReg(LSM6DS0_STATUS_REG, val) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}
