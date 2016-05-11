/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "adp5589.h"

#include <stdlib.h>
#include "zos.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define ADP5589_REG_SIZE (uint16_t)3U


/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
static zos_bool_t is_device_valid(const zos_i2c_device_t* i2c);
static zos_bool_t args_valid(const zos_i2c_device_t* i2c, uint8_t* result);

/******************************************************
 *               Variables Definitions
 ******************************************************/
static zos_bool_t is_inited = ZOS_FALSE;
/******************************************************
 *          Internal Function Definitions
 ******************************************************/
/*******************************************************************************
* Function Name     : ADP5589_ReadReg
* Description       : Generic I2C reading function
* Input             : Register Address
* Output            : Data REad
* Return            : None
*******************************************************************************/
static zos_result_t ADP5589_ReadReg(const zos_i2c_device_t* i2c, uint8_t Reg, uint8_t* Data)
{
    if (zn_i2c_master_read_reg8(i2c, Reg, Data) != ZOS_SUCCESS)
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
static zos_result_t ADP5589_WriteReg(const zos_i2c_device_t* i2c, uint8_t WriteAddr, uint8_t Data)
{
    if (zn_i2c_master_write_reg8(i2c, WriteAddr, Data) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : ADP5589_ReadRegWithSize
* Description       : Generic I2C reading function
* Input             : I2C slave device, Register Address, Buffer to hold data, Size to read
* Output            : Data Read
* Return         	: Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
static zos_result_t ADP5589_ReadRegWithSize(const zos_i2c_device_t* i2c, uint8_t Reg, uint8_t* Data, uint16_t size)
{
    if (zn_i2c_master_read_reg(i2c, Reg, Data, size) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

/*******************************************************************************
* Function Name     : PMODCDC1_WriteRegWithSize
* Description       : Generic I2C writing function
* Input             : I2C slave device, Register Address, Data to be written, Size of data
* Output            : None
* Return         	: Status [ZOS_ERROR, ZOS_SUCCESS]
*******************************************************************************/
static zos_result_t ADP5589_WriteRegWithSize(const zos_i2c_device_t* i2c, uint8_t WriteAddr, const uint8_t *Data, uint16_t size)
{
    if (zn_i2c_master_write_reg(i2c, WriteAddr, Data, size) != ZOS_SUCCESS)
        return ZOS_ERROR;

    return ZOS_SUCCESS;
}

static zos_bool_t args_valid(const zos_i2c_device_t* i2c, uint8_t* result)
{
    return (zos_bool_t)(is_device_valid(i2c) && (result != NULL));
}

static zos_bool_t is_device_valid(const zos_i2c_device_t* i2c)
{
    return (zos_bool_t)(i2c != NULL);
}

zos_result_t dp5589_init(const zos_i2c_device_t* i2c)
{
	// Configure PIN used as GPIO
	ADP5589_WriteReg(i2c, ADP5589_PIN_CONFIG_A, 0);
	ADP5589_WriteReg(i2c, ADP5589_PIN_CONFIG_B, 0);
	ADP5589_WriteReg(i2c, ADP5589_PIN_CONFIG_C, 0);

	// Disable All interrupts
	ADP5589_WriteReg(i2c, ADP5589_GPI_INTERRUPT_EN_A, 0);
	ADP5589_WriteReg(i2c, ADP5589_GPI_INTERRUPT_EN_B, 0);
	ADP5589_WriteReg(i2c, ADP5589_GPI_INTERRUPT_EN_C, 0);

	// Disable all event reports
	ADP5589_WriteReg(i2c, ADP5589_GPI_EVENT_EN_A, 0);
	ADP5589_WriteReg(i2c, ADP5589_GPI_EVENT_EN_B, 0);
	ADP5589_WriteReg(i2c, ADP5589_GPI_EVENT_EN_C, 0);

	// Enable debounce
	ADP5589_WriteReg(i2c, ADP5589_DEBOUNCE_DIS_A, 0);
	ADP5589_WriteReg(i2c, ADP5589_DEBOUNCE_DIS_B, 0);
	ADP5589_WriteReg(i2c, ADP5589_DEBOUNCE_DIS_C, 0);
	is_inited = ZOS_TRUE;
	return ZOS_SUCCESS;
}

zos_result_t adp5589_get_input_values(const zos_i2c_device_t* i2c, uint8_t* result)
{
    zos_result_t cmd_result = ZOS_INVALID_ARG;
    if (args_valid(i2c, result))
    {
        cmd_result = ADP5589_ReadReg(i2c, ADP5589_GPI_STATUS_A, &result[0]);
        cmd_result = ADP5589_ReadReg(i2c, ADP5589_GPI_STATUS_B, &result[1]);
        cmd_result = ADP5589_ReadReg(i2c, ADP5589_GPI_STATUS_C, &result[2]);
    }
    return cmd_result;
}

zos_result_t adp5589_get_output_values(const zos_i2c_device_t* i2c, uint8_t* result)
{
	zos_result_t cmd_result = ZOS_INVALID_ARG;

	if (args_valid(i2c, result))
	{
		cmd_result =ADP5589_ReadRegWithSize(i2c, ADP5589_GPO_DATA_OUT_A, result, ADP5589_REG_SIZE);
	}

	return cmd_result;
}

zos_result_t adp5589_set_values(const zos_i2c_device_t* i2c, const uint8_t* values)
{
	zos_result_t cmd_result = ZOS_INVALID_ARG;

	if (is_device_valid(i2c))
	{
		cmd_result = ADP5589_WriteRegWithSize(i2c, ADP5589_GPO_DATA_OUT_A, values,  ADP5589_REG_SIZE);
	}

	return cmd_result;
}

zos_result_t adp5589_get_directions(const zos_i2c_device_t* i2c, uint8_t* result)
{
	zos_result_t cmd_result = ZOS_INVALID_ARG;

	if (args_valid(i2c, result))
	{
		cmd_result = ADP5589_ReadRegWithSize(i2c, ADP5589_GPIO_DIRECTION_A, result, ADP5589_REG_SIZE);
		result[0] = (~result[0]);
		result[1] = (~result[1]);
		result[2] = (~result[2]);
	}

	return cmd_result;
}

zos_result_t adp5589_set_directions(const zos_i2c_device_t* i2c, const uint8_t* directions)
{
    zos_result_t cmd_result = ZOS_INVALID_ARG;
    uint8_t directions_set[3];

    if(is_inited == ZOS_FALSE)
    {
    	dp5589_init(i2c);
    }
    if (is_device_valid(i2c))
    {
    	directions_set[0] = (~directions[0]);
    	directions_set[1] = (~directions[1]);
    	directions_set[2] = (~directions[2]);
    	ZOS_LOG("directions=%x:%x:%x", directions_set[0], directions_set[1], directions_set[2]);
        cmd_result = ADP5589_WriteRegWithSize(i2c, ADP5589_GPIO_DIRECTION_A, directions_set, ADP5589_REG_SIZE);
    }

    return cmd_result;
}

static zos_result_t rpull_to_polarities(uint8_t *rpull, uint8_t *pol)
{
	uint8_t rpull_index=0, pol_index=0;
	uint8_t rpull_shiftbits, rpull_realindex=0;
	uint8_t pol_shiftbits, pol_realindex=0;
	pol[0]=pol[1]=pol[2] = 0;

	for(;rpull_index < 5*8; rpull_index+=2 )
	{
		rpull_shiftbits = rpull_index%8;
		rpull_realindex = rpull_index/8;
		pol_shiftbits = pol_index%8;
		pol_realindex = pol_index/8;
		if(((((uint8_t)rpull[rpull_realindex] >> rpull_shiftbits) & 0x3) == 0) || ((((uint8_t)rpull[rpull_realindex] >> rpull_shiftbits) &0x3) == 2))
		{

			pol[pol_realindex] |= (1<< pol_shiftbits);
		}
		else if((((uint8_t)rpull[rpull_realindex] >> rpull_shiftbits) &0x3) == 1)
		{
			pol[pol_realindex] &= (~(1<< pol_shiftbits));
		}
		else
		{
			pol[pol_realindex] &= (~(1<< pol_shiftbits));
		}
		pol_index++;
	}
	return ZOS_SUCCESS;
}

static zos_result_t polarities_to_rpull(const uint8_t *pol, uint8_t *rpull)
{
	uint8_t rpull_index=0, pol_index=0;
	uint8_t rpull_shiftbits, rpull_realindex=0;
	uint8_t pol_shiftbits, pol_realindex=0;

	for( ;pol_index < 2*8+3; pol_index++ )
	{
		rpull_shiftbits = rpull_index%8;
		rpull_realindex = rpull_index/8;
		pol_shiftbits = pol_index%8;
		pol_realindex = pol_index/8;
		if(((uint8_t)pol[pol_realindex] >> pol_shiftbits & 0x01) == 1)
		{

			uint8_t andVal = ~(0x3 << rpull_shiftbits);
			rpull[rpull_realindex] &= andVal;
		}
		else
		{
			rpull[rpull_realindex] |= (0x01 << rpull_shiftbits);
		}
		rpull_index += 2;
	}
	return ZOS_SUCCESS;
}

zos_result_t adp5589_get_polarities(const zos_i2c_device_t* i2c, uint8_t* result)
{
	zos_result_t cmd_result = ZOS_INVALID_ARG;
	uint8_t rpull[5]={0}; // RPULL uses 5 registers to configure polarities

	if (args_valid(i2c, result))
	{
		cmd_result = ADP5589_ReadRegWithSize(i2c, ADP5589_RPULL_CONFIG_A, rpull, 5);
		rpull_to_polarities(rpull, result);
	}
	return cmd_result;
}

zos_result_t adp5589_set_polarities(const zos_i2c_device_t* i2c, const uint8_t* polarities)
{
	zos_result_t cmd_result = ZOS_INVALID_ARG;
	uint8_t rpull[5]={0}; // RPULL uses 5 registers to configure polarities
	if (is_device_valid(i2c))
	{
		polarities_to_rpull(polarities, rpull);
		cmd_result = ADP5589_WriteRegWithSize(i2c, ADP5589_RPULL_CONFIG_A, rpull, 5);
	}

	return cmd_result;
}
