/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#ifndef __MPU9250_MAGN_DRIVER__H
#define __MPU9250_MAGN_DRIVER__H

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>
#include "zos.h"
#include "sensor.h"

/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef enum {
  MPU9250_AK8963_PWR_DOWN                 =             0x0,
  MPU9250_AK8963_SNGL_MEASURE             =             0x1,
  MPU9250_AK8963_CONT_MEASURE_1           =             0x2,
  MPU9250_AK8963_CONT_MEASURE_2           =             0x6,
  MPU9250_AK8963_EXT_MEASURE              =             0x4,
  MPU9250_AK8963_SELF_TEST                =             0x8,
  MPU9250_AK8963_FUSE_ROM                 =             0xF
} MPU9250_AK8963_MagnOperationMode_t;


typedef struct {
  int16_t MPU9250_AK8963_MAGN_AXIS_X;
  int16_t MPU9250_AK8963_MAGN_AXIS_Y;
  int16_t MPU9250_AK8963_MAGN_AXIS_Z;
} MPU9250_AK8963_MagnAxesRaw_t;

typedef struct {
  int16_t MPU9250_AK8963_MAGN_ADJUST_X;
  int16_t MPU9250_AK8963_MAGN_ADJUST_Y;
  int16_t MPU9250_AK8963_MAGN_ADJUST_Z;
} MPU9250_AK8963_MagnSensAdjust_t;

typedef enum {
  MPU9250_AK8963_MAGN_OUTPUT_14           =               0x00,
  MPU9250_AK8963_MAGN_OUTPUT_16           =               0x01
} MPU9250_AK8963_MagnOutBitSize_t;

typedef enum {
  MPU9250_BYPASS_DISABLED                 =             0,
  MPU9250_BYPASS_ENABLED                  =             1
} MPU9250_I2C_Bypass_t;

/******************************************************************************/
/*  Register Definitions                                                      */
/******************************************************************************/
// Master I2C Enable (bypass mode to enable AK8963 magnetometer)
#define MPU9250_I2C_MASTER_BYPASS                       0x37
#define MPU9250_BYPASS_EN                               (1)

// AK8963 Magnetometer Control Register
#define MPU9250_AK8963_CTRL                             0x0A
#define MPU9250_AK8963_OUTPUT_SIZE                      0x10
#define MPU9250_AK8963_OUTPUT                           (4)

// Output Register
#define MPU9250_AK8963_MAGN_XOUT_L                      0x03
#define MPU9250_AK8963_MAGN_XOUT_H                      0x04
#define MPU9250_AK8963_MAGN_YOUT_L                      0x05
#define MPU9250_AK8963_MAGN_YOUT_H                      0x06
#define MPU9250_AK8963_MAGN_ZOUT_L                      0x07
#define MPU9250_AK8963_MAGN_ZOUT_H                      0x08

// Sensitivity Adjustment Registers
#define MPU9250_AK8963_MAGN_ASAX                        0x10
#define MPU9250_AK8963_MAGN_ASAY                        0x11
#define MPU9250_AK8963_MAGN_ASAZ                        0x12

// Status Register
#define MPU9250_AK8963_STATUS                           0x02
#define MPU9250_AK8963_DATA_RDY                         0x01

// Sensor I2C Slave Address
#define MPU9250_SLAVE_ADDRESS                           0x68
#define AK8963_SLAVE_ADDRESS                            0x0C

/******************************************************
 *          External Function Definitions
 ******************************************************/

zos_result_t MPU9250_MagnSetI2CBypass(MPU9250_I2C_Bypass_t bypass);
zos_result_t MPU9250_AK8963_MagnSetOprMode(MPU9250_AK8963_MagnOperationMode_t mode);
zos_result_t MPU9250_AK8963_MagnGetStatusReg(uint8_t* val);
zos_result_t MPU9250_AK8963_MagnGetAxesRaw(MPU9250_AK8963_MagnAxesRaw_t* buff);
zos_result_t MPU9250_AK8963_MagnSetOutBitSize(MPU9250_AK8963_MagnOutBitSize_t bit_size);
zos_result_t MPU9250_AK8963_MagnGetOutBitSize(MPU9250_AK8963_MagnOutBitSize_t* bit_size);
zos_result_t MPU9250_AK8963_MagnReadSensAdj(MPU9250_AK8963_MagnSensAdjust_t* adjust_data);
#endif /* __MPU9250_MAGN_DRIVER__H */

