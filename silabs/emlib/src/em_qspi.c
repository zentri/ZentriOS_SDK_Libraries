/**************************************************************************//**
 * @file em_qspi.c
 * @brief QSPI Octal-SPI Flash Controller API
 * @version 5.1.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
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

#include "em_qspi.h"

#if defined(QSPI_COUNT) && (QSPI_COUNT > 0)

#include "em_assert.h"

/***************************************************************************//**
 * @addtogroup emlib
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup QSPI
 * @{
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initialize QSPI.
 *
 * @param[in] qspi
 *   Pointer to the QSPI peripheral register block.
 *
 * @param[in] init
 *   Pointer to initialization structure used to configure QSPI.
 ******************************************************************************/
void QSPI_Init(QSPI_TypeDef * qspi, const QSPI_Init_TypeDef * init)
{
  uint32_t divisor;

  EFM_ASSERT((init->divisor >= 2) && (init->divisor <= 32));
  divisor = init->divisor / 2 - 1;

  qspi->CONFIG = (qspi->CONFIG & ~_QSPI_CONFIG_MSTRBAUDDIV_MASK)
                 | (divisor << _QSPI_CONFIG_MSTRBAUDDIV_SHIFT);
  QSPI_Enable(qspi, init->enable);
}

/***************************************************************************//**
 * @brief
 *   Configure Read Operations.
 *
 * @param[in] qspi
 *   Pointer to the QSPI peripheral register block.
 *
 * @param[in] config
 *   Pointer to configuration structure for QSPI read operations.
 ******************************************************************************/
void QSPI_ReadConfig(QSPI_TypeDef * qspi, const QSPI_ReadConfig_TypeDef * config)
{
  EFM_ASSERT(config->dummyCycles < 31);

  QSPI_WaitForIdle(qspi);
  qspi->DEVINSTRRDCONFIG = (config->opCode << _QSPI_DEVINSTRRDCONFIG_RDOPCODENONXIP_SHIFT)
                           | (config->dummyCycles << _QSPI_DEVINSTRRDCONFIG_DUMMYRDCLKCYCLES_SHIFT)
                           | (config->addrTransfer << _QSPI_DEVINSTRRDCONFIG_ADDRXFERTYPESTDMODE_SHIFT)
                           | (config->dataTransfer << _QSPI_DEVINSTRRDCONFIG_DATAXFERTYPEEXTMODE_SHIFT)
                           | (config->instTransfer << _QSPI_DEVINSTRRDCONFIG_INSTRTYPE_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Configure Write Operations.
 *
 * @param[in] qspi
 *   Pointer to the QSPI peripheral register block.
 *
 * @param[in] config
 *   Pointer to configuration structure for QSPI write operations.
 ******************************************************************************/
void QSPI_WriteConfig(QSPI_TypeDef * qspi, const QSPI_WriteConfig_TypeDef * config)
{
  EFM_ASSERT(config->dummyCycles < 31);

  QSPI_WaitForIdle(qspi);
  qspi->DEVINSTRWRCONFIG = (config->opCode << _QSPI_DEVINSTRWRCONFIG_WROPCODE_SHIFT)
                           | (config->dummyCycles << _QSPI_DEVINSTRWRCONFIG_DUMMYWRCLKCYCLES_SHIFT)
                           | (config->addrTransfer << _QSPI_DEVINSTRWRCONFIG_ADDRXFERTYPESTDMODE_SHIFT)
                           | (config->dataTransfer << _QSPI_DEVINSTRWRCONFIG_DATAXFERTYPEEXTMODE_SHIFT)
                           | ((config->autoWEL ? 0 : 1) << _QSPI_DEVINSTRWRCONFIG_WELDIS_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Execute a STIG command.
 *
 * @details
 *   STIG means "software triggered instruction generator" and is used when the
 *   application needs to access status registers, configuration registers or
 *   perform erase functions. The STIG commands can be used to perform any
 *   instruction that the flash device supports.
 *
 * @param[in] qspi
 *   Pointer to the QSPI peripheral register block.
 *
 * @param[in] stigCmd
 *   Pointer to a structure that describes the STIG command.
 ******************************************************************************/
void QSPI_ExecStigCmd(QSPI_TypeDef * qspi, const QSPI_StigCmd_TypeDef * stigCmd)
{
  uint32_t i;

  EFM_ASSERT(stigCmd->addrSize <= 4);
  EFM_ASSERT(stigCmd->writeDataSize <= 8);
  EFM_ASSERT(stigCmd->readDataSize <= 8);
  EFM_ASSERT(stigCmd->dummyCycles < 32);

  if (stigCmd->writeDataSize)
  {
    EFM_ASSERT(stigCmd->writeBuffer);
  }

  if (stigCmd->readDataSize)
  {
    EFM_ASSERT(stigCmd->readBuffer);
  }

  QSPI_WaitForIdle(qspi);

  qspi->FLASHCMDCTRL = (stigCmd->cmdOpcode << _QSPI_FLASHCMDCTRL_CMDOPCODE_SHIFT)
                       | (stigCmd->dummyCycles << _QSPI_FLASHCMDCTRL_NUMDUMMYCYCLES_SHIFT);

  if (stigCmd->writeDataSize)
  {
    uint32_t buffer[2] = {0, 0};
    uint8_t * dst = (uint8_t *) buffer;
    uint8_t * src = stigCmd->writeBuffer;

    qspi->FLASHCMDCTRL |= QSPI_FLASHCMDCTRL_ENBWRITEDATA
                          | ((stigCmd->writeDataSize - 1)
                             << _QSPI_FLASHCMDCTRL_NUMWRDATABYTES_SHIFT);

    for (i = 0; i < stigCmd->writeDataSize; i++)
    {
      dst[i] = src[i];
    }

    qspi->FLASHWRDATALOWER = buffer[0];
    qspi->FLASHWRDATAUPPER = buffer[1];
  }

  if (stigCmd->addrSize)
  {
    qspi->FLASHCMDCTRL |= QSPI_FLASHCMDCTRL_ENBCOMDADDR
                          | ((stigCmd->addrSize - 1)
                             << _QSPI_FLASHCMDCTRL_NUMADDRBYTES_SHIFT);
    qspi->FLASHCMDADDR = stigCmd->address;
  }

  if (stigCmd->modeBitEnable)
  {
    qspi->FLASHCMDCTRL |= QSPI_FLASHCMDCTRL_ENBMODEBIT;
  }

  if (stigCmd->readDataSize)
  {
    qspi->FLASHCMDCTRL |= QSPI_FLASHCMDCTRL_ENBREADDATA
                          | ((stigCmd->readDataSize - 1)
                             << _QSPI_FLASHCMDCTRL_NUMRDDATABYTES_SHIFT);
  }

  // Start command execution
  qspi->FLASHCMDCTRL |= QSPI_FLASHCMDCTRL_CMDEXEC;

  while (qspi->FLASHCMDCTRL & QSPI_FLASHCMDCTRL_CMDEXECSTATUS)
    ;

  // Read data if any
  if (stigCmd->readDataSize)
  {
    uint32_t buffer[2] = {0, 0};
    const uint8_t * src = (const uint8_t *)buffer;
    uint8_t * dst = stigCmd->readBuffer;

    buffer[0] = qspi->FLASHRDDATALOWER;
    buffer[1] = qspi->FLASHRDDATAUPPER;

    for (i = 0; i < stigCmd->readDataSize; i++)
    {
      dst[i] = src[i];
    }
  }
}

/***************************************************************************//**
 * @brief
 *   Configure Indirect Access.
 *
 * @param[in] qspi
 *   Pointer to the QSPI peripheral register block.
 *
 * @param[in] config
 *   Pointer to a structure used to configure indirect access.
 ******************************************************************************/
void QSPI_IndirectConfig(QSPI_TypeDef * qspi, const QSPI_IndirectConfig_TypeDef * config)
{
  EFM_ASSERT((config->triggerAddr >= QSPI0_MEM_BASE)
             && (config->triggerAddr <= QSPI0_MEM_END));

  qspi->INDAHBADDRTRIGGER = config->triggerAddr;
  qspi->INDIRECTTRIGGERADDRRANGE = 2; // Setup 2^2=4 bytes as the range for triggering
  qspi->INDIRECTREADXFERWATERMARK = config->readWatermark;
  qspi->INDIRECTWRITEXFERWATERMARK = config->writeWatermark;
}

/***************************************************************************//**
 * @brief
 *   Perform an flash read operation using Indirect Access.
 *
 * @param[in] qspi
 *   Pointer to the QSPI peripheral register block.
 *
 * @param[in] buffer
 *   Address of the buffer to read data into.
 *
 * @param[in] n
 *   Number of bytes to read.
 ******************************************************************************/
void QSPI_IndirectRead(QSPI_TypeDef * qspi, void * buffer, unsigned n)
{
  qspi->INDIRECTREADXFERSTART = (uint32_t) buffer;
  qspi->INDIRECTREADXFERNUMBYTES = n;
}

/***************************************************************************//**
 * @brief
 *   Perform an flash write operation using Indirect Access.
 *
 * @param[in] qspi
 *   Pointer to the QSPI peripheral register block.
 *
 * @param[in] buffer
 *   Address of the buffer to write data from.
 *
 * @param[in] n
 *   Number of bytes to write.
 ******************************************************************************/
void QSPI_IndirectWrite(QSPI_TypeDef * qspi, const void * buffer, unsigned n)
{
  qspi->INDIRECTWRITEXFERSTART = (uint32_t) buffer;
  qspi->INDIRECTWRITEXFERNUMBYTES = n;
}


/** @} (end addtogroup QSPI) */
/** @} (end addtogroup emlib) */

#endif /* defined(QSPI_COUNT) && (QSPI_COUNT > 0) */
