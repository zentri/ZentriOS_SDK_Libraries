/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once


// Registers specific to the ArduCAM-M-2MP camera shield SPI interface


#define ARDUCHIP_RW_FLAG            0x80 // 0 is for read, 1 is for write

#define ARDUCHIP_REG_TEST1          0x00  // TEST register

#define ARDUCHIP_REG_FRAMES         0x01  // FRAME control register, Bit[2:0] = Number of frames to be captured                                                                        //On 5MP_Plus platforms bit[2:0] = 7 means continuous capture until frame buffer is full


#define ARDUCHIP_REG_MODE           0x02  // Mode register
#define MCU2LCD_MODE                0x00
#define CAM2LCD_MODE                0x01
#define LCD2MCU_MODE                0x02

#define ARDUCHIP_REG_TIM            0x03  // Timming control
#define HREF_LEVEL_MASK             0x01  // 0 = High active ,       1 = Low active
#define VSYNC_LEVEL_MASK            0x02  // 0 = High active ,       1 = Low active
#define LCD_BKEN_MASK               0x04  // 0 = Enable,                     1 = Disable
#define PCLK_DELAY_MASK             0x08  // 0 = data no delay,      1 = data delayed one PCLK
#define MODE_MASK                   0x10  // 0 = LCD mode,               1 = FIFO mode
#define FIFO_PWRDN_MASK             0x20  // 0 = Normal operation, 1 = FIFO power down
#define LOW_POWER_MODE              0x40  // 0 = Normal mode,          1 = Low power mode

#define ARDUCHIP_REG_FIFO           0x04  // FIFO and I2C control
#define FIFO_CLEAR_MASK             0x01  // clear FIFO write done flag
#define FIFO_START_MASK             0x02  // start catpure
#define FIFO_RDPTR_RST_MASK         0x10  // reset FIFO write pointer
#define FIFO_WRPTR_RST_MASK         0x20  // reset FIFO read pointer

#define ARDUCHIP_REG_GPIO_DIR       0x05  // GPIO Direction Register
#define GPIO_DIR_SENSOR_RESET       0x01  // Sensor reset IO direction, 0 = input, 1 = output
#define GPIO_DIR_SENSOR_PWR_DOWN    0x02  // Sensor power down IO direction
#define GPIO_DIR_SENSOR_PWR_ENABLE  0x03  // Sensor power enable IO direction

#define ARDUCHIP_REG_GPIO_WRITE     0x06  // GPIO Write Register
#define GPIO_RESET_MASK             0x01  //0 = Sensor reset,               1 =  Sensor normal operation
#define GPIO_PWDN_MASK              0x02  //0 = Sensor normal operation,    1 = Sensor standby
#define GPIO_PWREN_MASK             0x04    //0 = Sensor LDO disable,       1 = sensor LDO enable

#define ARDUCHIP_REG_GPIO_READ      0x45

#define BURST_FIFO_READ             0x3C  // Burst FIFO read operation
#define SINGLE_FIFO_READ            0x3D  // Single FIFO read operation

#define ARDUCHIP_REG_REV            0x40  //ArduCHIP revision
#define VER_LOW_MASK                0x3F
#define VER_HIGH_MASK               0xC0

#define ARDUCHIP_REG_STATUS         0x41  // Trigger source
#define VSYNC_MASK                  0x01
#define SHUTTER_MASK                0x02
#define CAP_DONE_MASK               0x08

#define FIFO_SIZE1                  0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2                  0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3                  0x44  //Camera write FIFO size[18:16]
