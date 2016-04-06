/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "nt3h1x.h"
#include "nfc_driver_interface.h"


#ifndef NFC_NVM_WRITE_TIMEOUT
#define NFC_NVM_WRITE_TIMEOUT 500
#endif


static zos_result_t set_i2c_lock(void);
static zos_result_t clear_i2c_lock(void);
static zos_result_t wait_while_eeprom_busy(void);


static const zos_i2c_device_t nt3h1x_i2c_device =
{
        .port = PLATFORM_STD_I2C,
        .address = (uint16_t)NT3H1X_SLAVE_ADDRESS,
        .speed = I2C_CLOCK_LOW_SPEED,
        .retries = (uint16_t)3U
};

nt3h1x_driver_context_t nt3h1x_driver_context;


/*************************************************************************************************/
void nfc_driver_get_memory_range(nfc_driver_memory_t memory, uint8_t *start_ptr, uint8_t *stop_ptr)
{
    if(memory == NFC_DRIVER_SRAM)
    {
        *start_ptr = NT3H1X_REG_SRAM_START;
        *stop_ptr = NT3H1X_REG_SRAM_STOP;
    }
    else
    {
        *start_ptr = nt3h1x_driver_context.nvm_start;
        *stop_ptr = nt3h1x_driver_context.nvm_stop;
    }
}

/*************************************************************************************************/
zos_result_t nfc_driver_set_last_ndef_block(uint8_t block_addr)
{
    block_addr = MAX(block_addr + nt3h1x_driver_context.nvm_start - 1, nt3h1x_driver_context.nvm_start);
    return nt3h1x_write_register(NT3H1X_REG_SESSION, NT3H1X_CONFIG_LAST_NDEF_BLOCK, 0xFF, block_addr);
}

/*************************************************************************************************/
zos_result_t nfc_driver_poll_status(nfc_status_t *status_ptr)
{
    zos_result_t result;
    uint8_t status_reg;
    nfc_status_t status = 0;

    if(!ZOS_FAILED(result, nt3h1x_read_register(NT3H1X_REG_SESSION, NT3H1X_SESSION_NS_REG, &status_reg)))
    {
        if(status_reg & NT3H1X_NS_REG_NDEF_READ)
        {
            status |= NFC_NVM_READ;
        }
        if(status_reg & NT3H1X_NS_REG_SRAM_RF_RDY)
        {
            status |= NFC_DATA_TX_READY;
        }
        if(status_reg & NT3H1X_NS_REG_SRAM_I2C_RDY)
        {
            status |= NFC_DATA_RECEIVED;
        }
        if(status_reg & NT3H1X_NS_REG_RF_FIELD)
        {
            status |= NFC_RF_DETECTED;
        }
    }

    *status_ptr = status;

    return result;
}



/*************************************************************************************************/
zos_result_t nt3h1x_init(const nt3hlx_config_t *config)
{
    zos_result_t result;

    if(!ZOS_FAILED(result, zn_i2c_init(&nt3h1x_i2c_device)))
    {
        uint8_t chip_info[NT3H1X_BLOCK_SIZE];

        nt3h1x_i2c_reset();

        if(!ZOS_FAILED(result, nt3h1x_read_block(NT3H1X_REG_INFO, chip_info)))
        {
            if(chip_info[NT3H12_REG_INFO_CAPS_ID_INDEX] == NT3H11_CAPABILITIES_ID)
            {
                nt3h1x_driver_context.nvm_start = NT3H11_REG_NVM_START;
                nt3h1x_driver_context.nvm_stop = NT3H11_REG_NVM_END;
                nt3h1x_driver_context.config_reg = NT3H11_REG_CONFIG;
            }
            else if(chip_info[NT3H12_REG_INFO_CAPS_ID_INDEX] == NT3H12_CAPABILITIES_ID)
            {
                nt3h1x_driver_context.nvm_start = NT3H12_REG_NVM_START;
                nt3h1x_driver_context.nvm_stop = NT3H12_REG_NVM_END;
                nt3h1x_driver_context.config_reg = NT3H12_REG_CONFIG;
            }
            else
            {
                result = ZOS_NOT_FOUND;
            }

        }
    }

    return result;
}

/*************************************************************************************************/
void nt3h1x_i2c_reset(void)
{
#if 0
    /*
     * 9.3 Soft reset feature
        In the case where the I2C interface is constantly powered on, NTAG I2C can trigger a reset
        of the I2C interface via its soft reset feature- see Table 13.
        When this feature is enabled, if the microcontroller does not issue a stop condition
        between two start conditions, this situation will trigger a reset of the I2C interface and
        hence may hamper the communication via the I2C interface.
     */
    uint8_t dummy = 0;
    const zos_i2c_message_t msg[3] =
    {
            {
                    .buffer = &dummy,
                    .length = 1,
                    .flags = I2C_MSG_READ|I2C_MSG_NO_STOP_SEQ
            },
            {
                    .buffer = &dummy,
                    .length = 1,
                    .flags = I2C_MSG_READ
            }
    };

    zn_i2c_transfer(&nt3h1x_i2c_device, msg, 2);

    zn_rtos_delay_milliseconds(100);
#endif
}

/*************************************************************************************************/
zos_result_t nt3h1x_read_register(uint8_t address, uint8_t reg_index, uint8_t *val_ptr)
{
    zos_result_t result;

    if(!ZOS_FAILED(result, zn_i2c_master_write_reg8(&nt3h1x_i2c_device, address, reg_index)))
    {
        const zos_i2c_message_t msg =
        {
                .buffer = val_ptr,
                .length = 1,
                .flags = I2C_MSG_READ
        };

        result = zn_i2c_transfer(&nt3h1x_i2c_device, &msg, 1);
    }
    return result;
}

/*************************************************************************************************/
zos_result_t nt3h1x_write_register(uint8_t address, uint8_t reg_index, uint8_t mask, uint8_t value)
{
    const uint8_t const buffer[4] = { address, reg_index, mask, value };

    const zos_i2c_message_t msg =
    {
            .buffer = (void*)buffer,
            .length = 4,
            .flags = I2C_MSG_WRITE
    };

    // only allow access to the session register
    // chip could be bricked otherwise!!!
    if(address != NT3H1X_REG_SESSION)
    {
        return ZOS_INVALID_ARG;
    }

    return zn_i2c_transfer(&nt3h1x_i2c_device, &msg, 1);
}

/*************************************************************************************************/
zos_result_t nt3h1x_read_block(uint8_t address, uint8_t *block_data)
{
    const zos_i2c_message_t msg[2] =
    {
            [0] = {
                    .buffer = &address,
                    .length = 1,
                    .flags = I2C_MSG_WRITE
            },
            [1] = {
                    .buffer = (void*)block_data,
                    .length = NT3H1X_BLOCK_SIZE,
                    .flags = I2C_MSG_READ
            }
    };

    return zn_i2c_transfer(&nt3h1x_i2c_device, msg, 2);
}

/*************************************************************************************************/
zos_result_t nt3h1x_write_block(uint8_t address, const uint8_t *block_data)
{
    zos_result_t result;
    const zos_i2c_message_t msg[2] =
    {
            [0] = {
                    .buffer = &address,
                    .length = 1,
                    .flags = I2C_MSG_WRITE|I2C_MSG_NO_STOP_SEQ
            },
            [1] = {
                    .buffer = (void*)block_data,
                    .length = NT3H1X_BLOCK_SIZE,
                    .flags = I2C_MSG_WRITE|I2C_MSG_NO_START_SEQ
            }
    };


    // only allow block write access to user memory or SRAM
    // chip could be bricked otherwise!!
    if(!((address >= nt3h1x_driver_context.nvm_start && address <= nt3h1x_driver_context.nvm_stop) ||
         (address >= NT3H1X_REG_SRAM_START && address <= NT3H1X_REG_SRAM_STOP)))
    {
        return ZOS_INVALID_ARG;
    }

    if(!ZOS_FAILED(result, zn_i2c_transfer(&nt3h1x_i2c_device, msg, 2)))
    {
        if(address >= nt3h1x_driver_context.nvm_start && address <= nt3h1x_driver_context.nvm_stop)
        {
            zn_rtos_delay_milliseconds(5);
            if(ZOS_FAILED(result, wait_while_eeprom_busy()))
            {
            }
        }
    }

    return result;
}

/*************************************************************************************************/
zos_result_t nt3h1x_read(uint8_t address, uint8_t offset, uint8_t *data, uint8_t length)
{
    zos_result_t result;

    if(!ZOS_FAILED(result, set_i2c_lock()))
    {
        while(length > 0)
        {
            uint8_t buffer[NT3H1X_BLOCK_SIZE];
            if(ZOS_FAILED(result, nt3h1x_read_block(address, buffer)))
            {
                break;
            }
            const uint8_t chunk = MIN(length, NT3H1X_BLOCK_SIZE-offset);
            memcpy(data, &buffer[offset], chunk);
            data += chunk;
            length -= chunk;
            ++address;
            offset = 0;
        }
        clear_i2c_lock();
    }

    return result;
}

/*************************************************************************************************/
zos_result_t nt3h1x_write(uint8_t address, uint8_t offset, const uint8_t *data, uint8_t length)
{
    zos_result_t result;

    if(!ZOS_FAILED(result, set_i2c_lock()))
    {
        while(length > 0)
        {
            uint8_t buffer[NT3H1X_BLOCK_SIZE];
            const uint8_t chunk = MIN(length, NT3H1X_BLOCK_SIZE-offset);

            if((offset > 0 || chunk < NT3H1X_BLOCK_SIZE) &&
               ZOS_FAILED(result, nt3h1x_read_block(address, buffer)))
            {
                break;
            }

            memcpy(&buffer[offset], data, chunk);
            if(ZOS_FAILED(result, nt3h1x_write_block(address, buffer)))
            {
                break;
            }
            data += chunk;
            length -= chunk;
            ++address;
            offset = 0;
        }
        clear_i2c_lock();
    }

    return result;
}


/*************************************************************************************************/
static zos_result_t set_i2c_lock(void)
{
    return nt3h1x_write_register(NT3H1X_REG_SESSION, NT3H1X_SESSION_NS_REG, NT3H1X_NS_REG_I2C_LOCKED, NT3H1X_NS_REG_I2C_LOCKED);
}

/*************************************************************************************************/
static zos_result_t clear_i2c_lock(void)
{
    return nt3h1x_write_register(NT3H1X_REG_SESSION, NT3H1X_SESSION_NS_REG,NT3H1X_NS_REG_I2C_LOCKED, 0);
}

/*************************************************************************************************/
static zos_result_t wait_while_eeprom_busy(void)
{
    zos_result_t result;
    const uint32_t start_time = zn_rtos_get_time();

    do
    {
        uint8_t val;
        if(ZOS_FAILED(result, nt3h1x_read_register(NT3H1X_REG_SESSION, NT3H1X_SESSION_NS_REG, &val)))
        {
            break;
        }
        if((val & NT3H1X_NS_REG_EEPROM_ERR))
        {
            return ZOS_WRITE_ERROR;
        }
        else if(!(val & NT3H1X_NS_REG_EEPROM_BUSY))
        {
            return ZOS_SUCCESS;
        }
    }
    while((zn_rtos_get_time() - start_time) < NFC_NVM_WRITE_TIMEOUT);

    return ZOS_TIMEOUT;
}


