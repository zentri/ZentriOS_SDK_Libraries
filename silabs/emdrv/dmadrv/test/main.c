/**************************************************************************//**
 * @file main.c
 * @brief DMADRV test program
 * @version INTERNAL
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014-2015 Silicon Labs, http://www.silabs.com</b>
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
#include "em_cmu.h"
#include "em_emu.h"
#include "em_usart.h"

#include "unity.h"
#if defined(UNITY_TEST_REPORT_SERIAL)
#include "unity_usart.h"
#else
#include "unity_swo.h"
#endif

#include "dmadrv.h"

#if defined(_SILICON_LABS_32B_SERIES_0)   \
    && (defined(_EZR32_LEOPARD_FAMILY)    \
        || defined(_EZR32_WONDER_FAMILY)  \
        || defined(_EFM32_GECKO_FAMILY)   \
        || defined(_EFM32_WONDER_FAMILY)  \
        || defined(_EFM32_ZERO_FAMILY)    \
        || (defined(_EFM32_GIANT_FAMILY) && (FLASH_PAGE_SIZE==2048))) /* Leopard STK */
#define UART_DEVICE            (USART1)
#define UART_CLOCK             (cmuClock_USART1)
#define DMA_REQUEST_SIGNAL_TX  (dmadrvPeripheralSignal_USART1_TXBL)
#define DMA_REQUEST_SIGNAL_RX  (dmadrvPeripheralSignal_USART1_RXDATAV)
#elif defined(_SILICON_LABS_32B_SERIES_1) \
      || defined(_EFM32_HAPPY_FAMILY)     \
      || defined(_EFM32_TINY_FAMILY)
#define UART_DEVICE            (USART0)
#define UART_CLOCK             (cmuClock_USART0)
#define DMA_REQUEST_SIGNAL_TX  (dmadrvPeripheralSignal_USART0_TXBL)
#define DMA_REQUEST_SIGNAL_RX  (dmadrvPeripheralSignal_USART0_RXDATAV)
#elif defined(_SILICON_LABS_32B_SERIES_0) && defined ( _EFM32_GIANT_FAMILY )
#define UART_DEVICE            (UART1)
#define UART_CLOCK             (cmuClock_UART1)
#define DMA_REQUEST_SIGNAL_TX  (dmadrvPeripheralSignal_UART1_TXBL)
#define DMA_REQUEST_SIGNAL_RX  (dmadrvPeripheralSignal_UART1_RXDATAV)
#else
#error Device not supported.
#endif

// Override DMA_CH_COUNT if on device with few DMA channels
#if !defined( EMDRV_DMADRV_DMA_CH_COUNT ) \
    || ( EMDRV_DMADRV_DMA_CH_COUNT > DMA_CHAN_COUNT )
#undef EMDRV_DMADRV_DMA_CH_COUNT
#define EMDRV_DMADRV_DMA_CH_COUNT DMA_CHAN_COUNT
#endif

static void test_DMADRV( void );
static void test_init( void );
static void test_alloc( void );
#if !defined( EMDRV_DMADRV_USE_NATIVE_API )
static void test_memory_to_peripheral( void );
static void test_memory_to_peripheral_noint( void );
#ifndef EMDRV_REGRESSION_TEST
static void test_peripheral_to_memory( void );
static void test_peripheral_to_memory_pp( void );
#endif
static void test_memory_to_peripheral_pp( void );
#else
static void test_native_api_memory_to_peripheral( void );
#endif

static void setup_USART(void);

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main( void )
{
  /* Chip revision alignment and errata fixes */
  CHIP_Init();

#if defined( _EMU_DCDCCTRL_MASK )
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
#endif

  setup_USART();
  
#if defined( UNITY_TEST_REPORT_SERIAL )
  UNITY_UsartSetup();
#else
  SWO_Setup();
#endif

  /* Run test */
  test_DMADRV();
  UnityPrint( "ENDSWO" );

  return 0;
}

static void setup_USART(void) {
  /* Enable peripheral clocks */
  CMU_ClockEnable(cmuClock_HFPER, true);
  /* Configure GPIO pins */
  CMU_ClockEnable(cmuClock_GPIO, true);

  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

  CMU_ClockEnable(UART_CLOCK, true);

  /* Configure USART for basic async operation */
  USART_InitAsync(UART_DEVICE, &init);
}

/**************************************************************************//**
 * @brief  Main test function
 *****************************************************************************/
static void test_DMADRV( void )
{
  UnityBeginGroup( "DMADRV" );

  // Init/DeInit test
  RUN_TEST( test_init, __LINE__ );

  // Alloc/Free test
  RUN_TEST( test_alloc, __LINE__ );

#if !defined( EMDRV_DMADRV_USE_NATIVE_API )

  // Simple USART transfer tests
  RUN_TEST( test_memory_to_peripheral, __LINE__ );
  RUN_TEST( test_memory_to_peripheral_noint, __LINE__ );
#ifndef EMDRV_REGRESSION_TEST
  RUN_TEST( test_peripheral_to_memory, __LINE__ );
#endif

  // Ping-Pong USART transfer tests
#ifndef EMDRV_REGRESSION_TEST
  RUN_TEST( test_peripheral_to_memory_pp, __LINE__ );
#endif
  RUN_TEST( test_memory_to_peripheral_pp, __LINE__ );

#else
  RUN_TEST( test_native_api_memory_to_peripheral, __LINE__ );
#endif

  UnityEnd();
}

// -----------------------------------------------------------------------------
static void test_init( void )
{
  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_Init() );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_DeInit() );
}

// -----------------------------------------------------------------------------
static void test_alloc( void )
{
  int i;
  unsigned int channel;

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_Init() );

  for ( i=0; i < EMDRV_DMADRV_DMA_CH_COUNT; i++ )
  {
    TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                       DMADRV_AllocateChannel( &channel, NULL ) );
  }
  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_CHANNELS_EXHAUSTED,
                     DMADRV_AllocateChannel( &channel, NULL ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_IN_USE,
                     DMADRV_DeInit() );

  for ( i=0; i < EMDRV_DMADRV_DMA_CH_COUNT-1; i++ )
  {
    TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                       DMADRV_FreeChannel( i ) );
  }

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_IN_USE,
                     DMADRV_DeInit() );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_FreeChannel( EMDRV_DMADRV_DMA_CH_COUNT-1 ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_DeInit() );
}

#if !defined( EMDRV_DMADRV_USE_NATIVE_API )
// -----------------------------------------------------------------------------
static volatile bool memory_to_peripheral_callback_called;
static unsigned int  memory_to_peripheral_channel;

static bool memory_to_peripheral_callback( unsigned int channel,
                                          unsigned int sequenceNo,
                                          void *userParam )
{
  TEST_ASSERT_EQUAL( channel, memory_to_peripheral_channel );
  TEST_ASSERT_EQUAL( sequenceNo, 1 );
  TEST_ASSERT_EQUAL( userParam, 17 );
  memory_to_peripheral_callback_called = true;

  return true;
}

static void test_memory_to_peripheral( void )
{
  char str[] = "Hello DMA !\n\r";
  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_Init() );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_AllocateChannel( &memory_to_peripheral_channel,
                                             NULL ) );

  memory_to_peripheral_callback_called = false;

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_MemoryPeripheral(
                         memory_to_peripheral_channel,
                         DMA_REQUEST_SIGNAL_TX,
                         (void*)&(UART_DEVICE->TXDATA), // *dst
                         str,                           // *src
                         true,                          // srcInc
                         strlen( str ),                 // length
                         dmadrvDataSize1,               // byte transfers
                         memory_to_peripheral_callback,  // callback func
                         (void*)17 ) );                 // *cbUserParam

  while ( memory_to_peripheral_callback_called == false )
  {
    // Should use a timer here to avoid that test hangs on error !!
  }

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_FreeChannel( memory_to_peripheral_channel ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_DeInit() );
}

// -----------------------------------------------------------------------------
static void test_memory_to_peripheral_noint( void )
{
  bool completed;
  char str[] = "Hello again DMA !\n\r";

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_Init() );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_AllocateChannel( &memory_to_peripheral_channel,
                                             NULL ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_MemoryPeripheral(
                         memory_to_peripheral_channel,
                         DMA_REQUEST_SIGNAL_TX,
                         (void*)&(UART_DEVICE->TXDATA),      // *dst
                         str,                           // *src
                         true,                          // srcInc
                         strlen( str ),                 // length
                         dmadrvDataSize1,               // byte transfers
                         NULL,                          // callback func
                         NULL ) );                      // *cbUserParam

  do
  {
    TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                       DMADRV_TransferDone( memory_to_peripheral_channel,
                                            &completed ) );
  } while ( !completed );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_FreeChannel( memory_to_peripheral_channel ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_DeInit() );
}
// -----------------------------------------------------------------------------
#ifndef EMDRV_REGRESSION_TEST
static volatile bool peripheral_to_memory_callback_called;
static unsigned int  peripheral_to_memory_channel;

static bool peripheral_to_memory_callback( unsigned int channel,
                                          unsigned int sequenceNo,
                                          void *userParam )
{
  TEST_ASSERT_EQUAL( channel, peripheral_to_memory_channel );
  TEST_ASSERT_EQUAL( sequenceNo, 1 );
  TEST_ASSERT_EQUAL( userParam, 19 );
  peripheral_to_memory_callback_called = true;

  return true;
}
#endif

#ifndef EMDRV_REGRESSION_TEST
static void test_peripheral_to_memory( void )
{
  bool completed;
  char strRx[5];
  char strTx[] = "Hit keyboard 5 times to complete test: ";

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_Init() );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_AllocateChannel( &peripheral_to_memory_channel,
                                             NULL ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_MemoryPeripheral(
                         peripheral_to_memory_channel,
                         DMA_REQUEST_SIGNAL_TX,
                         (void*)&(UART_DEVICE->TXDATA), // *dst
                         strTx,                         // *src
                         true,                          // srcInc
                         strlen( strTx ),               // length
                         dmadrvDataSize1,               // byte transfers
                         NULL,                          // callback func
                         NULL ) );                      // *cbUserParam

  do
  {
    TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                       DMADRV_TransferDone( peripheral_to_memory_channel,
                                            &completed ) );
  } while ( !completed );

  peripheral_to_memory_callback_called = false;

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_PeripheralMemory(
                         peripheral_to_memory_channel,
                         DMA_REQUEST_SIGNAL_RX,
                         strRx,
                         (void*)&(UART_DEVICE->RXDATA), // *dst
                         true,                          // dstInc
                         sizeof( strRx ),               // length
                         dmadrvDataSize1,               // byte transfers
                         peripheral_to_memory_callback,  // callback func
                         (void*)19 ) );                 // *cbUserParam

  while ( peripheral_to_memory_callback_called == false )
  {
    // Should use a timer here to avoid that test hangs on error !!
  }

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_MemoryPeripheral(
                         peripheral_to_memory_channel,
                         DMA_REQUEST_SIGNAL_TX,
                         (void*)&(UART_DEVICE->TXDATA), // *dst
                         strRx,                         // *src
                         true,                          // srcInc
                         sizeof( strRx ),               // length
                         dmadrvDataSize1,               // byte transfers
                         NULL,                          // callback func
                         NULL ) );                      // *cbUserParam

  do
  {
    TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                       DMADRV_TransferDone( peripheral_to_memory_channel,
                                            &completed ) );
  } while ( !completed );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_FreeChannel( peripheral_to_memory_channel ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_DeInit() );
}
#endif

// -----------------------------------------------------------------------------
#ifndef EMDRV_REGRESSION_TEST
static unsigned int  peripheral_to_memory_pp_channel;
static volatile bool peripheral_to_memory_pp_callback_called;

static bool peripheral_to_memory_pp_callback( unsigned int channel,
                                             unsigned int sequenceNo,
                                             void *userParam )
{
  TEST_ASSERT_EQUAL( channel, peripheral_to_memory_pp_channel );
  TEST_ASSERT_EQUAL( userParam, 27 );

  if ( sequenceNo == 4 )
    peripheral_to_memory_pp_callback_called = true;

  if ( sequenceNo >= 2 )
    return false;

  return true;
}

char strRx0[5];
char strRx1[5];

static void test_peripheral_to_memory_pp( void )
{
  bool completed;
  char strTx[] = "Hit keyboard 20 times to complete ping-pong test !";

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_Init() );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_AllocateChannel( &peripheral_to_memory_pp_channel,
                                             NULL ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_MemoryPeripheral(
                         peripheral_to_memory_pp_channel,
                         DMA_REQUEST_SIGNAL_TX,
                         (void*)&(UART_DEVICE->TXDATA), // *dst
                         strTx,                         // *src
                         true,                          // srcInc
                         strlen( strTx ),               // length
                         dmadrvDataSize1,               // byte transfers
                         NULL,                          // callback func
                         NULL ) );                      // *cbUserParam

  do
  {
    TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                       DMADRV_TransferDone( peripheral_to_memory_pp_channel,
                                            &completed ) );
  } while ( !completed );

  peripheral_to_memory_pp_callback_called = false;

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_PeripheralMemoryPingPong(
                         peripheral_to_memory_pp_channel,
                         DMA_REQUEST_SIGNAL_RX,
                         strRx0,                         // *dst0
                         strRx1,                         // *dst1
                         (void*)&(UART_DEVICE->RXDATA),  // *src
                         true,                           // dstInc
                         sizeof( strRx0 ),               // length
                         dmadrvDataSize1,                // byte transfers
                         peripheral_to_memory_pp_callback,// callback func
                         (void*)27 ) );                  // *cbUserParam

  while ( peripheral_to_memory_pp_callback_called == false )
  {
    // Should use a timer here to avoid that test hangs on error !!
  }

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_FreeChannel( peripheral_to_memory_pp_channel ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_DeInit() );
}
#endif
// -----------------------------------------------------------------------------
static unsigned int  memory_to_peripheral_pp_channel;
static volatile bool memory_to_peripheral_pp_callback_called;

static bool memory_to_peripheral_pp_callback( unsigned int channel,
                                             unsigned int sequenceNo,
                                             void *userParam )
{
  TEST_ASSERT_EQUAL( channel, memory_to_peripheral_pp_channel );
  TEST_ASSERT_EQUAL( userParam, 35 );

  if ( sequenceNo == 4 )
    memory_to_peripheral_pp_callback_called = true;

  if ( sequenceNo >= 2 )
    return false;

  return true;
}

static void test_memory_to_peripheral_pp( void )
{
  char strTx0[] = "String 1\r\n";
  char strTx1[] = "String 2\r\n";

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_Init() );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_AllocateChannel( &memory_to_peripheral_pp_channel,
                                             NULL ) );

  memory_to_peripheral_pp_callback_called = false;

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_MemoryPeripheralPingPong(
                         memory_to_peripheral_pp_channel,
                         DMA_REQUEST_SIGNAL_TX,
                         (void*)&(UART_DEVICE->TXDATA),  // *dst
                         strTx0,                         // *src0
                         strTx1,                         // *src1
                         true,                           // srcInc
                         sizeof( strTx0 ),               // length
                         dmadrvDataSize1,                // byte transfers
                         memory_to_peripheral_pp_callback,// callback func
                         (void*)35 ) );                  // *cbUserParam

  while ( memory_to_peripheral_pp_callback_called == false )
  {
    // Should use a timer here to avoid that test hangs on error !!
  }

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_FreeChannel( memory_to_peripheral_pp_channel ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_DeInit() );
}
#else
// -----------------------------------------------------------------------------
static volatile bool native_memory_to_peripheral_callback_called;
static unsigned int  native_memory_to_peripheral_channel;

#if defined( EMDRV_DMADRV_UDMA )
static void native_memory_to_peripheral_callback( unsigned int channel,
                                                 bool primary,
                                                 void *user )
{
  TEST_ASSERT_EQUAL( channel, native_memory_to_peripheral_channel );
  TEST_ASSERT_EQUAL( primary, true );
  TEST_ASSERT_EQUAL( user, 17 );
  native_memory_to_peripheral_callback_called = true;
}
#endif

#if defined( EMDRV_DMADRV_LDMA )
static bool native_memory_to_peripheral_callback( unsigned int channel,
                                                 unsigned int sequenceNo,
                                                 void *userParam )
{
  TEST_ASSERT_EQUAL( channel, native_memory_to_peripheral_channel );
  TEST_ASSERT_EQUAL( sequenceNo, 1 );
  TEST_ASSERT_EQUAL( userParam, 17 );
  native_memory_to_peripheral_callback_called = true;
  return true;
}
#endif

static void test_native_api_memory_to_peripheral( void )
{
#if defined( EMDRV_DMADRV_UDMA )
  DMA_CB_TypeDef dmaCb;
  DMA_CfgChannel_TypeDef dmaChannelCfg;
  DMA_CfgDescr_TypeDef cfgDesc;
#endif
  char str[] = "Hello native DMA !\n\r";

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_Init() );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_AllocateChannel( &native_memory_to_peripheral_channel,
                                             NULL ) );

  native_memory_to_peripheral_callback_called = false;

#if defined( EMDRV_DMADRV_UDMA )
  // Initialize DMA channel.
  dmaCb.cbFunc  = native_memory_to_peripheral_callback;
  dmaCb.userPtr = (void*)17;

  dmaChannelCfg.highPri   = false;
  dmaChannelCfg.enableInt = true;
  dmaChannelCfg.select    = DMA_REQUEST_SIGNAL_TX;
  dmaChannelCfg.cb        = &dmaCb;
  DMA_CfgChannel( native_memory_to_peripheral_channel, &dmaChannelCfg );

  cfgDesc.size    = dmaDataSize1;
  cfgDesc.arbRate = dmaArbitrate1;
  cfgDesc.hprot   = 0;
  cfgDesc.srcInc  = dmaDataInc1;
  cfgDesc.dstInc  = dmaDataIncNone;
  DMA_CfgDescr( native_memory_to_peripheral_channel, true, &cfgDesc );

  DMA_ActivateBasic( native_memory_to_peripheral_channel,
                     true,
                     false,
                     (void*)&(UART_DEVICE->TXDATA),
                     str,
                     strlen(str) - 1 );
#endif

#if defined( EMDRV_DMADRV_LDMA )
  LDMA_Descriptor_t desc =
      LDMA_DESCRIPTOR_SINGLE_M2P_BYTE( str, &UART_DEVICE->TXDATA, sizeof( str ) );
  LDMA_TransferCfg_t xferCfg =
      LDMA_TRANSFER_CFG_PERIPHERAL( DMA_REQUEST_SIGNAL_TX );

  TEST_ASSERT_EQUAL_HEX(  ECODE_EMDRV_DMADRV_OK,
                      DMADRV_LdmaStartTransfer(
                          native_memory_to_peripheral_channel,
                          &xferCfg,
                          &desc,
                          native_memory_to_peripheral_callback,
                          (void*)17 ) );
#endif

  while ( native_memory_to_peripheral_callback_called == false )
  {
    // Should use a timer here to avoid that test hangs on error !!
  }

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_FreeChannel( native_memory_to_peripheral_channel ) );

  TEST_ASSERT_EQUAL_HEX( ECODE_EMDRV_DMADRV_OK,
                     DMADRV_DeInit() );
}
#endif
