/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "si7021.h"



/******************************************************
 *                      Macros
 ******************************************************/
#define RESOLUTION_TO_REG_VAL(res) ((res & 0x1) | ((res & 0x2) << 7))
#define CLEAR_RES_BITS(reg_val) (reg_val & ~(0b10000001))

#define GET_VALUE(values) (((uint16_t)values[0] << 8) + (uint16_t)values[1])

#define VERIFY_SUCCESS(val) (val == ZOS_SUCCESS)
/******************************************************
 *                    Constants
 ******************************************************/
//Command table
#define CMD_MEAS_RH_HOLD        0xE5
#define CMD_MEAS_RH_NO_HOLD     0xF5
#define CMD_MEAS_TEMP_HOLD      0xE3
#define CMD_MEAS_TEMP_NO_HOLD   0xF3
#define CMD_READ_LAST_TEMP      0xE0
#define CMD_RESET               0xFE
#define CMD_WRITE_USER_REG      0xE6
#define CMD_READ_USER_REG       0xE7
#define CMD_WRITE_HEATER_REG    0x51
#define CMD_READ_HEATER_REG     0x11
#define CMD_READ_ID_1ST_1       0xFA
#define CMD_READ_ID_1ST_2       0x0F
#define CMD_READ_ID_2ND_1       0xFC
#define CMD_READ_ID_2ND_2       0xC9
#define CMD_READ_FW_REV_1       0x84
#define CMD_READ_FW_REV_2       0xB8

#define MASK_USER_REG_RES0      1
#define MASK_USER_REG_RES1      (1 << 7)
#define MASK_USER_REG_VDDS      (1 << 6)
#define MASK_USER_REG_HTRE      (1 << 2)

#define MASK_HEATER_REG_CUR     (0x0F)





static zos_result_t read_reg(uint8_t reg, uint8_t *result, uint16_t length);
static zos_result_t write_reg(uint8_t reg, uint8_t *data, uint16_t length);




static const zos_i2c_device_t const ZOS_I2C_SI7021 =
{
    .address = (uint16_t)SI7021_SLAVE_ADDRESS,
    .speed = I2C_CLOCK_STANDARD_SPEED,
    .retries = (uint16_t)3U,
    .flags = 0,
    .read_timeout = 1000 // timeout after 40ms of waiting for data
};




/* Initialise device */
zos_result_t si7021_init(const si7021_config_t *config)
{
    zos_result_t result = ZOS_ERROR;

    if (VERIFY_SUCCESS(si7021_resolution_set(config->resolution))      &&
        VERIFY_SUCCESS(si7021_heater_current_set(config->heater_current))   &&
        VERIFY_SUCCESS(si7021_heater_enable(config->heater_enable)))
    {
        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
/* Set resolution for relative humidity and temperature sensor */
zos_result_t si7021_resolution_set(const si7021_resolution_t res)
{
    zos_result_t result = ZOS_ERROR;
    uint8_t reg_val;

    if (VERIFY_SUCCESS(read_reg(CMD_READ_USER_REG, &reg_val, sizeof(reg_val))))
    {
        uint8_t reg_mask = RESOLUTION_TO_REG_VAL(res);

        reg_val = CLEAR_RES_BITS(reg_val) | reg_mask;

        if (VERIFY_SUCCESS(write_reg(CMD_WRITE_USER_REG, &reg_val, sizeof(reg_val))))
        {
            result = ZOS_SUCCESS;
        }
    }

    return result;
}

/*************************************************************************************************/
/* Measure and read the relative humidity */
zos_result_t si7021_humidity_read(uint16_t* humidity, const zos_bool_t use_checksum)
{
    zos_result_t result = ZOS_ERROR;
    uint8_t raw_values[2];

    //TODO add checksum support
    if (VERIFY_SUCCESS(read_reg(CMD_MEAS_RH_HOLD, raw_values, sizeof(raw_values))))
    {
        *humidity = GET_VALUE(raw_values);

        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
/* Read the ambient temperature (get last measurement or take new sample) */
zos_result_t si7021_temperature_read(uint16_t *temperature, const zos_bool_t use_last, const zos_bool_t use_checksum)
{
    zos_result_t result = ZOS_ERROR;
    uint8_t command;
    uint8_t raw_values[2];

    //TODO add checksum support
    if (use_last == ZOS_TRUE)
    {
        command = CMD_READ_LAST_TEMP;
    }
    else
    {
        command = CMD_MEAS_TEMP_HOLD;
    }

    if (VERIFY_SUCCESS(read_reg(command, raw_values, sizeof(raw_values))))
    {
        *temperature = GET_VALUE(raw_values);

        result = ZOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
/* Enable/Disable heater (clears condensation) */
zos_result_t si7021_heater_enable(const si7021_heat_en_t enable)
{
    zos_result_t result = ZOS_ERROR;
    uint8_t reg_val;

    if (VERIFY_SUCCESS(read_reg(CMD_READ_USER_REG, &reg_val, sizeof(reg_val))))
    {
        if (enable == SI7021_HEATER_ENABLE)
        {
            reg_val |= MASK_USER_REG_HTRE;
        }
        else
        {
            reg_val &= ~(MASK_USER_REG_HTRE);
        }

        result = write_reg(CMD_WRITE_USER_REG, &reg_val, sizeof(reg_val));
    }

    return result;
}

/*************************************************************************************************/
/* Set heater current */
zos_result_t si7021_heater_current_set(const si7021_heat_current_t current)
{
    zos_result_t result = ZOS_ERROR;
    uint8_t reg_val;

    if (VERIFY_SUCCESS(read_reg(CMD_READ_HEATER_REG, &reg_val, sizeof(reg_val))))
    {
        reg_val &= ~(MASK_HEATER_REG_CUR);//clear current value
        reg_val |= (current & MASK_HEATER_REG_CUR);//apply new value

        result = write_reg(CMD_WRITE_HEATER_REG, &reg_val, sizeof(reg_val));
    }

    return result;
}

/*************************************************************************************************/
/* Get VDD status */
zos_result_t si7021_vdd_status_get(si7021_vdd_status_t* status)
{
    zos_result_t result;
    uint8_t raw_value;

    result = read_reg(CMD_READ_USER_REG, &raw_value, sizeof(raw_value));

    if ((raw_value & MASK_USER_REG_VDDS) > 0)
    {
        *status = SI7021_VDD_LOW;
    }
    else
    {
        *status = SI7021_VDD_OK;
    }

    return result;
}

/*************************************************************************************************/
/* Read unique serial number of device
 * Read must be done in two steps (different 'command' bytes)
 * The first read contains upper 4 bytes with a CRC byte in between each (8-bytes total)
 * The second read contains the lower 4 bytes with a CRC byte after every 2-byte block (6-bytes total)
 */
zos_result_t si7021_serial_number_read(si7021_serial_num_t* serial)
{
    return ZOS_UNIMPLEMENTED;
}

/*************************************************************************************************/
/* Read firmware revision of sensor */
zos_result_t si7021_firmware_rev_read(si7021_firmware_rev_t* rev)
{
    return ZOS_UNIMPLEMENTED;
}

/*************************************************************************************************/
/* De-initialise driver */
void si7021_deinit(const zos_i2c_device_t* i2c)
{
}

/******************************************************
*          Internal Function Definitions
******************************************************/

static zos_result_t read_reg(uint8_t reg, uint8_t *result, uint16_t length)
{
    return zn_i2c_master_read_reg(&ZOS_I2C_SI7021, reg, result, length);
}

static zos_result_t write_reg(uint8_t reg, uint8_t *data, uint16_t length)
{
    return zn_i2c_master_write_reg(&ZOS_I2C_SI7021, reg, data, length);
}

