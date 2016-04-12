/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>
#include "zos_types.h"

/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef struct {
  int16_t FXOS8700CQ_ACC_AXIS_X;
  int16_t FXOS8700CQ_ACC_AXIS_Y;
  int16_t FXOS8700CQ_ACC_AXIS_Z;
} FXOS8700CQ_AccAxesRaw_t;

// note: values here assume sensor works in hybrid mode
typedef enum {
  FXOS8700CQ_ODR_400_0Hz                    =               0x00,
  FXOS8700CQ_ODR_200_0Hz                    =               0x01,
  FXOS8700CQ_ODR_100_0Hz                    =               0x02,
  FXOS8700CQ_ODR_50_0Hz                     =               0x03,
  FXOS8700CQ_ODR_25_0Hz                     =               0x04,
  FXOS8700CQ_ODR_6_25Hz                     =               0x05,
  FXOS8700CQ_ODR_3_125Hz                    =               0x06,
  FXOS8700CQ_ODR_0_7813Hz                   =               0x07,
} FXOS8700CQ_ODR_t;

typedef enum {
  FXOS8700CQ_ACC_FULLSCALE_2                =               0x00,
  FXOS8700CQ_ACC_FULLSCALE_4                =               0x01,
  FXOS8700CQ_ACC_FULLSCALE_8                =               0x02
} FXOS8700CQ_AccFullscale_t;

typedef enum {
    FXOS8700CQ_MODE_STANDBY                 =               0,
    FXOS8700CQ_MODE_ACTIVE                  =               1
} FXOS8700CQ_OperatingMode_t;

typedef enum {
    FXOS8700CQ_MODE_ACCEL_ONLY              =               0,
    FXOS8700CQ_MODE_MAGNT_ONLY              =               1,
    FXOS8700CQ_MODE_HYBRID                  =               3
} FXOS8700CQ_HybridMode_t;

typedef enum {
    FXOS8700CQ_LOW_NOISE_INACTV             =               0,
    FXOS8700CQ_LOW_NOISE_ACTV               =               1
} FXOS8700CQ_NoiseMode_t;

/******************************************************************************/
/*  Register Definitions                                                      */
/******************************************************************************/

// Accelerometer Control Register
#define FXOS8700CQ_CTRL_REG1                                0x2A
#define FXOS8700CQ_OPERATING_MODE                           (0)
#define FXOS8700CQ_LOW_NOISE                                (2)
#define FXOS8700CQ_ODR                                      (3)

// Magnetometer Control Register (to set hybrid mode)
#define FXOS8700CQ_M_CTRL_REG1                              0x5B
#define FXOS8700CQ_HYBRID                                   (0)

// Accelerometer Configuration Register (fullscale select)
#define FXOS8700CQ_XYZ_DATA_CFG                             0x0E
#define FXOS8700CQ_FSS                                      (0)

// Status Register
#define FXOS8700CQ_STATUS                                   0x00
#define FXOS8700CQ_DATA_READY                               0x08    // bit (3) presents ZYXDR

#define FXOS8700CQ_ACCEL_XOUT_H                             0x01
#define FXOS8700CQ_ACCEL_XOUT_L                             0x02
#define FXOS8700CQ_ACCEL_YOUT_H                             0x03
#define FXOS8700CQ_ACCEL_YOUT_L                             0x04
#define FXOS8700CQ_ACCEL_ZOUT_H                             0x05
#define FXOS8700CQ_ACCEL_ZOUT_L                             0x06

// Sensor I2C Slave Address
#define FXOS8700CQ_SLAVE_ADDRESS                            0x1E

/******************************************************
 *          External Function Definitions
 ******************************************************/

zos_result_t FXOS8700CQ_AccSetODR(FXOS8700CQ_ODR_t odr_value);
zos_result_t FXOS8700CQ_AccSetFullScale(FXOS8700CQ_AccFullscale_t fs);
zos_result_t FXOS8700CQ_AccGetStatusReg(uint8_t* val);
zos_result_t FXOS8700CQ_AccGetAxesRaw(FXOS8700CQ_AccAxesRaw_t* buff);
zos_result_t FXOS8700CQ_AccSetOperatingMode(FXOS8700CQ_OperatingMode_t mode);
zos_result_t FXOS8700CQ_AccSetLowNoiseMode(FXOS8700CQ_NoiseMode_t lnoise);
zos_result_t FXOS8700CQ_AccSetHybridMode(FXOS8700CQ_HybridMode_t mode);
