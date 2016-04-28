#ifndef __HMC5883L_MAGN_DRIVER__H
#define __HMC5883L_MAGN_DRIVER__H

#include <stdint.h>
#include "zos.h"
#include "sensor.h"
#include <math.h>

/******************************************************************************/
/*  Register Address Definitions                                              */
/******************************************************************************/
// Configuration Register
#define HMC5883L_CONFIG_REG_A	                0x00
#define HMC5883L_CONFIG_REG_B	                0x01
#define HMC5883L_CONFIG_MODE	                0x02

// Output Register
#define HMC5883L_DATA_XOUT_H	                0x03
#define HMC5883L_DATA_XOUT_L	                0x04
#define HMC5883L_DATA_ZOUT_H	                0x05
#define HMC5883L_DATA_ZOUT_L	                0x06
#define HMC5883L_DATA_YOUT_H	                0x07
#define HMC5883L_DATA_YOUT_L	                0x08

// Status Register
#define HMC5883L_STATUS	0x09

// Identification Register
#define HMC5883L_IDEN_REG_A		                0x0A
#define HMC5883L_IDEN_REG_B		                0x0B
#define HMC5883L_IDEN_REG_C		                0x0C

/******************************************************************************/
/*  Register Value Definitions                                              */
/******************************************************************************/
// Data Ready
#define HMC5883L_DATA_RDY                       0x01

// Measurement Mode
#define HMC5883L_MEASURE_MODE_NORMAL		    0x00
#define HMC5883L_MEASURE_MODE_POSITIVE		    0x01
#define HMC5883L_MEASURE_MODE_NEGATIVE		    0x02

// Data Output Rate
#define HMC5883L_DATA_OUTPUT_RATE_0_75	        0x00
#define HMC5883L_DATA_OUTPUT_RATE_1_50	        0x04
#define HMC5883L_DATA_OUTPUT_RATE_3_00	        0x08
#define HMC5883L_DATA_OUTPUT_RATE_7_50	        0x0c
#define HMC5883L_DATA_OUTPUT_RATE_15_00	        0x10
#define HMC5883L_DATA_OUTPUT_RATE_30_00	        0x14
#define HMC5883L_DATA_OUTPUT_RATE_75_00	        0x18

// Sample Numbers
#define HMC5883L_SAMPLE_NUMBER_1		        0x00
#define HMC5883L_SAMPLE_NUMBER_2		        0x20
#define HMC5883L_SAMPLE_NUMBER_4		        0x40
#define HMC5883L_SAMPLE_NUMBER_8		        0x60

// Operation Mode
#define HMC5883L_OPERATION_MODE_CONTINUOUS      0x00
#define HMC5883L_OPERATION_MODE_SINGLE			0x01
#define HMC5883L_OPERATION_MODE_IDLE            0x03

// Device Gain
#define HMC5883L_DEVICE_GAIN_DEFAULT	        0x20

// Sensor I2C Slave Address
#define HMC5883L_SLAVE_ADDRESS                  0x1E

// Default Configuration: SAMPLE_NUMBER_1 | DATA_OUTPUT_RATE_15_00 | MEASURE_MODE_NORMAL
#define HMC5883L_DEFAULT_CONFIG_VALUE		    0x10

typedef struct
{
  int16_t HMC5883L_MAGN_AXIS_X;
  int16_t HMC5883L_MAGN_AXIS_Y;
  int16_t HMC5883L_MAGN_AXIS_Z;
} HMC5883L_MagnAxesRaw_t;

typedef struct
{
  int16_t HMC5883L_MAGN_ADJUST_X;
  int16_t HMC5883L_MAGN_ADJUST_Y;
  int16_t HMC5883L_MAGN_ADJUST_Z;
} HMC5883L_MagnSensAdjust_t;

zos_result_t HMC5883L_MagnInit(void);
zos_result_t HMC5883L_MagnGetAxesRaw(HMC5883L_MagnAxesRaw_t* buff);
zos_result_t HMC5883L_MagnGetStatusReg(uint8_t* val);
#endif //HMC5883L_H
