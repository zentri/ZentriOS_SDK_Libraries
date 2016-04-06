/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once


#include "zos.h"
#include "nfc.h"

#define NT3H1X_SLAVE_ADDRESS 0x55
#define NT3H1X_BLOCK_SIZE 16

#define NT3H1X_REG_INFO         0x00
#define NT3H1X_REG_SRAM_START   0xF8
#define NT3H1X_REG_SRAM_STOP    0xFB
#define NT3H1X_REG_SESSION      0xFE


#define NT3H11_REG_NVM_START    0x01
#define NT3H11_REG_NVM_END      0x37
#define NT3H11_REG_CONFIG       0x3A
#define NT3H11_CAPABILITIES_ID  0x6D

#define NT3H12_REG_NVM_START    0x01
#define NT3H12_REG_NVM_END      0x77
#define NT3H12_REG_CONFIG       0x7A
#define NT3H12_CAPABILITIES_ID  0xEA

#define NT3H12_REG_INFO_CAPS_ID_INDEX       14
#define NT3H1X_CONFIG_NC_REG                0x00
#define NT3H1X_CONFIG_LAST_NDEF_BLOCK       0x01
#define NT3H1X_CONFIG_SRAM_MIRROR_BLOCK     0x02
#define NT3H1X_CONFIG_WDT_LS                0x03
#define NT3H1X_CONFIG_WDT_MS                0x04
#define NT3H1X_CONFIG_I2C_CLOCK_STR         0x05
#define NT3H1X_CONFIG_REG_LOCK              0x06


#define NT3H1X_SESSION_NC_REG               0x00
#define NT3H1X_SESSION_LAST_NDEF_BLOCK      0x01
#define NT3H1X_SESSION_SRAM_MIRROR_BLOCK    0x02
#define NT3H1X_SESSION_WDT_LS               0x03
#define NT3H1X_SESSION_WDT_MS               0x04
#define NT3H1X_SESSION_I2C_CLOCK_STR        0x05
#define NT3H1X_SESSION_NS_REG               0x06

#define NT3H1X_NC_REG_PTHRU_DIR             (1 << 0)
#define NT3H1X_NC_REG_SRAM_MIRROR_ON        (1 << 1)
#define NT3H1X_NC_REG_FD_ON_MASK            (3 << NT3H1X_NC_REG_FD_ON_SHIFT)
#define NT3H1X_NC_REG_FD_OFF_MASK           (3 << NT3H1X_NC_REG_FD_OFF_SHIFT)
#define NT3H1X_NC_REG_PTHRU_ON              (1 << 6)
#define NT3H1X_NC_REG_I2C_RST_ON            (1 << 7)

#define NT3H1X_NC_REG_FD_ON_SHIFT           2
#define NT3H1X_NC_REG_FD_OFF_SHIFT          4
#define NT3H1X_NC_REG_FD_ON_VAL(val)        (val << NT3H1X_NC_REG_FD_ON_SHIFT)
#define NT3H1X_NC_REG_FD_OFF_VAL(val)       (val << NT3H1X_NC_REG_FD_OFF_SHIFT)

#define NT3H1X_PTHRU_DIR_I2C_TO_RF          0
#define NT3H1X_PTHRU_DIR_RF_TO_I2C          1

#define NT3H1X_NS_REG_RF_FIELD              (1 << 0)
#define NT3H1X_NS_REG_EEPROM_BUSY           (1 << 1)
#define NT3H1X_NS_REG_EEPROM_ERR            (1 << 2)
#define NT3H1X_NS_REG_SRAM_RF_RDY           (1 << 3)
#define NT3H1X_NS_REG_SRAM_I2C_RDY          (1 << 4)
#define NT3H1X_NS_REG_RF_LOCKED             (1 << 5)
#define NT3H1X_NS_REG_I2C_LOCKED            (1 << 6)
#define NT3H1X_NS_REG_NDEF_READ             (1 << 7)



typedef struct
{
    uint8_t x;
} nt3hlx_config_t;

typedef struct
{
    uint8_t nvm_start;
    uint8_t nvm_stop;
    uint8_t config_reg;
    nfc_direction_t direction;
} nt3h1x_driver_context_t;



extern nt3h1x_driver_context_t nt3h1x_driver_context;


zos_result_t nt3h1x_init(const nt3hlx_config_t *config);

void nt3h1x_i2c_reset(void);

zos_result_t nt3h1x_read_register(uint8_t address, uint8_t reg_index, uint8_t *val_ptr);

zos_result_t nt3h1x_write_register(uint8_t address, uint8_t reg_index, uint8_t mask, uint8_t value);

zos_result_t nt3h1x_read_block(uint8_t address, uint8_t *block_data);

zos_result_t nt3h1x_write_block(uint8_t address, const uint8_t *block_data);

zos_result_t nt3h1x_read(uint8_t address, uint8_t offset, uint8_t *data, uint8_t length);

zos_result_t nt3h1x_write(uint8_t address, uint8_t offset, const uint8_t *data, uint8_t length);
