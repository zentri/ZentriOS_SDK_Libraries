/**************************************************************************//**
 * @file main.c
 * @brief UART example for EFM32_G2xx_DK and EFM32_G8xx_DK
 * @version 1.6.1
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
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"

#include "nvm.h"
#include "nvm_hal.h" /* Uses the HAL for testing and error handling. Not needed in a normal environment. */

#include "unity.h"

#if defined( UNITY_TEST_REPORT_SERIAL )
#include "unity_usart.h"
#include "unity_conf.h"
#else
#include "unity_swo.h"
#endif


/** Total amount of pages allocated. */
#define NVM_PAGES_TOTAL                        (NVM_PAGES + NVM_PAGES_SCRATCH)

/* Sizes. Internal sizes of different objects */
#define NVM_HEADER_SIZE                        0x8U

/* Generic constants. Clears up the code and make it more readable. */
#define NVM_NO_WRITE_32BIT                     0xffffffffU
#define NVM_FLIP_FIRST_BIT_OF_32_WHEN_WRITE    0xffff7fffU

/* Copy of NVM internal defines */
#define NVM_FIRST_BIT_ONE                      0x8000U
#define NVM_NO_PAGE_RETURNED                   0xffffffffUL
#define NVM_HIGHEST_32BIT                      0xffffffffUL
#define NVM_PAGE_EMPTY_VALUE                   0xffffU

/* Example objects. Note that these objects must match the definitions
 * in nvm_config.c. */
extern uint32_t colorTable[];
extern uint8_t coefficientTable[];
extern uint8_t primeNumberTable[];
extern uint16_t bonusTable[];
extern uint8_t privateKeyTable[];
extern uint16_t transformTable[];
extern int32_t safetyTable[];
extern uint8_t bigEmptyTable[450];
extern int8_t smallNegativeTable[];
extern uint16_t shortPositiveTable[];
extern uint32_t singleVariable;
extern uint8_t nvmData[];

/* Example object IDs.
 * These IDs should have names that relate to the data objects defined in nvm_config.c. */
typedef enum
{
  COLOR_TABLE_ID,
  COEFFICIENT_TABLE_ID,
  PRIME_NUMBER_TABLE_ID,
  BONUS_TABLE_ID,
  PRIVATE_KEY_TABLE_ID,
  TRANSFORM_TABLE_ID,
  SINGLE_VARIABLE_ID,
  SAFETY_TABLE_ID,
  BIG_EMPTY_TABLE_ID,
  SMALL_NEGATIVE_TABLE_ID,
  SHORT_POSITIVE_TABLE_ID
} NVM_Object_Ids;

/* Example page IDs.
 * These IDs should have names that relate to the pages defined in nvm_config.c. */
typedef enum
{
  MY_PAGE_1,
  MY_PAGE_2,
  MY_PAGE_3,
  MY_PAGE_4,
  MY_PAGE_5,
  MY_PAGE_6
} NVM_Page_Ids;


/* Test parameter for test_NVM_getWearLevel */
uint32_t expWearLevel;

static uint8_t myRand = 0;
uint8_t MyRand(void)
{
  return(--myRand);
}

/**************************************************************************//**
 * @brief  Functional copy of NVM_PageFind() function
 *****************************************************************************/
uint8_t* test_PageFind(uint16_t pageId)
{
  uint16_t page;
  /* Physical address to return. */
  uint8_t  *pPhysicalAddress = (uint8_t *)(NVM_ConfigGet()->nvmArea);
  /* Temporary variable used to read and compare logical page address. */
  uint16_t logicalAddress;

  /* Loop through memory looking for a matching watermark. */
  for (page = 0; page < NVM_ConfigGet()->pages; ++page)
  {
    /* Allow both versions of writing mark, invalid duplicates should already
     * have been deleted.
     * offsetof(NVM_Page_Header_t, watermark) = 4, but NVM_Page_Header_t is not public
     */
    NVMHAL_Read(pPhysicalAddress + 4, &logicalAddress, sizeof(logicalAddress));
    if (((pageId | NVM_FIRST_BIT_ONE) == logicalAddress) || (pageId == logicalAddress))
    {
      return pPhysicalAddress;
    }

    /* Move lookup point to the next page. */
    pPhysicalAddress += NVM_PAGE_SIZE;
  }

  /* No page found. */
  return (uint8_t *) NVM_NO_PAGE_RETURNED;
}

/**************************************************************************//**
 * @brief  Functional copy of NVM_ScratchPageFindBest() function
 *****************************************************************************/
uint8_t* test_ScratchPageFindBest(void)
{
  uint16_t page;
  /* Address for physical page to return. */
  uint8_t  *pPhysicalPage = (uint8_t *) NVM_NO_PAGE_RETURNED;

  /* Variable used to read and compare update id of physical pages. */
  uint32_t eraseCount = NVM_HIGHEST_32BIT;
  /* The best update id found. */
  uint32_t loEraseCount = NVM_HIGHEST_32BIT;

  /* Pointer to the current physical page. */
  uint8_t  *pPhysicalAddress = (uint8_t *)(NVM_ConfigGet()->nvmArea);
  /* Logical address that identifies the page. */
  uint16_t logicalAddress;

  /* Loop through all pages in memory. */
  for (page = 0; page < NVM_ConfigGet()->pages; ++page)
  {
    /* Read and check logical address.
     * offsetof(NVM_Page_Header_t, watermark) = 4, but NVM_Page_Header_t is not public
     */
    NVMHAL_Read(pPhysicalAddress + 4, &logicalAddress, sizeof(logicalAddress));
    if ((uint16_t) NVM_PAGE_EMPTY_VALUE == logicalAddress)
    {
      /* Find and compare erase count.
       * offsetof(NVM_Page_Header_t, updateId) = 0, but NVM_Page_Header_t is not public
       */
      NVMHAL_Read(pPhysicalAddress + 0, &eraseCount, sizeof(eraseCount));
      if (eraseCount < loEraseCount)
      {
        loEraseCount  = eraseCount;
        pPhysicalPage = pPhysicalAddress;
      }
    }

    /* Move lookup point to the next page. */
    pPhysicalAddress += NVM_PAGE_SIZE;
  }

  /* Return a pointer to the best/least used page. */
  return pPhysicalPage;
}


/* Some of the following tests are known to overlap, and some of them might be
 * redundant and could be removed. */

/* Test return of init function that should fail, because nvm contains no valid data. */
void test_NVM_initAfterStart(void)
{
  Ecode_t result;

  result = NVM_Init(NVM_ConfigGet());
  if (result == ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE)
  {
    /* Erase and write to get ECODE_EMDRV_NVM_OK */
    NVM_Erase(0);
    NVM_Write(MY_PAGE_1, NVM_WRITE_ALL_CMD);
    result = NVM_Init(NVM_ConfigGet());
  }
  TEST_ASSERT( (ECODE_EMDRV_NVM_OK == result) );
}

/* Test return of erase function that should succeed. */
void test_NVM_erase(void)
{
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Erase(0));
}

/* Test return of erase function with the input set to retention. */
void test_NVM_eraseRetain(void)
{
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Erase(NVM_ERASE_RETAINCOUNT));
}

/* Test correct error returned when initializing a system with no data. */
void test_NVM_initAfterErase(void)
{
  TEST_ASSERT(ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE == NVM_Init(NVM_ConfigGet()));
}

/* Test a read on an empty system, and check for correct return. */
void test_NVM_readAfterErase(void)
{
  TEST_ASSERT(ECODE_EMDRV_NVM_PAGE_INVALID == NVM_Read(MY_PAGE_1, COLOR_TABLE_ID));
  TEST_ASSERT(ECODE_EMDRV_NVM_PAGE_INVALID == NVM_Read(MY_PAGE_1, NVM_READ_ALL_CMD));
}

/* Test a valid write after erase. */
void test_NVM_validWriteAfterErase(void)
{
  int i;
  /* Initialize test data. */
  for (i = 0; i < 5; i++)
  {
    colorTable[i] = i;
  }

  /* Write data. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, NVM_WRITE_ALL_CMD));
}

/* Test read valid data, and compare with temp. */
void test_NVM_readValidData(void)
{
  int i;

  /* Initialize test data. */
  for (i = 0; i < 5; i++)
  {
    colorTable[i] = i;
  }

  /* Write test data. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, COLOR_TABLE_ID));

  /* Take a backup of the data, and reset the original to 0. */
  uint8_t CompareTable[5];
  for (i = 0; i < 5; i++)
  {
    CompareTable[i] = colorTable[i];
    colorTable[i] = 0;
  }

  /* Read the object. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_1, COLOR_TABLE_ID));

  /* Compare with the old data. */
  for (i = 0; i < 5; i++)
  {
    TEST_ASSERT(CompareTable[i] == colorTable[i]);
  }
}

/* Test read. */
void test_NVM_readValidDataAll(void)
{
  int i;

  /* Initialize test data. */
  for (i = 0; i < 4; i++)
  {
    colorTable[i] = MyRand();
  }
  for (i = 0; i < 4; i++)
  {
    bonusTable[i] = 256 - i;
  }

  /* Write test data. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, NVM_WRITE_ALL_CMD));

  /* Initialize comparison data, write 0 to driver data. */
  uint32_t colorCompareTable[4];
  uint16_t bonusCompareTable[4];
  for (i = 0; i < 4; i++)
  {
    colorCompareTable[i]  = colorTable[i];
    colorTable[i] = 0;
    bonusCompareTable[i] = bonusTable[i];
    bonusTable[i] = 0;
  }

  /* Read back data from NVM. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_1, NVM_READ_ALL_CMD));

  /* Compare. */
  for (i = 0; i < 4; i++)
  {
    TEST_ASSERT(colorCompareTable[i] == colorTable[i]);
    TEST_ASSERT(bonusCompareTable[i] == bonusTable[i]);
  }
}

/* Test a valid write of a sub-word object. */
void test_NVM_validSmallVariableWriteRead(void)
{
  singleVariable = 1;
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, SINGLE_VARIABLE_ID));

  singleVariable ^= 0x71;
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_1, SINGLE_VARIABLE_ID));

  TEST_ASSERT(singleVariable == 1);
}

/* Test initialization when system is in use. */
void test_NVM_initAfterWrite(void)
{
  Ecode_t status;

  status = NVM_Init(NVM_ConfigGet());
  if ((status != ECODE_EMDRV_NVM_OK) && (status != ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE))
  {
    TEST_ASSERT(false);
  }
}

/* Test rewrite of part of existing. */
void test_NVM_firstPageWriteUpdate(void)
{
  colorTable[0] += 2;
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, COLOR_TABLE_ID));
}

/* Test if updated data really was updated. */
void test_NVM_firstPageReadUpdate(void)
{
  uint32_t temp = colorTable[0];
  colorTable[0] += 2;
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_1, COLOR_TABLE_ID));
  TEST_ASSERT(temp == colorTable[0]);
}

/* Test if all data was copied. */
void test_NVM_firstPageReadTableTwo(void)
{
  bonusTable[1] = 8;
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, BONUS_TABLE_ID));
  bonusTable[1] = 10;
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_1, BONUS_TABLE_ID));
  TEST_ASSERT(8 == bonusTable[1]);
}

/* Test second page write. */
void test_NVM_secondPageWrite(void)
{
  Ecode_t expected, result;

  expected = ECODE_EMDRV_NVM_OK;
  result = NVM_Write(MY_PAGE_2, NVM_WRITE_ALL_CMD);
  TEST_ASSERT_EQUAL_UINT32(expected, result);
}

/* Test second page write many times. This is used to test wear level over multiple pages. */
void test_NVM_secondPageWriteMany(void)
{
  int32_t i;
  Ecode_t result;
  for (i = 0; i < 50; i++)
  {
    safetyTable[0] = i;
    result = NVM_Write(MY_PAGE_2, NVM_WRITE_ALL_CMD);
    TEST_ASSERT(ECODE_EMDRV_NVM_OK == result);
    safetyTable[0] = i + 100;
    TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_2, SAFETY_TABLE_ID));
    TEST_ASSERT(safetyTable[0] == i);
  }
}

/* Test second page read. */
void test_NVM_secondPageRead(void)
{
  safetyTable[1] = 0;
  TEST_ASSERT_EQUAL(ECODE_EMDRV_NVM_OK, NVM_Read(MY_PAGE_2, NVM_READ_ALL_CMD));
  TEST_ASSERT(72 == safetyTable[1]);
}

/* Test wear level function. */
void test_NVM_getWearLevel(void)
{
#if (NVM_FEATURE_WEARLEVELGET_ENABLED)
  TEST_ASSERT_EQUAL(expWearLevel, NVM_WearLevelGet());
#else
  TEST_IGNORE_MESSAGE("WearLevelGet disabled.");
#endif
}

/* Test what happens if we suddenly have two similar pages with different write bits. */
void test_NVM_interruptedWriteNonCorrupt(void)
{
  int i;
  /* Erase. */
  if (ECODE_EMDRV_NVM_OK != NVM_Erase(0))
  {
    TEST_IGNORE_MESSAGE("Erase operation unsuccessful.");
  }

  /* Init. */
  if (ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE != NVM_Init(NVM_ConfigGet()))
  {
    TEST_IGNORE_MESSAGE("Init operation unsuccessful.");
  }

  /* Setup data */
  for (i = 0; i < 4; i++)
  {
    colorTable[i] = 3 * i;
  }

  /* Write data. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, NVM_WRITE_ALL_CMD));

  /* Take a copy of the page  */
  uint8_t *oldAddress = test_PageFind(MY_PAGE_1);
  uint8_t *newAddress = test_ScratchPageFindBest();

  NVMHAL_Write(newAddress, oldAddress, NVM_PAGE_SIZE);

  /* Mark old as to write. */
  uint32_t buffer = NVM_FLIP_FIRST_BIT_OF_32_WHEN_WRITE;
  NVMHAL_Write(oldAddress, &buffer, 4);

  /* Init. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Init(NVM_ConfigGet()));

  /* Read and check. */
  uint8_t CompareTable[4];
  for (i = 0; i < 4; i++)
  {
    CompareTable[i] = colorTable[i];
    colorTable[i] = 0;
  }

  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_1, COLOR_TABLE_ID));

  for (i = 0; i < 4; i++)
  {
    TEST_ASSERT(CompareTable[i] == colorTable[i]);
  }
}

/* Test what happens if we suddenly have two similar pages with different write bits and one with bad data. */
void test_NVM_interruptedWriteCorrupt(void)
{
  int i;
  /* Erase. */
  if (ECODE_EMDRV_NVM_OK != NVM_Erase(0))
  {
    TEST_IGNORE_MESSAGE("Erase operation unsuccessful.");
  }

  /* Init. */
  if (ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE != NVM_Init(NVM_ConfigGet()))
  {
    TEST_IGNORE_MESSAGE("Init operation unsuccessful.");
  }

  /* Setup data */
  for (i = 0; i < 4; i++)
  {
    colorTable[i] = 3 * i;
  }

  /* Write data. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, NVM_WRITE_ALL_CMD));

  /* Take a copy of the page  */
  uint8_t *oldAddress = test_PageFind(MY_PAGE_1);
  uint8_t *newAddress = test_ScratchPageFindBest();

  NVMHAL_Write(newAddress, oldAddress, NVM_PAGE_SIZE);

  /* Mark old as to write. */
  uint32_t buffer = NVM_FLIP_FIRST_BIT_OF_32_WHEN_WRITE;
  NVMHAL_Write(oldAddress, &buffer, 4);

  buffer = 0;
  NVMHAL_Write(newAddress + NVM_HEADER_SIZE, &buffer, 4);

  /* Init. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Init(NVM_ConfigGet()));

  /* Read and check. */
  uint32_t CompareTable[4];
  for (i = 0; i < 4; i++)
  {
    CompareTable[i] = colorTable[i];
    colorTable[i] = 0;
  }

  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_1, COLOR_TABLE_ID));

  for (i = 0; i < 4; i++)
  {
    TEST_ASSERT(CompareTable[i] == colorTable[i]);
  }
}

/* Make sure the static wear level system moves stale data. */
void test_NVM_staticWear(void)
{
#if (NVM_FEATURE_STATIC_WEAR_ENABLED)
  uint16_t i;

  /* Erase. */
  if (ECODE_EMDRV_NVM_OK != NVM_Erase(0))
  {
    TEST_IGNORE_MESSAGE("Erase operation unsuccessful.");
  }

  /* Init. */
  if (ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE != NVM_Init(NVM_ConfigGet()))
  {
    TEST_IGNORE_MESSAGE("Init operation unsuccessful.");
  }

  /* Setup data */
  for (i = 0; i < 4; i++)
  {
    colorTable[i]  = 1 * (i + 1);
    coefficientTable[i] = 2 * (i + 1);
    primeNumberTable[i] = 3 * (i + 1);
  }

  /* Never updating. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_4, NVM_WRITE_ALL_CMD));
  uint8_t *address = test_PageFind(MY_PAGE_4);
  bool moved = false;

  /* Update loop. */
  for (i = 0; i < 200; i++)
  {
    colorTable[0]  = i;
    coefficientTable[0] = i;

    TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, NVM_WRITE_ALL_CMD));

    if (test_PageFind(MY_PAGE_1) != address)
    {
      moved = true;
    }

    if (rand() % 100 > 60)
    {
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_3, NVM_WRITE_ALL_CMD));
    }

    if (test_PageFind(MY_PAGE_3) != address)
    {
      moved = true;
    }
  }

  if (!moved)
  {
    TEST_FAIL_MESSAGE("The static page was not moved.");
  }
#else
  TEST_IGNORE_MESSAGE("The static wear leveling system was left out on compile time.");
#endif
}

/* Test initialization and read operation on corrupt data. */
void test_NVM_brokenDataInit(void)
{
  int i;
  Ecode_t result;

  /* Erase. */
  if (ECODE_EMDRV_NVM_OK != NVM_Erase(0))
  {
    TEST_IGNORE_MESSAGE("Erase operation unsuccessful.");
  }

  /* Init. */
  result = NVM_Init(NVM_ConfigGet());
  if (ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE != result)
  {
    TEST_IGNORE_MESSAGE("Init operation unsuccessful.");
  }

  /* Setup data */
  for (i = 0; i < 4; i++)
  {
    colorTable[i] = 3333 * (i + 1);
  }

  /* Write data. */
  result = NVM_Write(MY_PAGE_1, NVM_WRITE_ALL_CMD);
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == result);
  uint8_t *address = test_PageFind(MY_PAGE_1);

  /* Break data. */
  uint32_t temp = 0;
  result = NVMHAL_Write(address + NVM_HEADER_SIZE, &temp, 4);
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == result);

  /* Reinitialize, and check for error. */
  result = NVM_Init(NVM_ConfigGet());
  TEST_ASSERT(ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE == result);

  /* Try to read data. Should return an error. */
#if (NVM_FEATURE_READ_VALIDATION_ENABLED)
  result = NVM_Read(MY_PAGE_1, NVM_WRITE_ALL_CMD);
  TEST_ASSERT(ECODE_EMDRV_NVM_DATA_INVALID == result);
#else
  TEST_IGNORE_MESSAGE("Read validation disabled. Some tests have been run.");
#endif
}

/* Check return if scratchpad is tainted. */
void test_NVM_brokenScratchPadWriteRead(void)
{
#if (NVM_FEATURE_WRITE_VALIDATION_ENABLED)
  int i;

  /* Erase. */
  if (ECODE_EMDRV_NVM_OK != NVM_Erase(0))
  {
    TEST_IGNORE_MESSAGE("Erase operation unsuccessful.");
  }


  /* Init. */
  if (ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE != NVM_Init(NVM_ConfigGet()))
  {
    TEST_IGNORE_MESSAGE("Init operation unsuccessful.");
  }

  /* Setup data */
  for (i = 0; i < 4; i++)
  {
    colorTable[i] = 7777 * (i + 1);
  }

  /* Break scratchpad. */
  uint32_t     temp     = 0;
  uint8_t      *address = test_ScratchPageFindBest();
  Ecode_t      result;

  result = NVMHAL_Write(address + NVM_HEADER_SIZE, &temp, 4);
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == result);

  /* Write data. Should return an error. */
  result = NVM_Write(MY_PAGE_1, NVM_WRITE_ALL_CMD);
  TEST_ASSERT(ECODE_EMDRV_NVM_ERROR == result);

  /* Try to read data. Should return an invalid data error. */
#if (NVM_FEATURE_READ_VALIDATION_ENABLED)
  result = NVM_Read(MY_PAGE_1, NVM_WRITE_ALL_CMD);
  TEST_ASSERT(ECODE_EMDRV_NVM_DATA_INVALID == result);
#else
  TEST_IGNORE_MESSAGE("Read validation disabled. Some test have been run.");
#endif
#else
  TEST_IGNORE_MESSAGE("Write validation disabled.");
#endif
}

/* Test fifth and sixth page write. */
void test_NVM_fifthAndSixthPageWrite(void)
{
  Ecode_t expected, result;

  expected = ECODE_EMDRV_NVM_OK;
  result = NVM_Write(MY_PAGE_5, NVM_WRITE_ALL_CMD);
  TEST_ASSERT_EQUAL_UINT32(expected, result);
  result = NVM_Write(MY_PAGE_6, SHORT_POSITIVE_TABLE_ID);
  TEST_ASSERT_EQUAL_UINT32(expected, result);
}

/* Test many read write cycles spread across different objects and pages. */
void test_NVM_allPagesReadWriteManyRandom(void)
{
  int i;
  int32_t testSel;

  /* Erase. */
  if (ECODE_EMDRV_NVM_OK != NVM_Erase(0))
  {
    TEST_IGNORE_MESSAGE("Erase operation unsuccessful.");
  }

  /* Initialization. */
  if (ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE != NVM_Init(NVM_ConfigGet()))
  {
    TEST_IGNORE_MESSAGE("Init operation unsuccessful.");
  }

  /* Setup data */
  for (i = 0; i < 4; i++)
  {
    colorTable[i] = 1234 * (i + 1);
    coefficientTable[i] = 5678 * (i + 1);
    primeNumberTable[i] = 9012 * (i + 1);
    bonusTable[i] = 3456 * (i + 1);
  }

  /* Write initial versions. */
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, NVM_WRITE_ALL_CMD));
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_2, SAFETY_TABLE_ID));
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_3, NVM_WRITE_ALL_CMD));
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_4, NVM_WRITE_ALL_CMD));
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_5, SMALL_NEGATIVE_TABLE_ID));
  TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_6, NVM_WRITE_ALL_CMD));

  uint32_t temp1;
  int32_t temp2;
  /* Perform hundreds of randomly distributed write-read checks. */
  for (i = 0; i < 700; i++)
  {
    testSel = rand();
    testSel = testSel % 6;
    switch (testSel)
    {
    case 0:
      colorTable[3] = 42 * rand() % 10;
      temp1         = colorTable[3];
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_1, COLOR_TABLE_ID));
      colorTable[3] = 0xff;
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_1, COLOR_TABLE_ID));
      TEST_ASSERT(temp1 == colorTable[3]);
      break;

    case 1:
      safetyTable[8] = 42 * rand() % 10;
      temp2 = safetyTable[8];
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_2, SAFETY_TABLE_ID));
      safetyTable[8] = 0xffffffff;
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_2, NVM_READ_ALL_CMD));
      TEST_ASSERT(temp2 == safetyTable[8]);
      break;

    case 2:
      coefficientTable[3] = 42 * rand() % 10;
      temp1 = coefficientTable[3];
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_3, COEFFICIENT_TABLE_ID));
      coefficientTable[3] = 0xff;
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_3, COEFFICIENT_TABLE_ID));
      TEST_ASSERT(temp1 == coefficientTable[3]);
      break;

    case 3:
      primeNumberTable[3] = 42 * rand() % 10;
      temp1 = primeNumberTable[3];
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_4, PRIME_NUMBER_TABLE_ID));
      primeNumberTable[3] = 0xff;
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_4, PRIME_NUMBER_TABLE_ID));
      TEST_ASSERT(temp1 == primeNumberTable[3]);
      break;

    case 4:
      smallNegativeTable[0] = (smallNegativeTable[0] * i) & 0xFF;
      temp2 = smallNegativeTable[0];
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_5, SMALL_NEGATIVE_TABLE_ID));
      smallNegativeTable[0] = -3;
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_5, SMALL_NEGATIVE_TABLE_ID));
      TEST_ASSERT(temp2 == smallNegativeTable[0]);
      break;

    case 5:
      shortPositiveTable[2] = (shortPositiveTable[2] + i) & 0xFFFF;
      temp1 = shortPositiveTable[2];
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Write(MY_PAGE_6, SHORT_POSITIVE_TABLE_ID));
      shortPositiveTable[2] = 0xFF;
      TEST_ASSERT(ECODE_EMDRV_NVM_OK == NVM_Read(MY_PAGE_6, SHORT_POSITIVE_TABLE_ID));
      TEST_ASSERT(temp1 == shortPositiveTable[2]);
      break;

    default:
      TEST_ASSERT(false);
    }
  }
}

static void RunAllTests(void)
{
  /* Run Unity tests. */
  RUN_TEST(test_NVM_initAfterStart, __LINE__);

  /* This test sets eraseCount = 0 */
  RUN_TEST(test_NVM_erase, __LINE__);
  expWearLevel = 0;
  RUN_TEST(test_NVM_getWearLevel, __LINE__);

  RUN_TEST(test_NVM_initAfterErase, __LINE__);
  RUN_TEST(test_NVM_readAfterErase, __LINE__);

  /* Wear-level should be unchanged. */
  RUN_TEST(test_NVM_getWearLevel, __LINE__);

  /* This writes the page data to start of 1st page, eg to 0x1000 for 4k page size parts. */
  RUN_TEST(test_NVM_validWriteAfterErase, __LINE__);
  RUN_TEST(test_NVM_readValidData, __LINE__);

  /* This writes the page data to start of 2st page, eg to 0x2000 for 4k page size parts,
     then the 1st page is erased while its erase count is incremented. */
  RUN_TEST(test_NVM_validWriteAfterErase, __LINE__);
  expWearLevel = 1;
  RUN_TEST(test_NVM_getWearLevel, __LINE__);

  RUN_TEST(test_NVM_readValidDataAll, __LINE__);
  RUN_TEST(test_NVM_initAfterWrite, __LINE__);
  RUN_TEST(test_NVM_secondPageWrite, __LINE__);
  RUN_TEST(test_NVM_firstPageWriteUpdate, __LINE__);
  RUN_TEST(test_NVM_firstPageReadUpdate, __LINE__);
  RUN_TEST(test_NVM_validSmallVariableWriteRead, __LINE__);
  RUN_TEST(test_NVM_secondPageRead, __LINE__);

  /* Wear-level should be unchanged. */
  RUN_TEST(test_NVM_getWearLevel, __LINE__);

  RUN_TEST(test_NVM_secondPageWrite, __LINE__);

  /* Wear-level should be unchanged. */
  RUN_TEST(test_NVM_getWearLevel, __LINE__);

  RUN_TEST(test_NVM_secondPageWriteMany, __LINE__);

  /* test_NVM_secondPageWriteMany may increment erase count in some pages,
     depending on FLASH_PAGE_SIZE */
  expWearLevel = NVM_WearLevelGet();
  TEST_ASSERT(expWearLevel >= 1);

  RUN_TEST(test_NVM_firstPageReadTableTwo, __LINE__);

  /* This test sets eraseCount = NVM_ERASE_RETAINCOUNT, ie should still be
     unchanged. */
  RUN_TEST(test_NVM_eraseRetain, __LINE__);
  RUN_TEST(test_NVM_getWearLevel, __LINE__);

  /* This test sets eraseCount = 0 */
  RUN_TEST(test_NVM_erase, __LINE__);
  expWearLevel = 0;
  RUN_TEST(test_NVM_getWearLevel, __LINE__);

  RUN_TEST(test_NVM_interruptedWriteNonCorrupt, __LINE__);
  RUN_TEST(test_NVM_interruptedWriteCorrupt, __LINE__);

  /* This test sets eraseCount = NVM_ERASE_RETAINCOUNT
     so the wear-level should be unchanged. */
  RUN_TEST(test_NVM_eraseRetain, __LINE__);
  RUN_TEST(test_NVM_getWearLevel, __LINE__);

  RUN_TEST(test_NVM_erase, __LINE__);
  RUN_TEST(test_NVM_initAfterErase, __LINE__);

  RUN_TEST(test_NVM_brokenDataInit, __LINE__);

  RUN_TEST(test_NVM_brokenScratchPadWriteRead, __LINE__);

  RUN_TEST(test_NVM_staticWear, __LINE__);

  RUN_TEST(test_NVM_fifthAndSixthPageWrite, __LINE__);

  RUN_TEST(test_NVM_allPagesReadWriteManyRandom, __LINE__);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip revision alignment and errata fixes */
  CHIP_Init();

#if defined( UNITY_TEST_REPORT_SERIAL )
  UNITY_UsartSetup();
#else
  SWO_Setup();
#endif

  /* Setup Unity, and start. */
  UnityBeginGroup("NVM");

  /* Test with unaligned access trap both enabled and disabled.
     The ability to use this driver with unaligned access trap is required by a customer:
     https://jira.silabs.com/browse/EFM32ESS-1443
     The original driver was designed with use of unaligned access, so that is also required
     functionality.
  */
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
  UnityPrint("Unaligned access trap enabled");
  UNITY_PRINT_EOL;
  RunAllTests();
  SCB->CCR &= ~SCB_CCR_UNALIGN_TRP_Msk;
  UnityPrint("Unaligned access trap disabled");
  UNITY_PRINT_EOL;
  RunAllTests();

  /* Write summary. */
  UnityEnd();
  UnityPrint("ENDSWO");

  return 0;
}


