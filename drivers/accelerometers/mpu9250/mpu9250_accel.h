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
  int16_t MPU9250_ACC_AXIS_X;
  int16_t MPU9250_ACC_AXIS_Y;
  int16_t MPU9250_ACC_AXIS_Z;
} MPU9250_AccAxesRaw_t;

typedef enum {
  MPU9250_ODR_0_24Hz                     =               0x00,
  MPU9250_ODR_0_49Hz                     =               0x01,
  MPU9250_ODR_0_98Hz                     =               0x02,
  MPU9250_ODR_1_95Hz                     =               0x03,
  MPU9250_ODR_3_91Hz                     =               0x04,
  MPU9250_ODR_7_81Hz                     =               0x05,
  MPU9250_ODR_15_63Hz                    =               0x06,
  MPU9250_ODR_31_25Hz                    =               0x07,
  MPU9250_ODR_62_50Hz                    =               0x08,
  MPU9250_ODR_125_0Hz                    =               0x09,
  MPU9250_ODR_250_0Hz                    =               0x10,
  MPU9250_ODR_500_0Hz                    =               0x11
} MPU9250_ODR_t;

typedef enum {
  MPU9250_ACC_FULLSCALE_2                =               0x00,
  MPU9250_ACC_FULLSCALE_4                =               0x01,
  MPU9250_ACC_FULLSCALE_8                =               0x02,
  MPU9250_ACC_FULLSCALE_16               =               0x03
} MPU9250_AccFullscale_t;

typedef enum {
  MPU9250_RESET_RST                      =               0,
  MPU9250_RESET_SET                      =               1
} MPU9250_Reset_t;

typedef enum {
  MPU9250_SLEEP_RST                      =               0,
  MPU9250_SLEEP_SET                      =               1
} MPU9250_Sleep_t;

typedef enum {
  MPU9250_CYCLE_RST                      =               0,
  MPU9250_CYCLE_SET                      =               1
} MPU9250_Cycle_t;

typedef enum {
    MPU9250_STANDBY_RST                  =               0,
    MPU9250_STANDBY_SET                  =               1
} MPU9250_Standby_t;

/******************************************************************************/
/*  Register Definitions                                                      */
/******************************************************************************/
// Low Power Accelerometer ODR Control Register
#define MPU9250_LP_ACCEL_ODR                             0x1E

// Accelerometer Configuration Register (fullscale select)
#define MPU9250_ACCEL_CFG                                0x1C
#define MPU9250_FSS                                      (3)

// Accelerometer Configuration 2 Register (low pass filter)
#define MPU9250_ACCEL_CFG_2                              0x1D
#define MPU9250_ACCEL_FCHOICE                            (3)
#define MPU9250_ACCEL_DLPF_CFG                           (0)

// Power Management Register
#define MPU9250_PWR_MGMT_1                               0x6B
#define MPU9250_RESET                                    (7)
#define MPU9250_SLEEP                                    (6)
#define MPU9250_CYCLE                                    (5)
#define MPU9250_STANDBY                                  (4)

// Power Management Register 2 (enable/disable axes)
#define MPU9250_PWR_MGMT_2                               0x6C
#define MPU9250_ENABLE_XA                                (5)
#define MPU9250_ENABLE_YA                                (4)
#define MPU9250_ENABLE_ZA                                (3)
#define MPU9250_ENABLE_XG                                (2)
#define MPU9250_ENABLE_YG                                (1)
#define MPU9250_ENABLE_ZG                                (0)

// Interrupt Enable Register
#define MPU9250_INT_ENABLE                               0x38
#define MPU9250_RAW_RDY_EN                               0x01
#define MPU9250_FSYNC_INT_EN                             0x08
#define MPU9250_FIFO_OVERFLOW_EN                         0x10
#define MPU9250_WOM_EN                                   0x40

// Wake-on Motion Threshold Register
#define MPU9250_WOM_THR                                  0x1F

// Accelerometer Interrupt Control Register
#define MPU9250_ACCEL_INTEL_CTRL                         0x69
#define MPU9250_ACCEL_INTEL_EN                           (7)
#define MPU9250_ACCEL_INTEL_MODE                         (6)

// Output Register
#define MPU9250_ACCEL_XOUT_H                             0x3B
#define MPU9250_ACCEL_XOUT_L                             0x3C
#define MPU9250_ACCEL_YOUT_H                             0x3D
#define MPU9250_ACCEL_YOUT_L                             0x3E
#define MPU9250_ACCEL_ZOUT_H                             0x3F
#define MPU9250_ACCEL_ZOUT_L                             0x40

// Status Register
#define MPU9250_INTERRUPT_STATUS                         0x3A
#define MPU9250_RAW_DATA_RDY_INT                         0x01
#define MPU9250_WOM_INT                                  0x40

// Sensor I2C Slave Address
#define MPU9250_SLAVE_ADDRESS                            0x68

/******************************************************
 *          External Function Definitions
 ******************************************************/

zos_result_t MPU9250_AccSetODR(MPU9250_ODR_t odr_value);
zos_result_t MPU9250_AccSetFullScale(MPU9250_AccFullscale_t fs);
zos_result_t MPU9250_AccSetAccAxis(uint8_t enableX, uint8_t enableY, uint8_t enableZ);
zos_result_t MPU9250_AccGetStatusReg(uint8_t* val);
zos_result_t MPU9250_AccGetAxesRaw(MPU9250_AccAxesRaw_t* buff);
zos_result_t MPU9250_AccSetPowerMgmt(MPU9250_Sleep_t sleep, MPU9250_Cycle_t cycle, MPU9250_Standby_t standby);
zos_result_t MPU9250_AccGetFullScale(MPU9250_AccFullscale_t* fs);
zos_result_t MPU9250_AccSetLowPassFilter(uint8_t fchoice_val, uint8_t dlpf_cfg_val);
zos_result_t MPU9250_AccSetInterrupts(uint8_t int_en);
zos_result_t MPU9250_AccSetIntelligenceControl(uint8_t intelligence_en, uint8_t intelligence_mode);
zos_result_t MPU9250_AccSetWomThr(uint8_t thr);
zos_result_t MPU9250_AccSetGyroAxis(uint8_t enableX, uint8_t enableY, uint8_t enableZ);

