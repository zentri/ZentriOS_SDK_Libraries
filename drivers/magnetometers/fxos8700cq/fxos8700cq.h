/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#ifndef __FXOS8700CQ_MAGN_DRIVER__H
#define __FXOS8700CQ_MAGN_DRIVER__H

/******************************************************************************/
/*  Includes                                                                  */
/******************************************************************************/
#include <stdint.h>
#include "zos.h"
#include "sensor.h"

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct {
  int16_t FXOS8700CQ_MAGN_AXIS_X;
  int16_t FXOS8700CQ_MAGN_AXIS_Y;
  int16_t FXOS8700CQ_MAGN_AXIS_Z;
} FXOS8700CQ_MagnAxesRaw_t;

typedef enum {
    FXOS8700CQ_MAGN_OSR_0                   =             0x0,
    FXOS8700CQ_MAGN_OSR_1                   =             0x1,
    FXOS8700CQ_MAGN_OSR_2                   =             0x2,
    FXOS8700CQ_MAGN_OSR_3                   =             0x3,
    FXOS8700CQ_MAGN_OSR_4                   =             0x4,
    FXOS8700CQ_MAGN_OSR_5                   =             0x5,
    FXOS8700CQ_MAGN_OSR_6                   =             0x6,
    FXOS8700CQ_MAGN_OSR_7                   =             0x7
} FXOS8700CQ_MagnOSR_t;

typedef enum {
    FXOS8700CQ_MAGN_MODE_STANDBY            =               0,
    FXOS8700CQ_MAGN_MODE_ACTIVE             =               1
} FXOS8700CQ_OperatingMode_t;

typedef enum {
    FXOS8700CQ_MAGN_MODE_ACCEL_ONLY         =               0,
    FXOS8700CQ_MAGN_MODE_MAGNT_ONLY         =               1,
    FXOS8700CQ_MAGN_MODE_HYBRID             =               3
} FXOS8700CQ_HybridMode_t;

/******************************************************************************/
/*  Register Definitions                                                      */
/******************************************************************************/

// Device control Register
#define FXOS8700CQ_CTRL_REG1                                0x2A
#define FXOS8700CQ_OPERATING_MODE                           (0)
#define FXOS8700CQ_LOW_NOISE                                (2)
#define FXOS8700CQ_ODR                                      (3)

// Magnetometer Control Register 1
#define FXOS8700CQ_M_CTRL_REG1                              0x5B
#define FXOS8700CQ_AUTO_CALIB                               (7)
#define FXOS8700CQ_ONE_SHOT_RESET                           (6)
#define FXOS8700CQ_ONE_SHOT_MEASUREMENT                     (5)
#define FXOS8700CQ_OSR                                      (2)
#define FXOS8700CQ_HYBRID                                   (0)

// Magnetometer Control Register 2
#define FXOS8700CQ_M_CTRL_REG2                              0x5C
#define FXOS8700CQ_AUTO_INC                                 (5)

// Accelerometer Configuration Register
#define FXOS8700CQ_XYZ_DATA_CFG                             0x0E

// Output Register
#define FXOS8700CQ_MAGN_XOUT_H                              0x33
#define FXOS8700CQ_MAGN_XOUT_L                              0x34
#define FXOS8700CQ_MAGN_YOUT_H                              0x35
#define FXOS8700CQ_MAGN_YOUT_L                              0x36
#define FXOS8700CQ_MAGN_ZOUT_H                              0x37
#define FXOS8700CQ_MAGN_ZOUT_L                              0x38

// Status Register
#define FXOS8700CQ_STATUS                                   0x32
#define FXOS8700CQ_DATA_READY                               0x01

// Sensor I2C Slave Address
#define FXOS8700CQ_SLAVE_ADDRESS                            0x1E

/******************************************************
 *          External Function Definitions
 ******************************************************/

zos_result_t FXOS8700CQ_MagnSetHybridMode(FXOS8700CQ_HybridMode_t mode);
zos_result_t FXOS8700CQ_MagnSetControlParams(zos_bool_t auto_calib, zos_bool_t os_rest, zos_bool_t os_meas);
zos_result_t FXOS8700CQ_MagnSetOversampleRatio(FXOS8700CQ_MagnOSR_t osr);
zos_result_t FXOS8700CQ_MagnSetOperatingMode(FXOS8700CQ_OperatingMode_t mode);
zos_result_t FXOS8700CQ_MagnSetAutoIncrement(zos_bool_t auto_inc);
zos_result_t FXOS8700CQ_MagnGetStatusReg(uint8_t* val);
zos_result_t FXOS8700CQ_MagnGetAxesRaw(FXOS8700CQ_MagnAxesRaw_t* buff);

#endif /* __FXOS8700CQ_MAGN_DRIVER__H */

