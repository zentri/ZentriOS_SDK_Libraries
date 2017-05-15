/**************************************************************************//**
 * @file main.c
 * @brief SPIDRV test for EFM32/EZR32/EFR32
 * @version INTERNAL
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <string.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_core.h"

#include "unity.h"
#if defined( GPIO_ROUTE_SWOPEN ) || defined( GPIO_ROUTEPEN_SWVPEN )
#include "unity_swo.h"
#endif
#include "rtcdriver.h"

#include "spidrv.h"

#if defined( EFR32MG2_BRD4157A )
static SPIDRV_Init_t initDataMaster1 = SPIDRV_MASTER_USART3;
#elif defined( _SILICON_LABS_32B_SERIES_1 )
static SPIDRV_Init_t initDataMaster1 = SPIDRV_MASTER_USART1;
#else
static SPIDRV_Init_t initDataMaster1 = SPIDRV_MASTER_USART2;
#endif

#if defined( EFR32MG2_BRD4157A )
static SPIDRV_Init_t initDataMaster0 = SPIDRV_MASTER_USART2;
#elif defined( _EZR32_LEOPARD_FAMILY ) || defined( _EZR32_WONDER_FAMILY )
static SPIDRV_Init_t initDataMaster0 = SPIDRV_MASTER_USART1;
#else
static SPIDRV_Init_t initDataMaster0 = SPIDRV_MASTER_USART0;
#endif

#if defined( EFR32MG2_BRD4157A )
static SPIDRV_Init_t initDataSlave1 = SPIDRV_SLAVE_USART3;
#elif defined( _SILICON_LABS_32B_SERIES_1 )
static SPIDRV_Init_t initDataSlave1 = SPIDRV_SLAVE_USART1;
#else
static SPIDRV_Init_t initDataSlave1 = SPIDRV_SLAVE_USART2;
#endif

#if defined( EFR32MG2_BRD4157A )
static SPIDRV_Init_t initDataSlave0 = SPIDRV_SLAVE_USART2;
#elif defined( _EZR32_LEOPARD_FAMILY ) || defined( _EZR32_WONDER_FAMILY )
static SPIDRV_Init_t initDataSlave0 = SPIDRV_SLAVE_USART1;
#else
static SPIDRV_Init_t initDataSlave0 = SPIDRV_SLAVE_USART0;
#endif

static SPIDRV_Init_t *pInitDataMaster;
static SPIDRV_Init_t *pInitDataSlave;

static SPIDRV_HandleData_t  spi_handle1;
static SPIDRV_HandleData_t  spi_handle2;
static SPIDRV_Handle_t  handleOne = &spi_handle1;
static SPIDRV_Handle_t  handleTwo = &spi_handle2;

static const uint8_t pattern1[] = { 0xAA, 0x01, 0x02, 0x03, 0x04 };
static const uint8_t pattern2[] = { 0x55, 0x11, 0x22, 0x33, 0x44 };
static uint8_t rxBuffer1[5];
static uint8_t rxBuffer2[5];

static void test_SPIDRV( void );
static void test_driver_init( void );
static void test_master_receive( void );
static void test_master_transmit( void );
static void test_master_transmit_blocking( void );
static void test_transfer_status( void );
static void test_mastertx_slaverx( void );
static void test_masterrx_slavetx( void );
static void test_master_slave_transfer( void );
static void test_master_slave_transfer_singlebuffers( void );
static void test_master_slave_single_transfer( void );
static void test_misc( void );
static void test_slave_timeout( void );
static void test_abort_transfer( void );
static void test_slave_delayed_start( void );

/**************************************************************************//**
 * @brief  Main function
 *
 * @note   This test relies on the following connections (use protoboard):
 *
 *         --- EFM Classic ---
 *         1: US2#0 <-> 0: US0#0
 *         PC2      <-> PE10
 *         PC3      <-> PE11
 *         PC4      <-> PE12
 *         PC5      <-> PE13
 *
 *        -- EFR32MG1_BRD4150B, EFR32MG2_BRD4156A, EFR32MG13_BRD4158A --
 *         0: USART0#0/0/14/12  <-> 1: USART1#11
 *  MOSI   PA0  (EXP_HEADER_12) <-> PC6 (EXP_HEADER_4)
 *  MISO   PA1  (EXP_HEADER_14) <-> PC7 (EXP_HEADER_6)
 *  SCLK   PC11 (EXP_HEADER_16) <-> PC8 (EXP_HEADER_8)
 *  CS     PC10 (EXP_HEADER_15) <-> PC9 (EXP_HEADER_10)
 *
 *        -- EFR32MG13_BRD4165A --
 *         0: USART0[0,0,14,12] <-> 1: USART1[11,11,4,15]
 *  MOSI   PA0  (EXP_HEADER_12) <-> PC6  (EXP_HEADER_4)
 *  MISO   PA1  (EXP_HEADER_14) <-> PC7  (EXP_HEADER_6)
 *  SCLK   PC11 (EXP_HEADER_16) <-> PB11 (EXP_HEADER_8)
 *  CS     PC10 (EXP_HEADER_15) <-> PD10 (EXP_HEADER_7)
 *
 *        -- EFR32MG2_BRD4157A --
 *         0: USART2#1          <-> 1: USART3#10/30
 *  MOSI   PA6  (EXP_HEADER_4)  <-> PB6 (EXP_HEADER_12)
 *  MISO   PA7  (EXP_HEADER_6)  <-> PB8 (EXP_HEADER_14)
 *  SCLK   PA8  (EXP_HEADER_8)  <-> PD8 (EXP_HEADER_3)
 *  CS     PA9  (EXP_HEADER_10) <-> PD9 (EXP_HEADER_5)
 *
 *        -- EFM32PG STK --
 *         0: USART0#0/13       <-> 1: USART1#11
 *  MOSI   PA0  (EXP_HEADER_12) <-> PC6 (EXP_HEADER_4)
 *  MISO   PA1  (EXP_HEADER_14) <-> PC7 (EXP_HEADER_6)
 *  SCLK   PC11 (EXP_HEADER_16) <-> PC8 (EXP_HEADER_8)
 *  CS     PC10 (EXP_HEADER_15) <-> PC9 (EXP_HEADER_10)
 *
 *        -- EZR32LG/WG WSTK --
 *         0: USART1#1          <->     1: USART2#1
 *  MOSI   PD0 (EXP_HEADER_4)   <->     PB3  (WSTK_P38)
 *  MISO   PD1 (EXP_HEADER_6)   <->     PB4  (WSTK_P34)
 *  SCLK   PD2 (EXP_HEADER_8)   <->     PB5  (WSTK_P35)
 *  CS     PD3 (EXP_HEADER_10)  <->     PB6  (WSTK_P36)
 *
 *****************************************************************************/
int main( void )
{
#if defined(EFM32GG_DK3750) || defined(EFM32LG_DK3650) || defined(EFM32WG_DK3850) || defined(EFM32_GXXX_DK)
  // Set correct locations for protoboard
  initDataMaster0.portLocation = _USART_ROUTE_LOCATION_LOC0;
  initDataMaster1.portLocation = _USART_ROUTE_LOCATION_LOC0;
  initDataSlave0.portLocation = _USART_ROUTE_LOCATION_LOC0;
  initDataSlave1.portLocation = _USART_ROUTE_LOCATION_LOC0;
#endif

#if defined(EFR32MG1_BRD4150A)  \
  || defined(EFR32MG1_BRD4150B) \
  || defined(EFR32MG1_BRD4151A) \
  || defined(EFR32MG1_BRD4153A) \
  || defined(EFR32MG2_BRD4156A) \
  || defined(EFR32MG13_BRD4158A)
  // PC11
  initDataMaster0.portLocationClk = _USART_ROUTELOC0_CLKLOC_LOC14;
  initDataSlave0.portLocationClk  = _USART_ROUTELOC0_CLKLOC_LOC14;
  // PC10
  initDataMaster0.portLocationCs  = _USART_ROUTELOC0_CSLOC_LOC12;
  initDataSlave0.portLocationCs   = _USART_ROUTELOC0_CSLOC_LOC12;
#endif

#if defined(EFR32MG2_BRD4157A)
  // PA6
  initDataMaster0.portLocationTx = _USART_ROUTELOC0_TXLOC_LOC1;
  initDataSlave0.portLocationTx  = _USART_ROUTELOC0_TXLOC_LOC1;

  // PA7
  initDataMaster0.portLocationRx = _USART_ROUTELOC0_RXLOC_LOC1;
  initDataSlave0.portLocationRx  = _USART_ROUTELOC0_RXLOC_LOC1;

  // PA8
  initDataMaster0.portLocationClk = _USART_ROUTELOC0_CLKLOC_LOC1;
  initDataSlave0.portLocationClk  = _USART_ROUTELOC0_CLKLOC_LOC1;

  // PA9
  initDataMaster0.portLocationCs  = _USART_ROUTELOC0_CSLOC_LOC1;
  initDataSlave0.portLocationCs   = _USART_ROUTELOC0_CSLOC_LOC1;

  // PB6
  initDataMaster1.portLocationTx = _USART_ROUTELOC0_TXLOC_LOC10;
  initDataSlave1.portLocationTx  = _USART_ROUTELOC0_TXLOC_LOC10;

  // PB7
  initDataMaster1.portLocationRx = _USART_ROUTELOC0_RXLOC_LOC10;
  initDataSlave1.portLocationRx  = _USART_ROUTELOC0_RXLOC_LOC10;

  // PD8
  initDataMaster1.portLocationClk = _USART_ROUTELOC0_CLKLOC_LOC30;
  initDataSlave1.portLocationClk  = _USART_ROUTELOC0_CLKLOC_LOC30;

  // PD9
  initDataMaster1.portLocationCs  = _USART_ROUTELOC0_CSLOC_LOC30;
  initDataSlave1.portLocationCs   = _USART_ROUTELOC0_CSLOC_LOC30;
#endif

#if defined(EFR32MG13_BRD4165A)
  // PC11 (EXP_16)
  initDataMaster0.portLocationClk = 14;
  initDataSlave0.portLocationClk  = 14;
  // PC10 (EXP_15)
  initDataMaster0.portLocationCs  = 12;
  initDataSlave0.portLocationCs   = 12;
  
  // PB11 (EXP_8)
  initDataMaster1.portLocationClk = 4;
  initDataSlave1.portLocationClk  = 4;
  // PD10 (EXP_7)
  initDataMaster1.portLocationCs  = 15;
  initDataSlave1.portLocationCs   = 15;
#endif

#if defined(SLSTK3401A_EFM32PG)
  // PC10
  initDataMaster0.portLocationClk = _USART_ROUTELOC0_CLKLOC_LOC13;
  initDataSlave0.portLocationClk  = _USART_ROUTELOC0_CLKLOC_LOC13;
  // PC11
  initDataMaster0.portLocationCs  = _USART_ROUTELOC0_CSLOC_LOC13;
  initDataSlave0.portLocationCs   = _USART_ROUTELOC0_CSLOC_LOC13;
#endif

  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Enable unity test report output via SWO (Cortex-M3/M4) or
     USART (Cortex-M0+) */
#if defined( GPIO_ROUTE_SWOPEN ) || defined( GPIO_ROUTEPEN_SWVPEN )
  SWO_Setup();
#endif

  /* Run test */
  test_SPIDRV();
  UnityPrint( "ENDSWO" );

  return 0;
}

/**************************************************************************//**
 * @brief  Main test function
 *****************************************************************************/
static void test_SPIDRV( void )
{
  UnityBeginGroup( "SPIDRV" );

  RTCDRV_Init();

  // Init()/DeInit() test
  pInitDataMaster = &initDataMaster1;
  RUN_TEST( test_driver_init, __LINE__ );

  // MTransmit() test on two different USART'S
  pInitDataMaster = &initDataMaster1;
  RUN_TEST( test_master_transmit, __LINE__ );
  pInitDataMaster = &initDataMaster0;
  RUN_TEST( test_master_transmit, __LINE__ );

  // MTransmitB() test
  RUN_TEST( test_master_transmit_blocking, __LINE__ );
  CORE_ATOMIC_SECTION(
    RUN_TEST( test_master_transmit_blocking, __LINE__ );
  )

  // TransferStatus() test
  RUN_TEST( test_transfer_status, __LINE__ );

  // MReceive() test on two different USART'S
  pInitDataMaster = &initDataMaster1;
  RUN_TEST( test_master_receive, __LINE__ );
  pInitDataMaster = &initDataMaster0;
  RUN_TEST( test_master_receive, __LINE__ );

  // 2 x master transmit -> slave receive test with role swapping
  pInitDataMaster = &initDataMaster1;
  pInitDataSlave  = &initDataSlave0;
  RUN_TEST( test_mastertx_slaverx, __LINE__ );
  pInitDataMaster = &initDataMaster0;
  pInitDataSlave  = &initDataSlave1;
  RUN_TEST( test_mastertx_slaverx, __LINE__ );

  // 2 x master receive -> slave transmit test with role swapping
  pInitDataMaster = &initDataMaster1;
  pInitDataSlave  = &initDataSlave0;
  RUN_TEST( test_masterrx_slavetx, __LINE__ );
  pInitDataMaster = &initDataMaster0;
  pInitDataSlave  = &initDataSlave1;
  RUN_TEST( test_masterrx_slavetx, __LINE__ );

  // master <-> slave transfer test
  RUN_TEST( test_master_slave_transfer, __LINE__ );

  // master <-> slave transfer test with single data buffers
  RUN_TEST( test_master_slave_transfer_singlebuffers, __LINE__ );

  // master <-> slave transfer test with single data buffers
  RUN_TEST( test_master_slave_single_transfer, __LINE__ );

  RUN_TEST( test_misc, __LINE__ );

  RUN_TEST( test_slave_timeout, __LINE__ );

  RUN_TEST( test_abort_transfer, __LINE__ );

  RUN_TEST( test_slave_delayed_start, __LINE__ );

  RTCDRV_DeInit();

  UnityEnd();
}

// -----------------------------------------------------------------------------
static void test_driver_init( void )
{

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, pInitDataMaster ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
}

// -----------------------------------------------------------------------------
static volatile bool test_master_transmit_callback_called;

static void test_master_transmit_callback( SPIDRV_Handle_t handle,
                                           Ecode_t transferStatus,
                                           int itemsTransferred )
{
  test_master_transmit_callback_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataMaster->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_OK );
  TEST_ASSERT_TRUE( itemsTransferred == sizeof( pattern1 ) );
}

static void test_master_transmit( void )
{
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, pInitDataMaster ) );

  test_master_transmit_callback_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransmit( handleOne,
                                       pattern1,
                                       sizeof( pattern1 ),
                                       test_master_transmit_callback ) );

  // Check status immediately, should not be completed
  TEST_ASSERT_FALSE( test_master_transmit_callback_called );

  // Check status after a few msecs, should be completed now !
  RTCDRV_Delay( 5 );
  TEST_ASSERT_TRUE( test_master_transmit_callback_called );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
}

// -----------------------------------------------------------------------------
static void test_master_transmit_blocking( void )
{
  int transferred, remaining;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, &initDataMaster1 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransmitB( handleOne,
                                        pattern2,
                                        sizeof( pattern2 ) ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_GetTransferStatus( handleOne,
                                               &transferred,
                                               &remaining ) );
  TEST_ASSERT_EQUAL( 0, remaining );
  TEST_ASSERT_EQUAL( sizeof( pattern2 ), transferred );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
}
// -----------------------------------------------------------------------------
static volatile bool test_master_receive_callback_called;

static void test_master_receive_callback( SPIDRV_Handle_t handle,
                                          Ecode_t transferStatus,
                                          int itemsTransferred )
{
  test_master_receive_callback_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataMaster->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_OK );
  TEST_ASSERT_TRUE( itemsTransferred == sizeof( rxBuffer1 ) );
}

static void test_master_receive( void )
{
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, pInitDataMaster ) );

  test_master_receive_callback_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MReceive( handleOne,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      test_master_receive_callback ) );

  // Check status immediately, should not be completed
  TEST_ASSERT_FALSE( test_master_receive_callback_called );

  // Check status after a few msecs, should be completed now !
  RTCDRV_Delay( 5 );
  TEST_ASSERT_TRUE( test_master_receive_callback_called );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
}

// -----------------------------------------------------------------------------
static void test_transfer_status( void )
{
  int transferred, remaining;
#if defined( _EFR32_MIGHTY_FAMILY ) || defined( _EFM32_PEARL_FAMILY )
  SPIDRV_Init_t initData = SPIDRV_MASTER_USART1;
#else
  SPIDRV_Init_t initData = SPIDRV_MASTER_USART2;
#endif

  // Go slow on SPI bus !
  initData.bitRate = 3000;

  //initData.csControl = spidrvCsControlExternal;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, &initData ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransmit( handleOne,
                                       pattern2,
                                       sizeof( pattern2 ),
                                       NULL ) );

  // Check for 0 items transferred
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_GetTransferStatus( handleOne,
                                               &transferred,
                                               &remaining ) );
  TEST_ASSERT_EQUAL( 0, transferred );
  TEST_ASSERT_EQUAL( sizeof( pattern2 ), remaining );

  RTCDRV_Delay( 7 );

  // Check for greater than 0 items transferred, but not completed
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_GetTransferStatus( handleOne,
                                               &transferred,
                                               &remaining ) );
  TEST_ASSERT_EQUAL( sizeof( pattern2 ), remaining + transferred );
  TEST_ASSERT_TRUE( transferred > 0 );
  TEST_ASSERT_TRUE( transferred < (int)sizeof( pattern2 ) );

  RTCDRV_Delay( 10 );

  // Check for completed transmission
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_GetTransferStatus( handleOne,
                                               &transferred,
                                               &remaining ) );
  TEST_ASSERT_EQUAL( 0, remaining );
  TEST_ASSERT_EQUAL( sizeof( pattern2 ), transferred );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
}
// -----------------------------------------------------------------------------
static volatile bool test_mastertx_slaverx_callback_called;

static void test_mastertx_slaverx_callback( SPIDRV_Handle_t handle,
                                            Ecode_t transferStatus,
                                            int itemsTransferred )
{
  test_mastertx_slaverx_callback_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataSlave->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_OK );
  TEST_ASSERT_TRUE( itemsTransferred == sizeof( rxBuffer1 ) );
}

static void test_mastertx_slaverx( void )
{
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, pInitDataMaster ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleTwo, pInitDataSlave ) );

  memset( rxBuffer1, 0, sizeof( rxBuffer1 ) );
  test_mastertx_slaverx_callback_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SReceive(
                                handleTwo,
                                rxBuffer1,
                                sizeof( rxBuffer1 ),
                                test_mastertx_slaverx_callback,
                                0 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransmitB( handleOne,
                                        pattern2,
                                        sizeof( pattern2 ) ) );

  // Has the callback been called ?
  TEST_ASSERT_TRUE( test_mastertx_slaverx_callback_called );
  // Do testdata compare correctly ?
  TEST_ASSERT_EQUAL( 0, memcmp( pattern2, rxBuffer1, sizeof( pattern2 ) ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleTwo ) );
}

// -----------------------------------------------------------------------------
static volatile bool test_masterrx_slavetx_callback_called;

static void test_masterrx_slavetx_callback( SPIDRV_Handle_t handle,
                                            Ecode_t transferStatus,
                                            int itemsTransferred )
{
  test_masterrx_slavetx_callback_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataSlave->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_OK );
  TEST_ASSERT_TRUE( itemsTransferred == sizeof( pattern2 ) );
}

static void test_masterrx_slavetx( void )
{
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, pInitDataMaster ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleTwo, pInitDataSlave ) );

  memset( rxBuffer1, 0, sizeof( rxBuffer1 ) );
  test_masterrx_slavetx_callback_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_STransmit(
                                handleTwo,
                                pattern2,
                                sizeof( pattern2 ),
                                test_masterrx_slavetx_callback,
                                0 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MReceiveB( handleOne,
                                       rxBuffer1,
                                       sizeof( rxBuffer1 ) ) );

  // Has the callback been called ?
  TEST_ASSERT_TRUE( test_masterrx_slavetx_callback_called );
  // Do testdata compare correctly ?
  TEST_ASSERT_EQUAL( 0, memcmp( pattern2, rxBuffer1, sizeof( pattern2 ) ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleTwo ) );
}

// -----------------------------------------------------------------------------
static volatile bool test_master_slave_transfer_callback_called;

static void test_master_slave_transfer_callback( SPIDRV_Handle_t handle,
                                                 Ecode_t transferStatus,
                                                 int itemsTransferred )
{
  test_master_slave_transfer_callback_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataSlave->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_OK );
  TEST_ASSERT_TRUE( itemsTransferred == sizeof( pattern2 ) );
}

static void test_master_slave_transfer( void )
{
  pInitDataMaster = &initDataMaster1;
  pInitDataSlave  = &initDataSlave0;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, pInitDataMaster ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleTwo, pInitDataSlave ) );

  memset( rxBuffer1, 0, sizeof( rxBuffer1 ) );
  memset( rxBuffer2, 0, sizeof( rxBuffer2 ) );
  test_master_slave_transfer_callback_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_STransfer(
                                handleTwo,
                                pattern2,
                                rxBuffer2,
                                sizeof( pattern2 ),
                                test_master_slave_transfer_callback,
                                0 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransferB(
                                handleOne,
                                pattern1,
                                rxBuffer1,
                                sizeof( pattern1 ) ) );

  // Has the callback been called ?
  TEST_ASSERT_TRUE( test_master_slave_transfer_callback_called );
  // Do testdata compare correctly ?
  TEST_ASSERT_EQUAL( 0, memcmp( pattern1, rxBuffer2, sizeof( pattern1 ) ) );
  TEST_ASSERT_EQUAL( 0, memcmp( pattern2, rxBuffer1, sizeof( pattern2 ) ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleTwo ) );
}

// -----------------------------------------------------------------------------
static void test_master_slave_transfer_singlebuffers( void )
{
  // This test verifies that we can use the same databuffer for both
  // transmit and receive when using the XTransfer(B) API functions.
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, &initDataMaster1 ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleTwo, &initDataSlave0 ) );

  memcpy( rxBuffer1, pattern1, sizeof( pattern1 ) );
  memcpy( rxBuffer2, pattern2, sizeof( pattern2 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_STransfer(
                                handleTwo,
                                rxBuffer2,
                                rxBuffer2,
                                sizeof( pattern2 ),
                                NULL,
                                0 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransferB(
                                handleOne,
                                rxBuffer1,
                                rxBuffer1,
                                sizeof( pattern1 ) ) );

  // Do testdata compare correctly ?
  TEST_ASSERT_EQUAL( 0, memcmp( pattern1, rxBuffer2, sizeof( pattern1 ) ) );
  TEST_ASSERT_EQUAL( 0, memcmp( pattern2, rxBuffer1, sizeof( pattern2 ) ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleTwo ) );
}

// -----------------------------------------------------------------------------
static void test_master_slave_single_transfer( void )
{
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, &initDataMaster1 ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleTwo, &initDataSlave0 ) );

  memset( rxBuffer1, 0, sizeof( rxBuffer1 ) );
  rxBuffer2[0] = 0x00; rxBuffer2[1] = 0xFF;
  rxBuffer2[2] = 0xFF; rxBuffer2[3] = 0xFF;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_STransfer(
                                handleTwo,
                                pattern2,
                                rxBuffer2,
                                1,
                                NULL,
                                0 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransferSingleItemB(
                                handleOne,
                                0x77,
                                rxBuffer1 ) );

  // Do testdata compare correctly ?
  TEST_ASSERT_EQUAL( rxBuffer1[0], pattern2[0] );
  TEST_ASSERT_EQUAL( rxBuffer1[1], 0           );
  TEST_ASSERT_EQUAL( rxBuffer1[2], 0           );
  TEST_ASSERT_EQUAL( rxBuffer1[3], 0           );
  TEST_ASSERT_EQUAL( rxBuffer2[0], 0x77        );
  TEST_ASSERT_EQUAL( rxBuffer2[1], 0xFF        );
  TEST_ASSERT_EQUAL( rxBuffer2[2], 0xFF        );
  TEST_ASSERT_EQUAL( rxBuffer2[3], 0xFF        );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleTwo ) );
}

// -----------------------------------------------------------------------------
static void test_misc( void )
{
  uint32_t frameLength;
  uint32_t bitRate;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, &initDataMaster1 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_GetBitrate( handleOne, &bitRate ) );
  TEST_ASSERT_INT_WITHIN( 50000, 1000000, bitRate );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_GetFramelength( handleOne, &frameLength ) );
  TEST_ASSERT_EQUAL( 8, frameLength );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransferSingleItemB(
                                handleOne,
                                0x71,
                                NULL ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SetBitrate( handleOne, 500000 ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_GetBitrate( handleOne, &bitRate ) );
  TEST_ASSERT_INT_WITHIN( 25000, 500000, bitRate );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SetFramelength( handleOne, 11 ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_GetFramelength( handleOne, &frameLength ) );
  TEST_ASSERT_EQUAL( 11, frameLength );


  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransferSingleItemB(
                                handleOne,
                                0x71,
                                NULL ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
}

// -----------------------------------------------------------------------------
static volatile bool test_slave_timeout_callback1_called;
static volatile bool test_slave_timeout_callback2_called;
static volatile bool test_slave_timeout_callback3_called;

static void test_slave_timeout_callback1( SPIDRV_Handle_t handle,
                                          Ecode_t transferStatus,
                                          int itemsTransferred )
{
  test_slave_timeout_callback1_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataSlave->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_TIMEOUT );
  TEST_ASSERT_TRUE( itemsTransferred == 0 );
}

static void test_slave_timeout_callback2( SPIDRV_Handle_t handle,
                                          Ecode_t transferStatus,
                                          int itemsTransferred )
{
  test_slave_timeout_callback2_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataSlave->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_OK );
  TEST_ASSERT_TRUE( itemsTransferred == sizeof( rxBuffer1 ) );
}

static void test_slave_timeout_callback3( SPIDRV_Handle_t handle,
                                          Ecode_t transferStatus,
                                          int itemsTransferred )
{
  test_slave_timeout_callback3_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataSlave->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_TIMEOUT );
  TEST_ASSERT_TRUE( itemsTransferred == 2 );
}

static void test_slave_timeout( void )
{
  pInitDataMaster = &initDataMaster1;
  pInitDataSlave  = &initDataSlave0;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, pInitDataMaster ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleTwo, pInitDataSlave ) );

  //------------------------------------------
  test_slave_timeout_callback1_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SReceive( handleTwo,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      test_slave_timeout_callback1,
                                      10 ) );

  RTCDRV_Delay( 12 );
  // Has the callback been called ?
  TEST_ASSERT_TRUE( test_slave_timeout_callback1_called );
  //------------------------------------------
  test_slave_timeout_callback1_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SReceive( handleTwo,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      test_slave_timeout_callback1,
                                      10 ) );

  RTCDRV_Delay( 5 );
  // Callback should not have been called
  TEST_ASSERT_FALSE( test_slave_timeout_callback1_called );
  // Callback should have been called
  RTCDRV_Delay( 7 );
  TEST_ASSERT_TRUE( test_slave_timeout_callback1_called );
  //------------------------------------------
  test_slave_timeout_callback2_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SReceive( handleTwo,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      test_slave_timeout_callback2,
                                      10 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransmitB( handleOne,
                                        pattern2,
                                        sizeof( pattern2 ) ) );

  // Callback should have been called
  TEST_ASSERT_TRUE( test_slave_timeout_callback2_called );
  //------------------------------------------
  test_slave_timeout_callback3_called = false;

  // Go slow, it now takes approx 13 ms to tranmit pattern2.
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SetBitrate( handleOne, 3000 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SReceive( handleTwo,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      test_slave_timeout_callback3,
                                      6 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransmit( handleOne,
                                       pattern2,
                                       sizeof( pattern2 ),
                                       NULL ) );
  RTCDRV_Delay( 7 );
  // Callback should have been called
  TEST_ASSERT_TRUE( test_slave_timeout_callback3_called );
  RTCDRV_Delay( 7 );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SetBitrate( handleOne, 1000000 ) );
  //------------------------------------------
  test_slave_timeout_callback2_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SReceive( handleTwo,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      test_slave_timeout_callback2,
                                      0 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransmit( handleOne,
                                       pattern2,
                                       sizeof( pattern2 ),
                                       NULL ) );

  // Callback should not have been called
  TEST_ASSERT_FALSE( test_slave_timeout_callback2_called );
  RTCDRV_Delay( 5 );
  //------------------------------------------
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_TIMEOUT,
                     SPIDRV_SReceiveB( handleTwo,
                                       rxBuffer1,
                                       sizeof( rxBuffer1 ),
                                       5 ) );
  //------------------------------------------

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleTwo ) );
}

// -----------------------------------------------------------------------------
static volatile bool test_abort_transfer_callback1_called;
static volatile bool test_abort_transfer_callback2_called;

static void test_abort_transfer_callback1( SPIDRV_Handle_t handle,
                                           Ecode_t transferStatus,
                                           int itemsTransferred )
{
  test_abort_transfer_callback1_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataSlave->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_ABORTED );
  TEST_ASSERT_TRUE( itemsTransferred == 0 );
}

static void test_abort_transfer_callback2( SPIDRV_Handle_t handle,
                                           Ecode_t transferStatus,
                                           int itemsTransferred )
{
  test_abort_transfer_callback2_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataSlave->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_ABORTED );
  TEST_ASSERT_TRUE( itemsTransferred == 2 );
}

static void test_abort_transfer( void )
{
  int transferred, remaining;

  pInitDataMaster = &initDataMaster1;
  pInitDataSlave  = &initDataSlave0;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, pInitDataMaster ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleTwo, pInitDataSlave ) );

  //------------------------------------------
  test_abort_transfer_callback1_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SReceive( handleTwo,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      test_abort_transfer_callback1,
                                      0 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_AbortTransfer( handleTwo ) );

  // Callback should have been called
  TEST_ASSERT_TRUE( test_abort_transfer_callback1_called );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_GetTransferStatus( handleTwo,
                                               &transferred,
                                               &remaining ) );
  TEST_ASSERT_EQUAL( sizeof( rxBuffer1 ), remaining );
  TEST_ASSERT_EQUAL( 0, transferred );
  //------------------------------------------
  test_abort_transfer_callback2_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SReceive( handleTwo,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      test_abort_transfer_callback2,
                                      0 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransferSingleItemB(
                                      handleOne,
                                      0x77,
                                      NULL ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_MTransferSingleItemB(
                                      handleOne,
                                      0x33,
                                      NULL ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_AbortTransfer( handleTwo ) );

  // Callback should have been called
  TEST_ASSERT_TRUE( test_abort_transfer_callback2_called );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_GetTransferStatus( handleTwo,
                                               &transferred,
                                               &remaining ) );
  TEST_ASSERT_EQUAL( 3, remaining );
  TEST_ASSERT_EQUAL( 2, transferred );
  //------------------------------------------

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleTwo ) );
}

// -----------------------------------------------------------------------------
static volatile bool test_slave_delayed_start_callback_called;

static void test_slave_delayed_start_callback( SPIDRV_Handle_t handle,
                                               Ecode_t transferStatus,
                                               int itemsTransferred )
{
  test_slave_delayed_start_callback_called = true;
  TEST_ASSERT_TRUE( handle->initData.port == pInitDataSlave->port );
  TEST_ASSERT_TRUE( transferStatus == ECODE_EMDRV_SPIDRV_TIMEOUT );
  TEST_ASSERT_TRUE( itemsTransferred == 0 );
}

static void TimerTimeout( RTCDRV_TimerID_t id, void *user )
{
  (void)id;
  (void)user;

  // Force CS hi
  GPIO_PinOutSet( (GPIO_Port_TypeDef)handleOne->csPort, handleOne->csPin );
}

static void test_slave_delayed_start( void )
{
  RTCDRV_TimerID_t timer;
  initDataSlave0.slaveStartMode = spidrvSlaveStartDelayed;
  pInitDataMaster = &initDataMaster1;
  pInitDataSlave  = &initDataSlave0;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleOne, pInitDataMaster ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_Init( handleTwo, pInitDataSlave ) );

  //------------------------------------------
  test_slave_delayed_start_callback_called = false;

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SReceive( handleTwo,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      test_slave_delayed_start_callback,
                                      10 ) );
  RTCDRV_Delay( 15 );

  // Callback should have been called
  TEST_ASSERT_TRUE( test_slave_delayed_start_callback_called );

  //------------------------------------------
  // Force CS low
  #if defined( USART_ROUTEPEN_CSPEN )
    handleOne->initData.port->ROUTEPEN &= ~USART_ROUTEPEN_CSPEN;
  #else
    handleOne->initData.port->ROUTE &= ~USART_ROUTE_CSPEN;
  #endif
  GPIO_PinOutClear( (GPIO_Port_TypeDef)handleOne->csPort, handleOne->csPin );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_TIMEOUT,
                     SPIDRV_SReceive( handleTwo,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      NULL,
                                      10 ) );
  //------------------------------------------
  // Combine the following test with scope and flip GPIO (PortC,pin0)
  // in the WaitForIdleLine() function within the SPI driver

  TEST_ASSERT_EQUAL( ECODE_EMDRV_RTCDRV_OK,
                     RTCDRV_AllocateTimer( &timer ) );

  // Arm a timer to fire TimerTimeout() 5 msec's from now
  // TimerTimeout() will deassert the CS
  TEST_ASSERT_EQUAL( ECODE_EMDRV_RTCDRV_OK,
                     RTCDRV_StartTimer( timer,
                                        rtcdrvTimerTypeOneshot,
                                        5,
                                        TimerTimeout,
                                        NULL ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_SReceive( handleTwo,
                                      rxBuffer1,
                                      sizeof( rxBuffer1 ),
                                      NULL,
                                      0 ) );

  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleOne ) );
  TEST_ASSERT_EQUAL( ECODE_EMDRV_SPIDRV_OK,
                     SPIDRV_DeInit( handleTwo ) );

  // Reset initdata to default
  initDataSlave0.slaveStartMode = spidrvSlaveStartImmediate;
}
