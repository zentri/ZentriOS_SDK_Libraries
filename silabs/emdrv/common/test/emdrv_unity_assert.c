/***************************************************************************//**
 * @file  custom_assert.c
 * @brief Assert handler for regression testing.
 * @author Silicon Laboratories, Inc.
 *
 * @version 4.4.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Silicon Laboratories, Inc, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include "em_assert.h"
#include "unity.h"
#if defined(UNITY_TEST_REPORT_SERIAL)
#include "unity_usart.h"
#endif
#include <string.h>

/**************************************************************************//**
 * @brief Unity assert handler for test. This handler generates a Unity error
 *        message and then ends the testrun.
 * @param file File where assertion triggered
 * @param line Line where assertion triggered
 *****************************************************************************/
void assertEFM(const char *file, int line)
{
#if defined(UNITY_TEST_REPORT_SERIAL)
  // Ensure unity output is initialized
  UNITY_UsartSetup();
#endif
  char failMessage[128];

  snprintf(failMessage, sizeof(failMessage), "ASSERT:%s:%d", file, line);
  TEST_FAIL_MESSAGE(failMessage);
}

void HardFault_Handler(void)
{
  UnityPrintf("HARNESS:0:<unknown>:FAIL:HardFault\n");
  UnityPrintf("ENDSWO\n");
  while (1);
}
