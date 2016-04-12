/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#ifndef __FXAS21002C_GYRO_DRIVER__H
#define __FXAS21002C_GYRO_DRIVER__H

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>
#include "zos_types.h"


/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef struct {
  int16_t FXAS21002C_GYRO_AXIS_X;
  int16_t FXAS21002C_GYRO_AXIS_Y;
  int16_t FXAS21002C_GYRO_AXIS_Z;
} FXAS21002C_GyroAxesRaw_t;

typedef enum {
    FXAS21002C_GYRO_FULLSCALE_2000            =               0x00,
    FXAS21002C_GYRO_FULLSCALE_1000            =               0x01,
    FXAS21002C_GYRO_FULLSCALE_500             =               0x02,
    FXAS21002C_GYRO_FULLSCALE_250             =               0x03
} FXAS21002C_GyroFullscale_t;

typedef enum {
    FXAS21002C_ODR_800_0Hz                    =               0x00,
    FXAS21002C_ODR_400_0Hz                    =               0x01,
    FXAS21002C_ODR_200_0Hz                    =               0x02,
    FXAS21002C_ODR_100_0Hz                    =               0x03,
    FXAS21002C_ODR_50_0Hz                     =               0x04,
    FXAS21002C_ODR_25_0Hz                     =               0x05,
    FXAS21002C_ODR_12_5Hz                     =               0x06,
    FXAS21002C_ODR_RESERVED                   =               0x07,
} FXAS21002C_ODR_t;

typedef enum {
    FXAS21002C_MODE_STANDBY                   =               0,
    FXAS21002C_MODE_ACTIVE                    =               1
} FXAS21002C_OperatingMode_t;

/******************************************************************************/
/*  Register Definitions                                                      */
/******************************************************************************/
#define FXAS21002C_STATUS                   0x07
#define FXAS21002C_DATA_READY               0x08    // bit (3) presents ZYXDR

#define FXAS21002C_CTRL_REG0                0x0D
#define FXAS21002C_FSS                      (0)

#define FXAS21002C_CTRL_REG1                0x13
#define FXAS21002C_OPERATING_MODE           (1)
#define FXAS21002C_ODR                      (2)

// Output Register
#define FXAS21002C_GYRO_XOUT_H              0x01
#define FXAS21002C_GYRO_XOUT_L              0x02
#define FXAS21002C_GYRO_YOUT_H              0x03
#define FXAS21002C_GYRO_YOUT_L              0x04
#define FXAS21002C_GYRO_ZOUT_H              0x05
#define FXAS21002C_GYRO_ZOUT_L              0x06

// Sensor I2C Slave Address
#define FXAS21002C_SLAVE_ADDRESS            0x20

/******************************************************
 *          External Function Definitions
 ******************************************************/

zos_result_t FXAS21002C_GyroSetFullScale(FXAS21002C_GyroFullscale_t fs);
zos_result_t FXAS21002C_GyroSetODR(FXAS21002C_ODR_t odr_value);
zos_result_t FXAS21002C_GyroSetOperatingMode(FXAS21002C_OperatingMode_t mode);
zos_result_t FXAS21002C_GyroGetStatusReg(uint8_t* val);
zos_result_t FXAS21002C_GyroGetAxesRaw(FXAS21002C_GyroAxesRaw_t* buff);

#endif /* __FXAS21002C_GYRO_DRIVER__H */

