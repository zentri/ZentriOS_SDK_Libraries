/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LSM6DS0_PLATFORM_H
#define __LSM6DS0_PLATFORM_H

#include <stdint.h>
#include "zos.h"
#include "sensor.h"

typedef struct
{
    int16_t AXIS_X;
    int16_t AXIS_Y;
    int16_t AXIS_Z;
} AxesRaw_t;

typedef enum
{
    LSM6DS0_G_ODR_PD                       =    ((uint8_t)0x00), /*!< Output Data Rate: Power-down*/
    LSM6DS0_G_ODR_14_9HZ                   =    ((uint8_t)0x20), /*!< Output Data Rate: 14.9 Hz, cutoff 5Hz */
    LSM6DS0_G_ODR_59_5HZ                   =    ((uint8_t)0x40), /*!< Output Data Rate: 59.5 Hz, cutoff 19Hz */
    LSM6DS0_G_ODR_119HZ                    =    ((uint8_t)0x60), /*!< Output Data Rate: 119 Hz, cutoff 38Hz*/
    LSM6DS0_G_ODR_238HZ                    =    ((uint8_t)0x80), /*!< Output Data Rate: 238 Hz, cutoff 76Hz*/
    LSM6DS0_G_ODR_476HZ                    =    ((uint8_t)0xA0), /*!< Output Data Rate: 476 Hz, cutoff 100Hz*/
    LSM6DS0_G_ODR_952HZ                    =    ((uint8_t)0xC0), /*!< Output Data Rate: 952 Hz, cutoff 100Hz*/
} LSM6DS0_ODR_t;

typedef enum
{
    LSM6DS0_FULLSCALE_2                    =    0x00,
    LSM6DS0_FULLSCALE_4                    =    0x02,
    LSM6DS0_FULLSCALE_8                    =    0x03,
    LSM6DS0_FULLSCALE_16                   =    0x01
} LSM6DS0_Fullscale_t;

typedef enum
{
    LSM6DS0_BLE_LSB                        =    0x00,
    LSM6DS0_BLE_MSB                        =    0x01
} LSM6DS0_Endianess_t;

typedef enum
{
    LSM6DS0_X_ENABLE                       =    0x08,
    LSM6DS0_X_DISABLE                      =    0x00,
    LSM6DS0_Y_ENABLE                       =    0x10,
    LSM6DS0_Y_DISABLE                      =    0x00,
    LSM6DS0_Z_ENABLE                       =    0x20,
    LSM6DS0_Z_DISABLE                      =    0x00
} LSM6DS0_AXISenable_t;

#define LSM6DS0_XG_WHO_AM_I_ADDR                0x0F
#define LSM6DS0_STATUS_REG                      0x17
#define LSM6DS0_XG_CTRL_REG5_XL                 0x1F
#define LSM6DS0_XG_CTRL_REG6_XL                 0x20
#define LSM6DS0_XG_CTRL_REG8_XL                 0x22
#define LSM6DS0_XG_OUT_X_L_XL                   0x28
#define LSM6DS0_XG_OUT_X_H_XL                   0x29
#define LSM6DS0_XG_OUT_Y_L_XL                   0x2A
#define LSM6DS0_XG_OUT_Y_H_XL                   0x2B
#define LSM6DS0_XG_OUT_Z_L_XL                   0x2C
#define LSM6DS0_XG_OUT_Z_H_XL                   0x2D

#define I_AM_LSM6DS0_XG                         0x68
#define LSM6DS0_I2C_ADDRESS                     0x6B
#define LSM6DS0_I2C_ACCT_DATA_READY_BIT         0x01

zos_result_t LSM6DS0_Init(void);
zos_result_t LSM6DS0_SetODR(LSM6DS0_ODR_t ov);
zos_result_t LSM6DS0_SetAxis(LSM6DS0_AXISenable_t axis);
zos_result_t LSM6DS0_SetFullScale(LSM6DS0_Fullscale_t fs);
zos_result_t LSM6DS0_SetBLE(LSM6DS0_Endianess_t ble);
zos_result_t LSM6DS0_ResetDevice(void);
zos_result_t LSM6DS0_GetStatusReg(uint8_t* val);
zos_result_t LSM6DS0_GetAccAxesRaw(AxesRaw_t *pData);
#endif /* __LSM6DS0_PLATFORM_H */
