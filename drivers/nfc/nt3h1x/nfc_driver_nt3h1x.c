/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "nfc.h"
#include "nt3h1x.h"
#include "nfc_driver_interface.h"



static zos_result_t set_sram_enabled(zos_bool_t enabled);


/*************************************************************************************************/
zos_result_t nfc_driver_init(const nfc_config_t *config)
{
    UNUSED_PARAMETER(config);

    return nt3h1x_init(NULL);
}

/*************************************************************************************************/
zos_result_t nfc_driver_nvm_erase_all(zos_bool_t add_empty_record)
{
    zos_result_t result = ZOS_SUCCESS;
    uint8_t buffer[NT3H1X_BLOCK_SIZE];
    uint8_t start_block, stop_block;

    nfc_driver_get_memory_range(NFC_DRIVER_EEPROM, &start_block, &stop_block);

    memset(buffer, 0, NT3H1X_BLOCK_SIZE);

    for(uint8_t addr = start_block; addr <= stop_block; ++addr)
    {
        if(ZOS_FAILED(result, nt3h1x_write(addr, 0, buffer, NT3H1X_BLOCK_SIZE)))
        {
            break;
        }
    }

    if(result == ZOS_SUCCESS && add_empty_record)
    {
        // add an empty record plus the terminator
        uint8_t buffer[3] = {TLV_TYPE_NDEF, 0, TLV_TYPE_TERMINATOR};
        result = nt3h1x_write(start_block, 0, buffer, 3);
    }

    return result;
}

/*************************************************************************************************/
zos_result_t nfc_driver_add_tlv_block(nfc_driver_memory_t memory, uint8_t *buffer, uint16_t length)
{
    zos_result_t result;
    uint8_t block = 0;
    uint8_t offset = 0;

    if(ZOS_FAILED(result, nfc_driver_find_next_tlv_record(memory, &block, &offset, TLV_TYPE_TERMINATOR)))
    {
        if(result != ZOS_NOT_FOUND)
        {
            return result;
        }
        block = 0;
        offset = 0;
    }

    if(ZOS_FAILED(result, nt3h1x_write(block, offset, buffer, length)))
    {
    }

    return result;
}

/*************************************************************************************************/
zos_result_t nfc_driver_read_tlv_block(nfc_driver_memory_t memory, uint8_t *block_ptr, uint8_t *block_offset_ptr, uint8_t *buffer)
{
    zos_result_t result;

    if(!ZOS_FAILED(result, nfc_driver_find_next_tlv_record(memory, block_ptr, block_offset_ptr, TLV_TYPE_NDEF)))
    {
        uint8_t block = *block_ptr;
        uint8_t offset = *block_offset_ptr;
        uint8_t tlv_header[TLV_HEADER_LEN];

        if(ZOS_FAILED(result, nt3h1x_read(block, offset, tlv_header, TLV_HEADER_LEN)))
        {
        }
        else if(tlv_header[1] == 0xFF)
        {
            // 3 byte lengths not currently supported
            result = ZOS_UNSUPPORTED;
        }
        else if(ZOS_FAILED(result, nt3h1x_read(block, offset, buffer, tlv_header[1] + TLV_HEADER_LEN)))
        {
        }
        else
        {
            const uint16_t next_record_offset = (block-NT3H11_REG_NVM_START)*NT3H1X_BLOCK_SIZE + offset + tlv_header[1] + TLV_HEADER_LEN;
            *block_ptr = next_record_offset / NT3H1X_BLOCK_SIZE;
            *block_offset_ptr = next_record_offset % NT3H1X_BLOCK_SIZE;
        }
    }

    return result;
}

/*************************************************************************************************/
zos_result_t nfc_driver_read_bytes(uint32_t block, uint32_t offset, void *buffer, uint32_t length)
{
    return nt3h1x_read(block, offset, buffer, length);
}

/*************************************************************************************************/
zos_result_t nfc_driver_write_bytes(uint32_t block, uint32_t offset, const void *buffer, uint32_t length)
{
    return nt3h1x_write(block, offset, buffer, length);
}


/*************************************************************************************************/
zos_result_t nfc_driver_set_pass_through_enabled(nfc_direction_t direction, zos_gpio_t gpio, zos_gpio_irq_callback_t callback)
{
    zos_result_t result;

    if(gpio != ZOS_GPIO_INVALID)
    {
        zn_gpio_irq_disable(gpio);
    }

    nt3h1x_driver_context.direction = direction;

    if(direction == NFC_NONE)
    {
        if(ZOS_FAILED(result, set_sram_enabled(ZOS_FALSE)))
        {
        }
        // defines the event upon which the signal output on the FD pin is brought up:
        // 10b… if the field is switched off or the last page
        // of the NDEF message has been read (defined in LAST_NDEF_BLOCK)
        // defines the event upon which the signal output on the FD pin is brought down
        // 00b… if the field is switched on
        else if(ZOS_FAILED(result, nt3h1x_write_register(NT3H1X_REG_SESSION,
                                                         NT3H1X_SESSION_NC_REG,
                                                         NT3H1X_NC_REG_FD_OFF_MASK  |NT3H1X_NC_REG_FD_ON_MASK  |NT3H1X_NC_REG_PTHRU_DIR,
                                                         NT3H1X_NC_REG_FD_OFF_VAL(2)|NT3H1X_NC_REG_FD_ON_VAL(2)|NT3H1X_PTHRU_DIR_RF_TO_I2C)))
        {
        }
    }
    else
    {
        const uint8_t dir = (direction == NFC_BUS_TO_RF) ? NT3H1X_PTHRU_DIR_I2C_TO_RF : NT3H1X_PTHRU_DIR_RF_TO_I2C;

        if(ZOS_FAILED(result, set_sram_enabled(ZOS_FALSE)))
        {
        }
        // defines the event upon which the signal output on the FD pin is brought up:
        // 11b... (if FD_ON = 11b) if the field is switched off
        // or if last data is read by I²C (in Pass-through mode RF ---> I²C)
        // or last data is written by I²C (in Pass-through mode I²C---> RF)
        // defines the event upon which the signal output on the FD pin is brought down:
        // 11b (in Pass-through mode RF-->I²C) if the data is ready to be read from the I²C interface
        // 11b (in Pass-through mode I²C--> RF) if the data is read by the RF interface
        else if(ZOS_FAILED(result, nt3h1x_write_register(NT3H1X_REG_SESSION,
                                                         NT3H1X_SESSION_NC_REG,
                                                         NT3H1X_NC_REG_PTHRU_DIR | NT3H1X_NC_REG_FD_OFF_MASK    | NT3H1X_NC_REG_FD_ON_MASK,
                                                         dir                     | NT3H1X_NC_REG_FD_OFF_VAL(3)))| NT3H1X_NC_REG_FD_ON_VAL(3))
        {

        }
        else if(ZOS_FAILED(result, set_sram_enabled(ZOS_TRUE)))
        {
        }
    }

    if(gpio != ZOS_GPIO_INVALID && result == ZOS_SUCCESS)
    {
        zn_gpio_init(gpio, GPIO_INPUT_HIGHZ, ZOS_FALSE);
        zn_gpio_irq_enable(gpio, GPIO_IRQ_TRIGGER_FALLING_EDGE, callback, NULL);
    }

    return result;
}



/*************************************************************************************************/
static zos_result_t set_sram_enabled(zos_bool_t enabled)
{
    const uint8_t val = enabled ? NT3H1X_NC_REG_PTHRU_ON : 0;
    return nt3h1x_write_register(NT3H1X_REG_SESSION, NT3H1X_SESSION_NC_REG, NT3H1X_NC_REG_PTHRU_ON, val);
}

/*************************************************************************************************/
//static zos_result_t set_sram_mirror_enabled(zos_bool_t enabled)
//{
//    const uint8_t val = enabled ? NT3H1X_NC_REG_SRAM_MIRROR_ON : 0;
//    return nt3h1x_write_register(NT3H1X_REG_SESSION, NT3H1X_SESSION_NC_REG, NT3H1X_NC_REG_SRAM_MIRROR_ON, val);
//}

