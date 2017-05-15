/**************************************************************************//**
 * @file em_eth.c
 * @brief Ethernet peripheral API
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

#include "em_eth.h"

#if defined(ETH_COUNT) && (ETH_COUNT > 0)

#include "em_assert.h"

/***************************************************************************//**
 * @addtogroup emlib
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ETH
 * @brief Ethernet peripheral API
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Retrieve the tx statistics counter
 *
 * @param[out] stats
 *   @li true - enable receive.
 *   @li false - disable receive.
 ******************************************************************************/
void ETH_TxStatisticsGet(ETH_Statistics_TypeDef * stats)
{
  stats->octets          = (uint64_t)ETH->OCTETSTXEDBOTTOM;
  stats->octets         |= (uint64_t)ETH->OCTETSTXEDTOP << 32;
  stats->frames          = ETH->FRAMESTXEDOK;
  stats->broadcastFrames = ETH->BROADCASTTXED;
  stats->multicastFrames = ETH->MULTICASTTXED;
  stats->pauseFrames     = ETH->PFRAMESTXED;
  stats->size64          = ETH->FRAMESTXED64;
  stats->size65to127     = ETH->FRAMESTXED65;
  stats->size128to255    = ETH->FRAMESTXED128;
  stats->size256to511    = ETH->FRAMESTXED256;
  stats->size512to1023   = ETH->FRAMESTXED512;
  stats->size1024to1518  = ETH->FRAMESTXED1024;
  stats->sizeAbove1518   = ETH->FRAMESTXED1519;
}

/***************************************************************************//**
 * @brief
 *   Retrieve the rx statistics counter
 *
 * @param[out] stats
 *   @li true - enable receive.
 *   @li false - disable receive.
 ******************************************************************************/
void ETH_RxStatisticsGet(ETH_Statistics_TypeDef * stats)
{
  stats->octets          = (uint64_t)ETH->OCTETSRXEDBOTTOM;
  stats->octets         |= (uint64_t)ETH->OCTETSRXEDTOP << 32;
  stats->frames          = ETH->FRAMESRXEDOK;
  stats->broadcastFrames = ETH->BROADCASTRXED;
  stats->multicastFrames = ETH->MULTICASTRXED;
  stats->pauseFrames     = ETH->PFRAMESRXED;
  stats->size64          = ETH->FRAMESRXED64;
  stats->size65to127     = ETH->FRAMESRXED65;
  stats->size128to255    = ETH->FRAMESRXED128;
  stats->size256to511    = ETH->FRAMESRXED256;
  stats->size512to1023   = ETH->FRAMESRXED512;
  stats->size1024to1518  = ETH->FRAMESRXED1024;
  stats->sizeAbove1518   = ETH->FRAMESRXED1519;
}

/***************************************************************************//**
 * @brief
 *   Configure a specific address filter
 *
 * @details
 *   There are 4 specific address filters available numbered 1-4. These address
 *   filters can filter incoming packets based on either source or destination
 *   MAC address.
 *
 * @param[in] id
 *   The number of the address filter to configure. Valid range is 1-4
 *
 * @param[in] comp
 *   Choose between source or destination address matching
 *
 * @param[in] macAddr
 *   The MAC address to filter on
 *
 * @param[in] ignoreMask
 *   Use this parameter to select which bytes of the MAC address that should be
 *   ignored when comparing incoming frames with the @ref macAddr. ignoreMask
 *   is only supported for filters 2-4. Here are some example ignoreMask
 *   values:
 *
 *   0x00 - Compare all bytes in the mac address.
 *   0x01 - Ignore byte 0 of the mac address, but compare all the others.
 *   0x3E - Ignore all bytes except byte 0.
 ******************************************************************************/
void ETH_SetAddressFilter(int id,
                          ETH_AddressCompare_TypeDef comp,
                          const ETH_MACAddress_TypeDef * macAddr,
                          uint32_t ignoreMask)
{
  __IO uint32_t * bottom;
  __IO uint32_t * top;
  uint32_t byteMask = (ignoreMask << _ETH_SPECADDR2TOP_FILTERBYTEMASK_SHIFT)
                      & _ETH_SPECADDR2TOP_FILTERBYTEMASK_MASK;


  EFM_ASSERT((id >= 1) && (id <= 4));

  switch (id)
  {
    case 1:
      bottom = &ETH->SPECADDR1BOTTOM;
      top = &ETH->SPECADDR1TOP;
      byteMask = 0; // SPECADDR1 does not support byte masks
      break;
    case 2:
      bottom = &ETH->SPECADDR2BOTTOM;
      top = &ETH->SPECADDR2TOP;
      break;
    case 3:
      bottom = &ETH->SPECADDR3BOTTOM;
      top = &ETH->SPECADDR3TOP;
      break;
    case 4:
      bottom = &ETH->SPECADDR4BOTTOM;
      top = &ETH->SPECADDR4TOP;
      break;
    default:
      return; // Invalid parameter and asserts are turned off
  }

  *bottom = (macAddr->addr[0])
            | (macAddr->addr[1] << 8)
            | (macAddr->addr[2] << 16)
            | (macAddr->addr[3] << 24);
  *top    = byteMask
            | (comp << _ETH_SPECADDR1TOP_FILTERTYPE_SHIFT)
            | (macAddr->addr[4])
            | (macAddr->addr[5] << 8);
}

/***************************************************************************//**
 * @brief
 *   Read a 16 bit value from a PHY register
 *
 * @note
 *   This read function is using the clause 22 frame format for reading the
 *   PHY register.
 *
 * @param[in] phyAddr
 *   PHY address, range from 0-31
 *
 * @param[in] phyReg
 *   PHY register, range from 0-31
 *
 * @return
 *   value of the PHY register
 ******************************************************************************/
uint16_t ETH_ReadPhy(uint8_t phyAddr, uint8_t phyReg)
{
  // phyAddr and phyReg is only 5 bits
  EFM_ASSERT((phyAddr & ~0x1F) == 0);
  EFM_ASSERT((phyReg  & ~0x1F) == 0);

  // Read a PHY register using the Clause 22 frame format
  ETH->PHYMNGMNT = 0x40020000
                   | (0x2 << _ETH_PHYMNGMNT_OPERATION_SHIFT)
                   | (phyAddr << _ETH_PHYMNGMNT_PHYADDR_SHIFT)
                   | (phyReg  << _ETH_PHYMNGMNT_REGADDR_SHIFT);
  while ((ETH->NETWORKSTATUS & ETH_NETWORKSTATUS_MANDONE) == 0)
    ;
  return ETH->PHYMNGMNT & _ETH_PHYMNGMNT_PHYRWDATA_MASK;
}

/***************************************************************************//**
 * @brief
 *   Write a 16 bit value to a PHY register
 *
 * @note
 *   This write function is using the clause 22 frame format for writing the
 *   PHY register value.
 *
 * @param[in] phyAddr
 *   PHY address, range from 0-31
 *
 * @param[in] phyReg
 *   PHY register, range from 0-31
 *
 * @param[in] value
 *   value to write into the PHY register
 ******************************************************************************/
void ETH_WritePhy(uint8_t phyAddr, uint8_t phyReg, uint16_t value)
{
  // phyAddr and phyReg is only 5 bits
  EFM_ASSERT((phyAddr & ~0x1F) == 0);
  EFM_ASSERT((phyReg  & ~0x1F) == 0);

  /* Write a PHY register using the Clause 22 frame format */
  ETH->PHYMNGMNT = 0x40020000
                   | (0x1 << _ETH_PHYMNGMNT_OPERATION_SHIFT)
                   | (phyAddr << _ETH_PHYMNGMNT_PHYADDR_SHIFT)
                   | (phyReg  << _ETH_PHYMNGMNT_REGADDR_SHIFT)
                   | (value << _ETH_PHYMNGMNT_PHYRWDATA_SHIFT);
  while ((ETH->NETWORKSTATUS & ETH_NETWORKSTATUS_MANDONE) == 0)
    ;
}

/***************************************************************************//**
 * @brief
 *   Set the interrupt moderation timing.
 *
 * @details
 *   The parameters to this function is in units of nanoseconds, however the
 *   actual hardware supports only a resolution of 800 ns.
 *
 * @param[in] txNanos
 *   Set the number of nano seconds to delay before the tx complete interrupt
 *   flag is set. The valid range is 0-204000 ns.
 *
 * @param[in] rxNanos
 *   Set the number of nano seconds to delay before the rx complete interrupt
 *   flag is set. The valid range is 0-204000 ns.
 ******************************************************************************/
void ETH_IntModerate(int txNanos, int rxNanos)
{
  EFM_ASSERT(txNanos >= 0);
  EFM_ASSERT(rxNanos >= 0);

  // Saturate txNanos to max value
  if (txNanos > (800 * 0xFF))
  {
    txNanos = 800 * 0xFF;
  }

  // Saturate rxNanos to max value
  if (rxNanos > (800 * 0xFF))
  {
    rxNanos = 800 * 0xFF;
  }
  
  uint32_t txIntMod = txNanos / 800;
  uint32_t rxIntMod = rxNanos / 800;
  ETH->IMOD = (txIntMod << _ETH_IMOD_TXINTMOD_SHIFT)
              | (rxIntMod << _ETH_IMOD_RXINTMOD_SHIFT);
}


/** @} (end addtogroup ETH) */
/** @} (end addtogroup emlib) */

#endif /* defined(ETH_COUNT) && (ETH_COUNT > 0) */
