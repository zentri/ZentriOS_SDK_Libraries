/**************************************************************************//**
 * @file main.c
 * @brief TEMPDRV test for EFM32
 * @version INTERNAL
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com</b>
 ******************************************************************************
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
#include "em_gpio.h"

#include "unity_swo.h"
#include "unity.h"

#include "tempdrv.h"

static void test_TEMPDRV(void);
static void test_driver_init(void);
static void test_driver_reg(void);
static void test_driver_enable(void);
static void test_errata_fix(void);

int main(void)
{
  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  SWO_Setup();

  /* Run test */
  test_TEMPDRV();
  UnityPrint("ENDSWO");
  return 0;
}

/**************************************************************************//**
 * Callbacks
 *****************************************************************************/
static void high_callback(int8_t temp, TEMPDRV_LimitType_t limit)
{
  (void) temp;
  TEST_ASSERT_EQUAL(TEMPDRV_LIMIT_HIGH, limit);
}

static void low_callback(int8_t temp, TEMPDRV_LimitType_t limit)
{
  (void) temp;
  TEST_ASSERT_EQUAL(TEMPDRV_LIMIT_LOW, limit);
}

/**************************************************************************//**
 * @brief  Main test function
 *****************************************************************************/
static void test_TEMPDRV( void )
{
  UnityBeginGroup("TEMPDRV");

  RUN_TEST(test_driver_init, __LINE__);
  RUN_TEST(test_driver_reg, __LINE__);
  RUN_TEST(test_driver_enable, __LINE__);
  RUN_TEST(test_driver_enable, __LINE__);
  RUN_TEST(test_errata_fix, __LINE__);

  UnityEnd();
}

// Test init and deinit
static void test_driver_init(void)
{
  int8_t temp;

  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK, TEMPDRV_Init());

  temp = TEMPDRV_GetTemp();
  UnityPrintf("Current Temperature=%d\n", temp);

  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK, TEMPDRV_DeInit());

  // Should fail because TEMPDRV needs initialization
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_NO_INIT,
                    TEMPDRV_RegisterCallback(temp+10,
                                             TEMPDRV_LIMIT_HIGH,
                                             high_callback));
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_NO_INIT,
                    TEMPDRV_RegisterCallback(temp-10,
                                             TEMPDRV_LIMIT_LOW,
                                             low_callback));

  // DeInit should clear registered callbacks
  TEMPDRV_Init();
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_RegisterCallback(temp+10,
                                             TEMPDRV_LIMIT_HIGH,
                                             high_callback));
  TEMPDRV_DeInit();
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_NO_CALLBACK,
                    TEMPDRV_UnregisterCallback(high_callback));
}

// Test callback registration
static void test_driver_reg(void)
{
  TEMPDRV_Init();
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()+10,
                                             TEMPDRV_LIMIT_HIGH,
                                             high_callback));

  TEST_ASSERT_EQUAL(1, TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_HIGH));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()-10,
                                             TEMPDRV_LIMIT_LOW,
                                             low_callback));

  TEST_ASSERT_EQUAL(1, TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_LOW));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_UnregisterCallback(high_callback));

  TEST_ASSERT_EQUAL(0, TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_HIGH));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_UnregisterCallback(low_callback));

  TEST_ASSERT_EQUAL(0, TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_LOW));

  // Attempt to register callback for wrong temperature
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_BAD_LIMIT,
                    TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()+10,
                                             TEMPDRV_LIMIT_LOW,
                                             high_callback));
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_BAD_LIMIT,
                    TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()-10,
                                             TEMPDRV_LIMIT_HIGH,
                                             low_callback));

  TEST_ASSERT_EQUAL(0, TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_HIGH));
  TEST_ASSERT_EQUAL(0, TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_LOW));

  // Attempt to unregister callback that hasn't been registered.
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_NO_CALLBACK,
                    TEMPDRV_UnregisterCallback(high_callback));

  // Attempt to register two callbacks at the same temperature
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()+10,
                                             TEMPDRV_LIMIT_HIGH,
                                             high_callback));
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_DUP_TEMP,
                    TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()+10,
                                             TEMPDRV_LIMIT_HIGH,
                                             high_callback));
  TEST_ASSERT_EQUAL(1, TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_HIGH));

  // Register a second callback with the same callback
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()+20,
                                             TEMPDRV_LIMIT_HIGH,
                                             high_callback));
  TEST_ASSERT_EQUAL(2, TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_HIGH));

  // Test unregistration. First call should clear both.
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_UnregisterCallback(high_callback));
  TEST_ASSERT_EQUAL(0, TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_HIGH));
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_NO_CALLBACK,
                    TEMPDRV_UnregisterCallback(high_callback));

  // Max out registration
  Ecode_t status;
  uint8_t i=0;
  do {
    i++;
    status = TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()+10+2*i,
                                      TEMPDRV_LIMIT_HIGH,
                                      high_callback);
  } while (status==ECODE_EMDRV_TEMPDRV_OK);
  // Should fail to register at TEMPDRV_CUSTOM_CALLBACK_DEPTH
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_NO_SPACE,status);
  TEST_ASSERT_EQUAL(TEMPDRV_CUSTOM_CALLBACK_DEPTH,i-1);
  TEST_ASSERT_EQUAL(TEMPDRV_CUSTOM_CALLBACK_DEPTH,
                    TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_HIGH));

  TEMPDRV_DeInit();

  TEST_ASSERT_EQUAL(0, TEMPDRV_GetActiveCallbacks(TEMPDRV_LIMIT_HIGH));
}

// Test enable/disable
static void test_driver_enable(void)
{
  TEMPDRV_Init();

  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK, TEMPDRV_Enable(true));
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK, TEMPDRV_Enable(false));
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK, TEMPDRV_Enable(false));

  //Register/Unregister call back should still work when disabled
  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()+10,
                                             TEMPDRV_LIMIT_HIGH,
                                             high_callback));

  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_UnregisterCallback(high_callback));

  TEMPDRV_Enable(true);

  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_RegisterCallback(TEMPDRV_GetTemp()+10,
                                             TEMPDRV_LIMIT_HIGH,
                                             high_callback));

  TEMPDRV_Enable(false);

  TEST_ASSERT_EQUAL(ECODE_EMDRV_TEMPDRV_OK,
                    TEMPDRV_UnregisterCallback(high_callback));

}

static void test_errata_fix(void)
{
  TEMPDRV_Init();
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
  // Test that the errata fix is enabled
  volatile uint32_t * reg = (volatile uint32_t *) 0x400E3164;
  TEST_ASSERT_BITS(0x74, 0x74, *reg);
#else
  // Test that the errata fix is disabled
  volatile uint32_t * reg = (volatile uint32_t *) 0x400E3164;
  TEST_ASSERT_BITS(0x74, 0x70, *reg);
#endif
  TEMPDRV_DeInit();
}

