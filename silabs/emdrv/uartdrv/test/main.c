/**************************************************************************//**
 * @file main.c
 * @brief UARTDRV test for EFM32/EFR32
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

#include "unity.h"
#if !defined(UNITY_TEST_REPORT_SERIAL)
#include "unity_swo.h"
#else
#include "unity_usart.h"
#endif
#include "rtcdriver.h"

#include "uartdrv.h"

/* Define receive/transmit operation queues */
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, rxBufferQueueI0);
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, txBufferQueueI0);
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, rxBufferQueueI1);
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, txBufferQueueI1);

#if defined(UARTDRV_TEST_USART)
#include "config_usart.h"
#define UARTDRV_TEST_NAME       "UARTDRV_USART"
#define UARTDRV_INIT_FUNCTION0  UARTDRV_Init
#define UARTDRV_INIT_FUNCTION1  UARTDRV_Init
#define UARTDRV_INIT_STRUCT0    UARTDRV_Init_t
#define UARTDRV_INIT_STRUCT1    UARTDRV_Init_t
#elif defined(UARTDRV_TEST_LEUART)
#include "config_leuart.h"
#if defined(LEUART_USE_LFXO)
#define UARTDRV_TEST_NAME       "UARTDRV_LEUART_LFXO"
#else
#define UARTDRV_TEST_NAME       "UARTDRV_LEUART"
#endif
#define UARTDRV_INIT_FUNCTION0  UARTDRV_InitLeuart
#define UARTDRV_INIT_FUNCTION1  UARTDRV_InitLeuart
#define UARTDRV_INIT_STRUCT0    UARTDRV_InitLeuart_t
#define UARTDRV_INIT_STRUCT1    UARTDRV_InitLeuart_t
#elif defined(UARTDRV_TEST_MIXED)
#include "config_mixed.h"
#define UARTDRV_TEST_NAME       "UARTDRV_MIXED"
#define UARTDRV_INIT_FUNCTION0  UARTDRV_InitLeuart
#define UARTDRV_INIT_FUNCTION1  UARTDRV_Init
#define UARTDRV_INIT_STRUCT0    UARTDRV_InitLeuart_t
#define UARTDRV_INIT_STRUCT1    UARTDRV_Init_t
#else
#error "No test specification provided - define either UARTDRV_TEST_USART or UARTDRV_TEST_LEUART in test json"
#endif

static UARTDRV_HandleData_t uartHandle0;
static UARTDRV_Handle_t  testHandle0 = &uartHandle0;
#if defined( TEST_UART_I1 )
static UARTDRV_HandleData_t uartHandle1;
static UARTDRV_Handle_t  testHandle1 = &uartHandle1;
static char *char0 = "G12345";
static uint8_t rxBuffer0[10];
static uint8_t rxBuffer1[10];
#if defined(TEST_UART_I1)
#define RX_BUFFER_SIZE 256
#define TX_BUFFER_SIZE 256
static uint8_t rxBuffer[RX_BUFFER_SIZE];
static uint8_t txBuffer[TX_BUFFER_SIZE];
#endif
static volatile uint32_t transmit_callback_count1 = 0;
static volatile uint32_t receive_callback_count0 = 0;
static volatile uint32_t receive_callback_count1 = 0;
static volatile uint32_t receive_abort_count0 = 0;
volatile uint32_t txOperationId = 0;
volatile uint32_t txCount = 0;
volatile bool txDone = false;
volatile uint32_t rxCount = 0;
#endif
static volatile uint32_t transmit_callback_count0 = 0;
static volatile uint32_t transmit_abort_count0 = 0;
static uint8_t pattern0[] = {'A', 'B', 'C', 'D', 'E', 'F', '1', '2', '3'};


static void test_UARTDRV(void);
static void test_driver_init(void);
static void test_UARTDRV_GetPeripheralStatus(void);
static void test_force_transmit_fc_none(void);
static void test_nb_transmit_fc_none(void);
#if defined( TEST_UART_I1 )
static void test_nb_receive_force_transmit_fc_hw(void);
static void test_nb_receive_b_transmit_fc_hw(void);
static void test_nb_receive_nb_transmit_fc_hw(void);
static void test_nb_receive_multiple_transmit_fc_hw(void);
static void test_nb_receive_force_transmit_fc_sw(void);
static void test_large_transfer_paused_by_sw(void);
#if defined(UARTDRV_TEST_USART) && defined(_USART_ROUTELOC0_MASK)
static void test_transfer_paused_by_cts(void);
static void test_large_transfer_paused_by_cts(void);
#endif
static void test_large_transfer_paused_by_cts_sw(void);
#endif
static void test_UARTDRV_Abort(void);
#if defined(UARTDRV_TEST_USART)
static void test_doc_example(void);
#endif

static void transmit_callback0(UARTDRV_Handle_t handle,
                             Ecode_t transferStatus,
                             uint8_t *data,
                             UARTDRV_Count_t transferCount)
{
  (void)data;
  (void)handle;
  (void)transferCount;

  if (transferStatus == ECODE_EMDRV_UARTDRV_OK)
  {
    transmit_callback_count0++;
  }
  else if (transferStatus == ECODE_EMDRV_UARTDRV_ABORTED)
  {
    transmit_abort_count0++;
  }
}

#if defined( TEST_UART_I1 )
static void transmit_callback1(UARTDRV_Handle_t handle,
                             Ecode_t transferStatus,
                             uint8_t *data,
                             UARTDRV_Count_t transferCount)
{
  (void)data;
  (void)handle;
  (void)transferCount;

  if (transferStatus == ECODE_EMDRV_UARTDRV_OK)
  {
    transmit_callback_count1++;
  }
}

static void receive_callback0(UARTDRV_Handle_t handle,
                             Ecode_t transferStatus,
                             uint8_t *data,
                             UARTDRV_Count_t transferCount)
{
  (void)data;
  (void)handle;
  (void)transferCount;

  if (transferStatus == ECODE_EMDRV_UARTDRV_OK)
  {
    receive_callback_count0++;
  }
  else if (transferStatus == ECODE_EMDRV_UARTDRV_ABORTED)
  {
    receive_abort_count0++;
  }
}

static void receive_callback1(UARTDRV_Handle_t handle,
                             Ecode_t transferStatus,
                             uint8_t *data,
                             UARTDRV_Count_t transferCount)
{
  (void)data;
  (void)handle;
  (void)transferCount;

  if (transferStatus == ECODE_EMDRV_UARTDRV_OK)
  {
    receive_callback_count1++;
  }
}
#endif

#if defined(UNITY_TEST_REPORT_SERIAL)
/**
 * Unity hook that's executed before each test run.
 */
void setUp(void)
{
  UnityPrintf("Test Garbage Start\n");
}

/**
 * Unity hook that's executed after each test run.
 *
 * This hook is created in order to enable test output on the same peripheral
 * which is used by uartdrv. This is the case when testing EZR32HG for instance.
 * Most of the uartdrv tests ends up disabling the UART peripheral, and
 * because of this we have to re-enable the UART before Unity tries to output
 * some test results.
 */
void tearDown(void)
{
  UNITY_UsartSetup();
  UnityPrintf("\nTest Garbage End\n");
}
#endif

/**************************************************************************//**
 * @brief  Main function
 *
 * @note   This test relies on the following connections on the DUT:
 *
 *   -- EFM32GG-DK3750 USART --
 *   PE7 (US0_TX  #1) -> PD1  (US1_RX  #1)
 *   PE6 (US0_RX  #1) <- PD0  (US1_TX  #1)
 *   PE4 (GPIO CTS  ) <- PD2  (GPIO RTS  )
 *   PE5 (GPIO RTS  ) -> PD3  (GPIO CTS  )
 *
 *   -- EFM32GG-DK3750 LEUART --
 *   PC6 (LEU1_TX #0) -> PD5  (LEU0_RX #0)
 *   PC7 (LEU1_RX #0) <- PD4  (LEU0_TX #0)
 *   PC8 (GPIO CTS  ) <- PD7  (GPIO RTS  )
 *   PC9 (GPIO RTS  ) -> PD6  (GPIO CTS  )
 *
 *   -- EFM32GG-DK3750 MIXED --
 *   PA5 (LEU1_TX #1) -> PB4  (US2_RX  #1)
 *   PA6 (LEU1_RX #1) <- PB3  (US2_TX  #1)
 *   PA4 (GPIO CTS  ) <- PB5  (GPIO RTS  )
 *   PA7 (GPIO RTS  ) -> PB2  (GPIO CTS  )
 *
 *   -- EFR32MG1 and EFR32MG2_BRD4156A WSTK / EFM32PG STK (USART/LEUART/MIXED) --
 *   PA1  (US0_TX  #1 )  <->  PC7 (US1_RX  #11)  EXP_HEADER 14 <->  6
 *   PA0  (US0_RX  #31)  <->  PC6 (US1_TX  #11)  EXP_HEADER 12 <->  4
 *   PC11 (US0_CTS #12)  <->  PC8 (US1_RTS #8)   EXP_HEADER 16 <->  8
 *   PC10 (US0_RTS #10)  <->  PC9 (US1_CTS #10)  EXP_HEADER 15 <-> 10
 *
 *   -- EFR32MG2_BRD4157A WSTK (USART/LEUART/MIXED) --
 *   PA6 (US2_TX  #1)  <->  PB6 (US3_RX  #9)  EXP_HEADER 4  <-> 14
 *   PA7 (US2_RX  #1)  <->  PB8 (US3_TX  #12) EXP_HEADER 6  <-> 12
 *   PA8 (US2_CTS #1)  <->  PD8 (US3_RTS #27) EXP_HEADER 8  <-> 5
 *   PA9 (US2_RTS #1)  <->  PD9 (US3_CTS #29) EXP_HEADER 10 <-> 3
 *
 *   -- EFR32MG13_BRD4165A (USART/LEUART/MIXED) --
 *   PA1  (US0_TX  #1 )  <->  PC7  (US1_RX  #11)  EXP_HEADER 14 <->  6
 *   PA0  (US0_RX  #31)  <->  PC6  (US1_TX  #11)  EXP_HEADER 12 <->  4
 *   PC11 (US0_CTS #12)  <->  PB11 (US1_RTS #1)   EXP_HEADER 16 <->  8
 *   PC10 (US0_RTS #10)  <->  PD10 (US1_CTS #14)  EXP_HEADER 15 <->  7
 *
 *   -- EZR32LG/WG WSTK (USART) --
 *      USART1#1          <->     USART2#1
 *   PD0 (EXP_HEADER_4)   <->     PB3  (WSTK_P34)
 *   PD1 (EXP_HEADER_6)   <->     PB4  (WSTK_P38)
 *   PD2 (EXP_HEADER_8)   <->     PB5  (WSTK_P36)
 *   PD3 (EXP_HEADER_10)  <->     PB6  (WSTK_P35)
 *****************************************************************************/
int main(void)
{
  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Enable unity test report output via SWO (Cortex-M3/M4) or
     USART (Cortex-M0+) */
#if !defined(UNITY_TEST_REPORT_SERIAL)
  SWO_Setup();
#else
  UNITY_UsartSetup();
#endif

  /* Run test */
  test_UARTDRV();
  UnityPrint("ENDSWO");

  return 0;
}

/**************************************************************************//**
 * @brief  Main test function
 *****************************************************************************/
static void test_UARTDRV( void )
{
  UnityBeginGroup(UARTDRV_TEST_NAME);

  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
  CMU_LFXOInit(&lfxoInit);

  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  RTCDRV_Init();

  RUN_TEST(test_driver_init, __LINE__);
  RUN_TEST(test_UARTDRV_GetPeripheralStatus, __LINE__);
  RUN_TEST(test_force_transmit_fc_none, __LINE__);
  RUN_TEST(test_nb_transmit_fc_none, __LINE__);

#if defined( TEST_UART_I1 )
  RUN_TEST(test_nb_receive_force_transmit_fc_hw, __LINE__);
  RUN_TEST(test_nb_receive_b_transmit_fc_hw, __LINE__);
  RUN_TEST(test_nb_receive_nb_transmit_fc_hw, __LINE__);
  RUN_TEST(test_nb_receive_multiple_transmit_fc_hw, __LINE__);
  RUN_TEST(test_nb_receive_force_transmit_fc_sw, __LINE__);
  RUN_TEST(test_large_transfer_paused_by_sw, __LINE__);
#if defined(UARTDRV_TEST_USART) && defined(_USART_ROUTELOC0_MASK)
  RUN_TEST(test_transfer_paused_by_cts, __LINE__);
  RUN_TEST(test_large_transfer_paused_by_cts, __LINE__);
#endif
  RUN_TEST(test_large_transfer_paused_by_cts_sw, __LINE__);
#endif
  RUN_TEST(test_UARTDRV_Abort, __LINE__);
#if defined(UARTDRV_TEST_USART)
  RUN_TEST(test_doc_example, __LINE__);
#endif
  UnityEnd();
}

// -----------------------------------------------------------------------------

// Test init and deinit
static void test_driver_init(void)
{
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle0));

#if defined( TEST_UART_I1 )
  {
    UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;

    TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
      UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1));

    TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
      UARTDRV_DeInit(testHandle1));
  }
#endif
}

// Test peripheral status reporting
static void test_UARTDRV_GetPeripheralStatus(void)
{
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
  uint32_t expected;

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0));

  expected = UARTDRV_STATUS_TXEN
           | UARTDRV_STATUS_TXBL
           | UARTDRV_STATUS_TXIDLE;
#if defined(UNITY_UART)
  if ((uint32_t)TEST_UART_I0_DEVICE == (uint32_t)UNITY_UART)
  {
    // Some M0 devices prints the test output on the USART port that is under test.
    // So the TXC flag will be set when running the UARTDRV_USART test, while it
    // will be cleared when running UARTDRV_LEUART and UARTDRV_MIXED.
    expected |= UARTDRV_STATUS_TXC;
  }
#endif

  TEST_ASSERT_EQUAL_HEX(expected, UARTDRV_GetPeripheralStatus(testHandle0));

  UARTDRV_ForceTransmit(testHandle0, pattern0, sizeof(pattern0));

  expected |= UARTDRV_STATUS_TXC;

  TEST_ASSERT_EQUAL_HEX(expected, UARTDRV_GetPeripheralStatus(testHandle0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle0));
}

// Test force transmit wo any receive
// FC is dont-care with ForceTransmit/Receive
static void test_force_transmit_fc_none(void)
{
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
#if defined( TEST_UART_I1 )
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
#endif
  uint32_t i;

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                   UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0));

#if defined( TEST_UART_I1 )
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                   UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1));
#endif

  for (i = 0; i < 10; i++)
  {
    TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                      UARTDRV_ForceTransmit(testHandle0,
                                            pattern0,
                                            sizeof(pattern0)));

#if defined( TEST_UART_I1 )
    TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                      UARTDRV_ForceTransmit(testHandle1,
                                            pattern0,
                                            sizeof(pattern0)));
#endif
  }

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle0));

#if defined( TEST_UART_I1 )
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle1));
#endif
}


// Test nb transmit wo any receive
// Fill TX operation queue
// Check callback count
static void test_nb_transmit_fc_none(void)
{
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
#if defined( TEST_UART_I1 )
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
#endif
  transmit_callback_count0 = 0;
  uint32_t i;
  uint8_t *buffer;
  UARTDRV_Count_t transferCount = 0;
  UARTDRV_Count_t bytesRemaining = 0;

  // Disable HW flow control as there is no RX with RTS set
  initDataTestUartI0.fcType = uartdrvFlowControlNone;
#if defined( TEST_UART_I1 )
  initDataTestUartI1.fcType = uartdrvFlowControlNone;
#endif

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                   UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0));

#if defined( TEST_UART_I1 )
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                   UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1));
#endif

  // Fill the TX operation buffer
  for (i = 0; i < EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS; i++)
  {
    TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                      UARTDRV_Transmit(testHandle0,
                                       pattern0,
                                       sizeof(pattern0),
                                       transmit_callback0));
  }

  // Buffer is set to NULL when the queue is empty
  do
  {
    UARTDRV_GetTransmitStatus(testHandle0, &buffer, &transferCount, &bytesRemaining);
  } while (buffer != NULL);

  // Test for callback called
  TEST_ASSERT_EQUAL(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, transmit_callback_count0);

  // Do not close the uart device if it is used for unity test reporting.
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle0));

#if defined( TEST_UART_I1 )
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle1));
#endif
}


#if defined( TEST_UART_I1 )

// Test nb receive with forced transmit and hw flow control
static void test_nb_receive_force_transmit_fc_hw(void)
{
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
  uint8_t *buffer;
  UARTDRV_Count_t transferCount;
  UARTDRV_Count_t bytesRemaining;

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1));

  memset(rxBuffer0, 0, sizeof(rxBuffer0));
  memset(rxBuffer1, 0, sizeof(rxBuffer1));


  // Force transmit 1 char
  receive_callback_count0 = 0;
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Receive(testHandle0, rxBuffer0, 1, receive_callback0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_ForceTransmit(testHandle1, (uint8_t *)char0, 1));

  // Buffer is set to NULL when the queue is empty
  do
  {
    UARTDRV_GetReceiveStatus(testHandle0, &buffer, &transferCount, &bytesRemaining);
  } while (buffer != NULL);

  // Test for callback called
  TEST_ASSERT_EQUAL(1, receive_callback_count0);

  // Test RX buffer
  TEST_ASSERT_EQUAL(0, memcmp(char0, rxBuffer0, 1));

  // Test for buffer overrun
  TEST_ASSERT_EQUAL(0, rxBuffer0[1]);


  // Force transmit 4 chars
  receive_callback_count0 = 0;
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Receive(testHandle0, rxBuffer1, 4, receive_callback0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_ForceTransmit(testHandle1, pattern0, 4));

  // Buffer is set to NULL when the queue is empty
  do
  {
    UARTDRV_GetReceiveStatus(testHandle0, &buffer, &transferCount, &bytesRemaining);
  } while (buffer != NULL);

  // Test for callback called
  TEST_ASSERT_EQUAL(1, receive_callback_count0);

  // Test RX buffer
  TEST_ASSERT_EQUAL(0, memcmp(pattern0, rxBuffer1, 4));

  // Test for buffer overrun
  TEST_ASSERT_EQUAL(0, rxBuffer0[4]);


  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle1));
}
#endif

#if defined( TEST_UART_I1 )
// Test b transmit to nb receive w hw flow control.
// First a single char, then 4 chars
static void test_nb_receive_b_transmit_fc_hw(void)
{
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
  uint8_t *buffer;
  UARTDRV_Count_t transferCount;
  UARTDRV_Count_t bytesRemaining;

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1));

  memset(rxBuffer0, 0, sizeof(rxBuffer0));
  memset(rxBuffer1, 0, sizeof(rxBuffer1));


  // Force transmit 1 char
  receive_callback_count0 = 0;
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Receive(testHandle0, rxBuffer0, 1, receive_callback0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_TransmitB(testHandle1, (uint8_t *)char0, 1));

  // Buffer is set to NULL when the queue is empty
  do
  {
    UARTDRV_GetReceiveStatus(testHandle0, &buffer, &transferCount, &bytesRemaining);
  } while (buffer != NULL);

  // Test for callback called
  TEST_ASSERT_EQUAL(1, receive_callback_count0);

  // Test RX buffer
  TEST_ASSERT_EQUAL(0, memcmp(char0, rxBuffer0, 1));

  // Test for buffer overrun
  TEST_ASSERT_EQUAL(0, rxBuffer0[1]);


  // Force transmit 4 chars
  receive_callback_count0 = 0;
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Receive(testHandle0, rxBuffer1, 4, receive_callback0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_TransmitB(testHandle1, pattern0, 4));

  // Buffer is set to NULL when the queue is empty
  do
  {
    UARTDRV_GetReceiveStatus(testHandle0, &buffer, &transferCount, &bytesRemaining);
  } while (buffer != NULL);

  // Test for callback called
  TEST_ASSERT_EQUAL(1, receive_callback_count0);

  // Test RX buffer
  TEST_ASSERT_EQUAL(0, memcmp(pattern0, rxBuffer1, 4));

  // Test for buffer overrun
  TEST_ASSERT_EQUAL(0, rxBuffer0[4]);


  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle1));
}
#endif

#if defined( TEST_UART_I1 )
// Test nb transmit to nb receive in two directions w hw flow control
static void test_nb_receive_nb_transmit_fc_hw(void)
{
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
  uint8_t *buffer;
  UARTDRV_Count_t transferCount;
  UARTDRV_Count_t bytesRemaining;

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1));

  memset(rxBuffer0, 0, sizeof(rxBuffer0));
  memset(rxBuffer1, 0, sizeof(rxBuffer1));

  receive_callback_count0 = 0;
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Receive(testHandle0, rxBuffer0, sizeof(pattern0), receive_callback0));

  receive_callback_count1 = 0;
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Receive(testHandle1, rxBuffer1, 6, receive_callback1));

  transmit_callback_count0 = 0;
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Transmit(testHandle0, (uint8_t *)char0, 6, transmit_callback0));

  transmit_callback_count1 = 0;
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Transmit(testHandle1, pattern0, sizeof(pattern0), transmit_callback1));

  // Buffer is set to NULL when the queue is empty
  do
  {
    UARTDRV_GetReceiveStatus(testHandle0, &buffer, &transferCount, &bytesRemaining);
  } while (buffer != NULL);

  do
  {
    UARTDRV_GetReceiveStatus(testHandle1, &buffer, &transferCount, &bytesRemaining);
  } while (buffer != NULL);

  // Test for callback called
  TEST_ASSERT_EQUAL(1, receive_callback_count0);
  TEST_ASSERT_EQUAL(1, receive_callback_count1);
  TEST_ASSERT_EQUAL(1, transmit_callback_count0);
  TEST_ASSERT_EQUAL(1, transmit_callback_count1);

  // Test RX buffers
  TEST_ASSERT_EQUAL(0, memcmp(pattern0, rxBuffer0, sizeof(pattern0)));
  TEST_ASSERT_EQUAL(0, memcmp(char0, rxBuffer1, 6));

  // Test for buffer overrun
  TEST_ASSERT_EQUAL(0, rxBuffer0[sizeof(pattern0)]);
  TEST_ASSERT_EQUAL(0, rxBuffer1[6]);

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle1));

}
#endif


#if defined( TEST_UART_I1 )
// Test multiple nb transmit to nb receive w hw flow control
static void test_nb_receive_multiple_transmit_fc_hw(void)
{
  uint32_t i;

  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
  uint8_t *buffer;
  UARTDRV_Count_t transferCount;
  UARTDRV_Count_t bytesRemaining;

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0));

    TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1));

  memset(rxBuffer0, 0, sizeof(rxBuffer0));

  receive_callback_count0 = 0;
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Receive(testHandle1, rxBuffer0, EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, receive_callback0));

  transmit_callback_count0 = 0;
  for (i = 0; i < EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS; i++)
  {
    TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                      UARTDRV_Transmit(testHandle0, &pattern0[i], 1, transmit_callback0));
  }

  // Buffer is set to NULL when the queue is empty
  do
  {
    UARTDRV_GetTransmitStatus(testHandle0, &buffer, &transferCount, &bytesRemaining);
  } while (buffer != NULL);

  do
  {
    UARTDRV_GetReceiveStatus(testHandle1, &buffer, &transferCount, &bytesRemaining);
  } while (buffer != NULL);

  // Test for callback called
  TEST_ASSERT_EQUAL(1, receive_callback_count0);
  TEST_ASSERT_EQUAL(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, transmit_callback_count0);

    // Test for buffer overrun
  TEST_ASSERT_EQUAL(0, rxBuffer0[EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS]);

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle0));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle1));

}
#endif

#if defined( TEST_UART_I1 )
// Test interrupting transmit with sw flow control bytes
static void test_nb_receive_force_transmit_fc_sw(void)
{
  uint32_t i;
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
  Ecode_t err;

  int16_t xoffFound = -1;
  int16_t xonFound = -1;

  initDataTestUartI0.fcType = uartdrvFlowControlSw;
  initDataTestUartI1.fcType = uartdrvFlowControlSw;

  err = UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  // Initialize the tx pattern
  for (i = 0; i < TX_BUFFER_SIZE; i++)
  {
    txBuffer[i] = (uint8_t) 65+i%57;
  }
  memset(rxBuffer, 0, RX_BUFFER_SIZE);

  // Setup a receiving operation of a chunk of the transmitting bytes
  receive_callback_count0 = 0;
  err = UARTDRV_Receive(testHandle1, &rxBuffer[0], RX_BUFFER_SIZE, receive_callback0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_Transmit(testHandle0, &txBuffer[0], TX_BUFFER_SIZE - 2, NULL);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  // Send XOFF and XON mid-stream
  RTCDRV_Delay(5);
  UARTDRV_FlowControlSet(testHandle0,uartdrvFlowControlOff);
  RTCDRV_Delay(5);
  UARTDRV_FlowControlSet(testHandle0,uartdrvFlowControlOn);

  // Wait for rx to be done
  while (receive_callback_count0==0);

  for (i=0; i<RX_BUFFER_SIZE; i++)
  {
    if (rxBuffer[i]==UARTDRV_FC_SW_XOFF)
    {
      xoffFound = i;
    }
    if (rxBuffer[i]==UARTDRV_FC_SW_XON)
    {
      xonFound = i;
    }
  }

  TEST_ASSERT_TRUE_MESSAGE (xonFound>0, "XON could not be found")
  TEST_ASSERT_TRUE_MESSAGE (xoffFound>0, "XOFF could not be found")
  TEST_ASSERT_TRUE_MESSAGE (xonFound<TX_BUFFER_SIZE-2, "XON found at end of transfer")
  TEST_ASSERT_TRUE_MESSAGE (xoffFound<TX_BUFFER_SIZE-2, "XOFF found at end of transfer")
  TEST_ASSERT_TRUE_MESSAGE (xoffFound<xonFound, "XON arrived before XOFF")

  // Deinitialize handles to clean up
  err = UARTDRV_DeInit(testHandle0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_DeInit(testHandle1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
}
#endif

#if defined( TEST_UART_I1 )
// tx callback functions that continues to send data until all a certain point
static void txContinue(UARTDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       uint8_t *data,
                       UARTDRV_Count_t transferCount)
{
  (void) data;
  const uint32_t txOperationCount = 32;

  txCount += transferCount;

  if (transferStatus == ECODE_EMDRV_UARTDRV_OK)
  {
    txOperationId++;
    if (txOperationId < txOperationCount)
    {
      // Schedule a new transmission
      UARTDRV_Transmit(handle, &txBuffer[0], TX_BUFFER_SIZE, txContinue);
    }
    else
    {
      txDone = true; // Normal completion
    }
  }
  else
  {
    txDone = true; // Error completion
  }
}

// rx callback functions that just sets up a new rx operations
static void rxContinue(UARTDRV_Handle_t handle,
                             Ecode_t transferStatus,
                             uint8_t *data,
                             UARTDRV_Count_t transferCount)
{
  (void)data;

  if (transferStatus == ECODE_EMDRV_UARTDRV_OK)
  {
    rxCount += transferCount;
    UARTDRV_Receive(handle, rxBuffer, RX_BUFFER_SIZE, rxContinue);
  }
}

static void test_large_transfer_paused_by_sw(void)
{
  uint32_t i;
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
  Ecode_t err;
  uint8_t * buffer;
  UARTDRV_Count_t itemsSent;
  UARTDRV_Count_t itemsReceived;
  UARTDRV_Count_t itemsRemaining;
  UARTDRV_Status_t status;
  uint32_t received;

  txCount = 0;
  rxCount = 0;
  txOperationId = 0;

  err = UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  // Initialize the tx pattern
  for (i = 0; i < TX_BUFFER_SIZE; i++)
  {
    txBuffer[i] = (uint8_t) i;
  }
  memset(rxBuffer, 0, RX_BUFFER_SIZE);

  // Setup a receiving operation of a chunk of the transmitting bytes
  err = UARTDRV_Receive(testHandle1, &rxBuffer[0], RX_BUFFER_SIZE, rxContinue);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_Transmit(testHandle0, &txBuffer[0], TX_BUFFER_SIZE, txContinue);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  RTCDRV_Delay(10);

  // Pause transmit
  err = UARTDRV_PauseTransmit(testHandle0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  // Wait for new TX FIFO to flush
  do
  {
    status = UARTDRV_GetTransmitStatus(testHandle0, &buffer, &itemsSent, &itemsRemaining);
  } while (!(status & UARTDRV_STATUS_TXC));
  // Store the current received count
  do
  {
    status = UARTDRV_GetReceiveStatus(testHandle1, &buffer, &itemsReceived, &itemsRemaining);
  } while (status & UARTDRV_STATUS_RXDATAV);
  received = itemsReceived + rxCount;
  TEST_ASSERT_TRUE_MESSAGE(received<8192, "Transmit Failed to Pause");

  // After 500ms check to verify that transfer is still paused
  RTCDRV_Delay(500);
  UARTDRV_GetReceiveStatus(testHandle1, &buffer, &itemsReceived, &itemsRemaining);
  UARTDRV_GetTransmitStatus(testHandle0, &buffer, &itemsSent, &itemsRemaining);
  itemsSent+=txCount;
  itemsReceived+=rxCount;
  TEST_ASSERT_EQUAL_MESSAGE(itemsReceived, itemsSent, "TX/RX Mismatch");
  // Allow for RX of 1 byte too few
  TEST_ASSERT_UINT_WITHIN_MESSAGE(1, itemsReceived, received, "PauseTransmit Failed");

  // Resume the transfer and check if more bytes have been received
  UARTDRV_ResumeTransmit(testHandle0);
  RTCDRV_Delay(1000);
  UARTDRV_GetReceiveStatus(testHandle1, &buffer, &itemsReceived, &itemsRemaining);
  TEST_ASSERT_TRUE_MESSAGE(received<rxCount+itemsReceived, "ResumeTransmit Failed");

  // Deinitialize handles to clean up
  err = UARTDRV_DeInit(testHandle0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_DeInit(testHandle1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
}
#endif

#if defined(TEST_UART_I1) && defined(_USART_ROUTELOC0_MASK) && defined(UARTDRV_TEST_USART)
// This test sets up a single transfer from USART0 to USART1. The receiver
// sets up a single receive operation which is smaller than the transmit
// operation. This will trigger the CTS line to go high when the receiver
// is full and the transmitter must pause the transmission and resume
// the transmission when the next receive operation is enqueued.
static void test_transfer_paused_by_cts(void)
{
  uint32_t i;
  const uint32_t rxChunk = TX_BUFFER_SIZE / 2;

  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
  uint8_t *buffer;
  UARTDRV_Count_t transferCount;
  UARTDRV_Count_t bytesRemaining;
  Ecode_t err;

  initDataTestUartI0.fcType = uartdrvFlowControlHwUart;
  initDataTestUartI1.fcType = uartdrvFlowControlHwUart;
  err = UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  // Initialize the tx pattern
  for (i = 0; i < TX_BUFFER_SIZE; i++)
  {
    txBuffer[i] = (uint8_t) i;
  }
  memset(rxBuffer, 0, RX_BUFFER_SIZE);

  receive_callback_count1 = 0;
  transmit_callback_count0 = 0;

  // Setup a receiving operation of a chunk of the transmitting bytes
  err = UARTDRV_Receive(testHandle1, &rxBuffer[0], rxChunk, receive_callback1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_Transmit(testHandle0, &txBuffer[0], TX_BUFFER_SIZE, transmit_callback0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  // Buffer is set to NULL when the queue is empty
  do
  {
    UARTDRV_GetReceiveStatus(testHandle1, &buffer, &transferCount, &bytesRemaining);
  } while (buffer != NULL);

  // Test for callback called
  TEST_ASSERT_EQUAL(1, receive_callback_count1);
  TEST_ASSERT_EQUAL(0, transmit_callback_count0);

  RTCDRV_Delay(10);

  // Setup a second rx operation which should receive the rest of the bytes
  err = UARTDRV_Receive(testHandle1, &rxBuffer[rxChunk], rxChunk, receive_callback1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  for (i = 0; i < 100; i++)
  {
    UARTDRV_GetReceiveStatus(testHandle1, &buffer, &transferCount, &bytesRemaining);
    if (buffer == NULL)
      break;
    RTCDRV_Delay(1);
  }

  TEST_ASSERT_EQUAL(2, receive_callback_count1);
  TEST_ASSERT_EQUAL(1, transmit_callback_count0);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(&txBuffer[0], &rxBuffer[0], TX_BUFFER_SIZE);

  err = UARTDRV_DeInit(testHandle0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_DeInit(testHandle1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
}
#endif

#if defined(TEST_UART_I1) && defined(_USART_ROUTELOC0_MASK) && defined(UARTDRV_TEST_USART)
// This test sets up a large transfer from UART0 to UART1. While the
// transfer is in progress we simulate that the receiver is running
// out of buffer space and is pulling the RTS line high. This will
// test how well the sender responds to changes in the CTS line.
static void test_large_transfer_paused_by_cts(void)
{
  uint32_t i;
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
  Ecode_t err;
  uint8_t * buffer;
  UARTDRV_Count_t itemsReceived;
  UARTDRV_Count_t itemsRemaining;

  txCount = 0;
  rxCount = 0;
  txOperationId = 0;
  txDone = false;

  initDataTestUartI0.fcType = uartdrvFlowControlHwUart;
  initDataTestUartI1.fcType = uartdrvFlowControlHwUart;

  err = UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  // Initialize the tx pattern
  for (i = 0; i < TX_BUFFER_SIZE; i++)
  {
    txBuffer[i] = (uint8_t) i;
  }
  memset(rxBuffer, 0, RX_BUFFER_SIZE);

  // Setup a receiving operation of a chunk of the transmitting bytes
  err = UARTDRV_Receive(testHandle1, &rxBuffer[0], RX_BUFFER_SIZE, rxContinue);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_Transmit(testHandle0, &txBuffer[0], TX_BUFFER_SIZE, txContinue);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  // Pretend that Interface 1 buffer is full from time to time
  TEST_UART_I1_DEVICE->ROUTEPEN = TEST_UART_I1_DEVICE->ROUTEPEN & ~(_USART_ROUTEPEN_RTSPEN_MASK);
  for (i = 0; i < 100; i++)
  {
    RTCDRV_Delay(5);
    GPIO_PinOutSet(TEST_UART_I1_RTS_PORT, TEST_UART_I1_RTS_PIN);
    RTCDRV_Delay(2);
    GPIO_PinOutClear(TEST_UART_I1_RTS_PORT, TEST_UART_I1_RTS_PIN);
  }

  // Wait for tx to be done
  while (!txDone)
    ;

  // Wait for rx to be done
  RTCDRV_Delay(10);
  UARTDRV_GetReceiveStatus(testHandle1, &buffer, &itemsReceived, &itemsRemaining);

  TEST_ASSERT_EQUAL_MESSAGE(8192, txCount, "Incorrect TX Count");
  TEST_ASSERT_EQUAL_MESSAGE(8192, rxCount+itemsReceived, "Incorrect RX Count");

  err = UARTDRV_DeInit(testHandle0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_DeInit(testHandle1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
}
#endif

#if defined(TEST_UART_I1)
// Test mixed CTS and SW pauses to ensure proper protections
static void test_large_transfer_paused_by_cts_sw(void)
{
  uint32_t i;
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
  Ecode_t err;
  uint8_t * buffer;
  UARTDRV_Count_t itemsSent;
  UARTDRV_Count_t itemsReceived;
  UARTDRV_Count_t itemsRemaining;
  UARTDRV_Status_t status;
  uint32_t received;

  txCount = 0;
  rxCount = 0;
  txOperationId = 0;

  initDataTestUartI0.fcType = uartdrvFlowControlHw;
  initDataTestUartI1.fcType = uartdrvFlowControlHw;

  err = UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  // Initialize the tx pattern
  for (i = 0; i < TX_BUFFER_SIZE; i++)
  {
    txBuffer[i] = (uint8_t) i;
  }
  memset(rxBuffer, 0, RX_BUFFER_SIZE);

  // Setup a receiving operation of a chunk of the transmitting bytes
  err = UARTDRV_Receive(testHandle1, &rxBuffer[0], RX_BUFFER_SIZE, rxContinue);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_Transmit(testHandle0, &txBuffer[0], TX_BUFFER_SIZE, txContinue);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);

  RTCDRV_Delay(10);

  // Pause transmit
  err = UARTDRV_PauseTransmit(testHandle0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  // Wait for new TX FIFO to flush
  do
  {
    status = UARTDRV_GetPeripheralStatus(testHandle0);
  } while (!(status & UARTDRV_STATUS_TXIDLE));

  // Store the current received count
  do
  {
    status = UARTDRV_GetReceiveStatus(testHandle1, &buffer, &itemsReceived, &itemsRemaining);
  } while (status & UARTDRV_STATUS_RXDATAV);
  received = itemsReceived + rxCount;
  TEST_ASSERT_TRUE_MESSAGE(received<8192, "Transmit Failed to Pause");

  // Set flow control to off to deassert RTS
  UARTDRV_FlowControlSet(testHandle1,uartdrvFlowControlOff);

  // After 500ms check to verify that transfer is still paused
  RTCDRV_Delay(500);
  UARTDRV_GetReceiveStatus(testHandle1, &buffer, &itemsReceived, &itemsRemaining);
  UARTDRV_GetTransmitStatus(testHandle0, &buffer, &itemsSent, &itemsRemaining);
  itemsSent+=txCount;
  itemsReceived+=rxCount;
  TEST_ASSERT_EQUAL_MESSAGE (itemsReceived, itemsSent, "TX/RX Mismatch");
  // Allow for RX of 1 byte too few
  TEST_ASSERT_UINT_WITHIN_MESSAGE(1, itemsReceived, received, "PauseTransmit Failed");

  // SW resume should do nothing because HW flow control is still off
  UARTDRV_ResumeTransmit(testHandle0);
  RTCDRV_Delay(500);
  UARTDRV_GetReceiveStatus(testHandle1, &buffer, &itemsReceived, &itemsRemaining);
  UARTDRV_GetTransmitStatus(testHandle0, &buffer, &itemsSent, &itemsRemaining);
  itemsSent+=txCount;
  itemsReceived+=rxCount;
  TEST_ASSERT_EQUAL_MESSAGE (itemsReceived, itemsSent, "TX/RX Mismatch");
  // Allow for RX of 1 byte too few
  TEST_ASSERT_UINT_WITHIN_MESSAGE(1, itemsReceived, received, "PauseTransmit Failed");

  // Asserting flow control should resume the transfer
  UARTDRV_FlowControlSet(testHandle1,uartdrvFlowControlOn);
  RTCDRV_Delay(500);
  TEST_ASSERT_TRUE_MESSAGE (received<rxCount, "ResumeTransmit Failed");

  // Deinitialize handles to clean up
  err = UARTDRV_DeInit(testHandle0);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
  err = UARTDRV_DeInit(testHandle1);
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK, err);
}
#endif

static void test_UARTDRV_Abort(void)
{
  UARTDRV_INIT_STRUCT0 initDataTestUartI0 = TEST_UART_I0;
#if defined( TEST_UART_I1 )
  UARTDRV_INIT_STRUCT1 initDataTestUartI1 = TEST_UART_I1;
#endif
  uint32_t i;

  // No flow control
  initDataTestUartI0.fcType = uartdrvFlowControlNone;
#if defined( TEST_UART_I1 )
  initDataTestUartI1.fcType = uartdrvFlowControlNone;
#endif

  // Init handle
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                   UARTDRV_INIT_FUNCTION0(testHandle0, &initDataTestUartI0));

#if defined( TEST_UART_I1 )
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                   UARTDRV_INIT_FUNCTION1(testHandle1, &initDataTestUartI1));
#endif

  // --------------------------------

  // Aborting before anything has happened should yield IDLE error
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_IDLE,
                    UARTDRV_Abort(testHandle0, uartdrvAbortTransmit));
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_IDLE,
                    UARTDRV_Abort(testHandle0, uartdrvAbortReceive));
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_IDLE,
                    UARTDRV_Abort(testHandle0, uartdrvAbortAll));

  // --------------------------------

  // Reset callback counters
  transmit_callback_count0 = 0;
  transmit_abort_count0 = 0;

  // Transmit a single buffer
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Transmit(testHandle0,
                                     pattern0,
                                     sizeof(pattern0),
                                     transmit_callback0));

  // The transfer should be aborted
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Abort(testHandle0, uartdrvAbortTransmit));

  // The callback should be called with the abort error code once
  TEST_ASSERT_EQUAL(1, transmit_abort_count0);

  // Transmit a single buffer again
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Transmit(testHandle0,
                                     pattern0,
                                     sizeof(pattern0),
                                     transmit_callback0));

  // Wait until TX queue is empty
  while (UARTDRV_GetTransmitDepth(testHandle0));

  // The callback should be called successfully once
  TEST_ASSERT_EQUAL(1, transmit_callback_count0);

  // --------------------------------

  // Reset callback counters
  transmit_callback_count0 = 0;
  transmit_abort_count0 = 0;

  // Fill the TX operation buffer
  for (i = 0; i < EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS; i++)
  {
    TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                      UARTDRV_Transmit(testHandle0,
                                       pattern0,
                                       sizeof(pattern0),
                                       transmit_callback0));
  }

  // The transfers should all be aborted
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Abort(testHandle0, uartdrvAbortTransmit));

  // Wait until TX queue is empty
  while (UARTDRV_GetTransmitDepth(testHandle0));

  // The callback should be called with the abort error code EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS times
  TEST_ASSERT_EQUAL(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, transmit_abort_count0);

  // The callback should not have been called successfully
  TEST_ASSERT_EQUAL(0, transmit_callback_count0);

  // --------------------------------

#if defined(TEST_UART_I1)

  // Reset callback counters
  transmit_callback_count0 = 0;
  transmit_abort_count0 = 0;
  receive_callback_count0 = 0;
  receive_abort_count0 = 0;

  // Aborting before anything has happened should yield IDLE error
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_IDLE,
                    UARTDRV_Abort(testHandle1, uartdrvAbortAll));

  // Setup 3 receives
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Receive(testHandle1, &rxBuffer[0], sizeof(pattern0), receive_callback0));
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Receive(testHandle1, &rxBuffer[16], sizeof(pattern0), receive_callback0));
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Receive(testHandle1, &rxBuffer[32], sizeof(pattern0), receive_callback0));

  // Aborting TX when there is only a queue of RX should yield IDLE error
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_IDLE,
                    UARTDRV_Abort(testHandle1, uartdrvAbortTransmit));

  // Setup 3 transmits
  for (i = 0; i < 3; i++)
  {
    TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                      UARTDRV_Transmit(testHandle0,
                                       pattern0,
                                       sizeof(pattern0),
                                       transmit_callback0));
  }

  // Wait for first TX to complete
  while (UARTDRV_GetTransmitDepth(testHandle0) > 2);

  // Abort TX
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Abort(testHandle0, uartdrvAbortTransmit));

  // Wait for corresponding RX to complete
  while (UARTDRV_GetReceiveDepth(testHandle1) > 2);

  // Abort RX
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_Abort(testHandle1, uartdrvAbortReceive));

  // Assert 1 received, 2 aborted
  TEST_ASSERT_EQUAL(1, receive_callback_count0);
  TEST_ASSERT_EQUAL(2, receive_abort_count0);

  // Assert 1 transmitted, 2 aborted
  TEST_ASSERT_EQUAL(1, transmit_callback_count0);
  TEST_ASSERT_EQUAL(2, transmit_abort_count0);

#endif // TEST_UART_I1

  // --------------------------------

  // Deinit handle
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle0));

#if defined( TEST_UART_I1 )
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(testHandle1));
#endif
}

#if defined(UARTDRV_TEST_USART)
#if defined(_SILICON_LABS_32B_SERIES_0)
#if !defined(USART_CTRL_MVDIS)
#include "../example/uartdrv_example_p1_nomvdis.c"
#elif !defined(USART1)
#include "../example/uartdrv_example_p1_usart0.c"
#elif defined(_EZR32_LEOPARD_FAMILY) || defined(_EZR32_WONDER_FAMILY)
#include "../example/uartdrv_example_p1_loc1.c"
#else
#include "../example/uartdrv_example_p1.c"
#endif
#else
#include "../example/uartdrv_example_p2.c"
#endif

static void test_doc_example(void)
{
  uartdrv_example();

  // Wait for transmit initiated by example to complete
  while (UARTDRV_GetTransmitDepth(handle));

  // Deinit handle inited by example
  TEST_ASSERT_EQUAL(ECODE_EMDRV_UARTDRV_OK,
                    UARTDRV_DeInit(handle));
}
#endif
