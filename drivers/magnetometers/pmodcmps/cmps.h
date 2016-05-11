#ifndef __CMPS_MAGN_DRIVER__H
#define __CMPS_MAGN_DRIVER__H

#include <stdint.h>
#include "zos.h"
#include "sensor.h"
#include <math.h>

/******************************************************************************/
/*  Register Address Definitions                                              */
/******************************************************************************/
//Configuration Register
#define CMPS_CONFIG_REG_A	0x00
#define CMPS_CONFIG_REG_B	0x01
#define CMPS_CONFIG_MODE	0x02

// Output Register
#define CMPS_DATA_XOUT_H	0x03
#define CMPS_DATA_XOUT_L	0x04
#define CMPS_DATA_ZOUT_H	0x05
#define CMPS_DATA_ZOUT_L	0x06
#define CMPS_DATA_YOUT_H	0x07
#define CMPS_DATA_YOUT_L	0x08

// Status Register
#define CMPS_STATUS	0x09

//Identification Register
#define CMPS_IDEN_REG_A		0xa
#define CMPS_IDEN_REG_B		0xb
#define CMPS_IDEN_REG_C		0xc

/******************************************************************************/
/*  Register Value Definitions                                              */
/******************************************************************************/
// Data Ready
#define CMPS_DATA_RDY                        0x01

//Measurement Mode
#define CMPS_MEASURE_MODE_NORMAL		0x00
#define CMPS_MEASURE_MODE_POSITIVE		0x01
#define CMPS_MEASURE_MODE_NEGATIVE		0x02

//DATA Output Rate
#define CMPS_DATA_OUTPUT_RATE_075	0x00
#define CMPS_DATA_OUTPUT_RATE_150	0x04
#define CMPS_DATA_OUTPUT_RATE_300	0x08
#define CMPS_DATA_OUTPUT_RATE_750	0x0c
#define CMPS_DATA_OUTPUT_RATE_1500	0x10
#define CMPS_DATA_OUTPUT_RATE_3000	0x14
#define CMPS_DATA_OUTPUT_RATE_7500	0x18

//Sample Numbers
#define CMPS_SAMPLE_NUMBER_1		0x00
#define CMPS_SAMPLE_NUMBER_2		0x20
#define CMPS_SAMPLE_NUMBER_4		0x40
#define CMPS_SAMPLE_NUMBER_8		0x60

// Operation Mode
#define CMPS_OPERATION_MODE_IDLE			0x03|0x02
#define CMPS_OPERATION_MODE_SINGLE			0x01
#define CMPS_OPERATION_MODE_CONTINUOUS		0x00

//Device Gain
#define CMPS_DEVICE_GAIN_DEFAULT	0x20

//Sensor I2C Slave Address
#define CMPS_SLAVE_ADDRESS          0x1e

//Default Configuration
#define CMPS_DEFAULT_CONFIG_VALUE		0x70 // CMPS_DATA_OUTPUT_RATE_1500| CMPS_OPERATION_MODE_CONTINUOUS | CMPS_MEASURE_MODE_NORMAL

typedef struct
{
  int16_t CMPS_MAGN_AXIS_X;
  int16_t CMPS_MAGN_AXIS_Y;
  int16_t CMPS_MAGN_AXIS_Z;
} CMPS_MagnAxesRaw_t;

typedef struct
{
  int16_t CMPS_MAGN_ADJUST_X;
  int16_t CMPS_MAGN_ADJUST_Y;
  int16_t CMPS_MAGN_ADJUST_Z;
} CMPS_MagnSensAdjust_t;

zos_result_t cmps_init(void);
zos_result_t CMPS_MagnGetAxesRaw(CMPS_MagnAxesRaw_t* buff);
zos_result_t CMPS_MagnGetStatusReg(uint8_t* val);
#endif //CMPS_H
