/***************************************************************************//**
 * @file em_eth.h
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

#ifndef EM_ETH_H
#define EM_ETH_H

#include "em_device.h"
#if defined(ETH_COUNT) && (ETH_COUNT > 0)

#include "em_bus.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup emlib
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ETH
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define ETH_TXD_USED         (1U << 31)
#define ETH_TXD_WRAP         (1U << 30)
#define ETH_TXD_LAST         (1U << 15)
#define ETH_TXD_LENGTH_MASK  (0x3FFFU)

#define ETH_RXD_WRAP         (1U << 1)
#define ETH_RXD_USED         (1U << 0)
#define ETH_RXD_LENGTH_MASK  (0x1FFFU)
#define ETH_RXD_ADDR_MASK    (0xFFFFFFFCU)

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/**
 * @brief
 *   This enum is used to select whether to compare a MAC address with the
 *   destination (DA) or source (SA) address.
 */
typedef enum {
  ethDestination = _ETH_SPECADDR1TOP_FILTERTYPE_DA,
  ethSource      = _ETH_SPECADDR1TOP_FILTERTYPE_SA,
} ETH_AddressCompare_TypeDef;

/**
 * @brief
 *   This enum is used to select the ethernet speed to use. Supported speeds
 *   are 10 Mbit and 100 Mbit.
 */
typedef enum {
  ethSpeed10Mbit  = 0,
  ethSpeed100Mbit = 1,
} ETH_Speed_TypeDef;

/** MDC Clock divisor selection. */
typedef enum
{
  ethMdcDiv_8     = ETH_NETWORKCFG_MDCCLKDIV_DIVBY8,   /**< Divide clock by 8. */
  ethMdcDiv_16    = ETH_NETWORKCFG_MDCCLKDIV_DIVBY16,  /**< Divide clock by 16. */
  ethMdcDiv_32    = ETH_NETWORKCFG_MDCCLKDIV_DIVBY32,  /**< Divide clock by 32. */
  ethMdcDiv_48    = ETH_NETWORKCFG_MDCCLKDIV_DIVBY48,  /**< Divide clock by 48. */
  ethMdcDiv_64    = ETH_NETWORKCFG_MDCCLKDIV_DIVBY64,  /**< Divide clock by 64. */
  ethMdcDiv_96    = ETH_NETWORKCFG_MDCCLKDIV_DIVBY96,  /**< Divide clock by 96. */
  ethMdcDiv_128   = ETH_NETWORKCFG_MDCCLKDIV_DIVBY128, /**< Divide clock by 128. */
  ethMdcDiv_224   = ETH_NETWORKCFG_MDCCLKDIV_DIVBY224, /**< Divide clock by 224. */
} ETH_MDCDiv_TypeDef;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/**
 * @brief
 *   DMA Tx descriptor.
 */
typedef struct {
  uint32_t address;
  uint32_t word1;
} ETH_TxDescriptor_TypeDef;

/**
 * @brief
 *   DMA Rx descriptor.
 */
typedef struct {
  uint32_t address;
  uint32_t word1;
} ETH_RxDescriptor_TypeDef;

/**
 * @brief
 *   Tx/Rx statistics counters.
 */
typedef struct {
  uint64_t octets;
  uint32_t frames;
  uint32_t broadcastFrames;
  uint32_t multicastFrames;
  uint32_t pauseFrames;
  uint32_t size64;
  uint32_t size65to127;
  uint32_t size128to255;
  uint32_t size256to511;
  uint32_t size512to1023;
  uint32_t size1024to1518;
  uint32_t sizeAbove1518;
} ETH_Statistics_TypeDef;

/**
 * @brief
 *   Ethernet MAC address
 *
 * @detail
 *   6 bytes address
 */
typedef struct {
  uint8_t addr[6];
} ETH_MACAddress_TypeDef;

/***************************************************************************//**
 * @brief
 *   Enable/disable ethernet transmit
 *
 * @param[in] enable
 *   @li true - enable transmit.
 *   @li false - disable transmit.
 ******************************************************************************/
__STATIC_INLINE void ETH_TxEnable(bool enable)
{
  BUS_RegBitWrite(&ETH->NETWORKCTRL, _ETH_NETWORKCTRL_ENBTX_SHIFT, enable);
}

/***************************************************************************//**
 * @brief
 *   Start ethernet transmission
 ******************************************************************************/
__STATIC_INLINE void ETH_TxStart(void)
{
  BUS_RegBitWrite(&ETH->NETWORKCTRL, _ETH_NETWORKCTRL_TXSTRT_SHIFT, 1);
}

/***************************************************************************//**
 * @brief
 *   Enable/disable ethernet receive
 *
 * @param[in] enable
 *   @li true - enable receive.
 *   @li false - disable receive.
 ******************************************************************************/
__STATIC_INLINE void ETH_RxEnable(bool enable)
{
  BUS_RegBitWrite(&ETH->NETWORKCTRL, _ETH_NETWORKCTRL_ENBRX_SHIFT, enable);
}

void ETH_TxStatisticsGet(ETH_Statistics_TypeDef * stats);
void ETH_RxStatisticsGet(ETH_Statistics_TypeDef * stats);

/***************************************************************************//**
 * @brief
 *   Clear all rx and tx statistics registers
 ******************************************************************************/
__STATIC_INLINE void ETH_StatisticsClear(void)
{
  BUS_RegBitWrite(&ETH->NETWORKCTRL, _ETH_NETWORKCTRL_CLRALLSTATSREGS_SHIFT, 1);
}

void ETH_SetAddressFilter(int id, ETH_AddressCompare_TypeDef comp, const ETH_MACAddress_TypeDef * macAddr, uint32_t ignoreMask);
uint16_t ETH_ReadPhy(uint8_t phyAddr, uint8_t phyReg);
void ETH_WritePhy(uint8_t phyAddr, uint8_t phyReg, uint16_t value);

/***************************************************************************//**
 * @brief
 *   Set the ethernet operational mode.
 *
 * @details
 *   Supported modes are 10 Mbit Half/Full Duplex, 100 Mbit Half/Full Duplex
 ******************************************************************************/
__STATIC_INLINE void ETH_SetMode(ETH_Speed_TypeDef speed, bool fullDuplex)
{
  ETH->NETWORKCFG = (ETH->NETWORKCFG & ~(_ETH_NETWORKCFG_FULLDUPLEX_MASK | _ETH_NETWORKCFG_SPEED_MASK))
                    | (uint32_t)fullDuplex << _ETH_NETWORKCFG_FULLDUPLEX_SHIFT
                    | speed << _ETH_NETWORKCFG_SPEED_SHIFT;
}

/***************************************************************************//**
 * @brief
 *   Set the MDC clock divider
 *
 * @param[in] div
 *   The divider to configure. The MDC clock rate can be maximum 2.5 MHz so the
 *   divider should be large enough to divide the ethernet peripheral clock
 *   down to maximum 2.5 MHz.
 ******************************************************************************/
__STATIC_INLINE void ETH_SetMdcDiv(ETH_MDCDiv_TypeDef div)
{
  ETH->NETWORKCFG = (ETH->NETWORKCFG & ~(_ETH_NETWORKCFG_MDCCLKDIV_MASK))
                    | div;
}

/***************************************************************************//**
 * @brief
 *   Enable interrupts.
 *
 * @param[in] flags
 *   The interrupt flags to enable.
 ******************************************************************************/
__STATIC_INLINE void ETH_IntEnable(uint32_t flags)
{
  ETH->IENS = flags;
}

/***************************************************************************//**
 * @brief
 *   Disable interrupts.
 *
 * @param[in] flags
 *   The interrupt flags to disable.
 ******************************************************************************/
__STATIC_INLINE void ETH_IntDisable(uint32_t flags)
{
  ETH->IENC = flags;
}

/***************************************************************************//**
 * @brief
 *   Get the current interrupt flags.
 *
 * @return
 *   This functions returns the current interrupt flags that are set.
 ******************************************************************************/
__STATIC_INLINE uint32_t ETH_IntGet(void)
{
  return ETH->IFCR;
}

/***************************************************************************//**
 * @brief
 *   Set the interrupt flags.
 *
 * @param[in] flags
 *   The interrupt flags to set. This is mainly used for test purposes.
 ******************************************************************************/
__STATIC_INLINE void ETH_IntSet(uint32_t flags)
{
  ETH->IENRO = flags;
}

/***************************************************************************//**
 * @brief
 *   Clear interrupt flags
 *
 * @param[in] flags
 *   The interrupt flags to clear.
 ******************************************************************************/
__STATIC_INLINE void ETH_IntClear(uint32_t flags)
{
  ETH->IFCR = flags;
}

/***************************************************************************//**
 * @brief
 *   Get a bitmask that shows which interrupts that are enabled.
 *
 * @note
 *   The ETH peripheral does not have a IEN register like the other standard
 *   EFM32 peripherals. The ETH peripheral has one register for enabling
 *   interrupts (IENS) and one register for disabling interrupts (IENC). These
 *   two registers are write only and there is a separate register that can be
 *   read to see which interrupts are enabled (IENRO). This function uses the
 *   IENRO register to see which interrupts are enabled.
 *
 *   If an interrupt is disable in the IENRO register then the interrupt flag
 *   will not be set.
 *
 * @return
 *   A bitmask that contains information on which interrupts that are
 *   enabled. A '1' bit in the bitmask means that the interrupt is enabled and
 *   a '0' bit in the bitmask means that the interrupt is disabled.
 ******************************************************************************/
__STATIC_INLINE uint32_t ETH_IntGetMask(void)
{
  return (~ETH->IENRO) & _ETH_IENRO_MASK;
}

void ETH_IntModerate(int txNanos, int rxNanos);

/** @} (end addtogroup ETH) */
/** @} (end addtogroup emlib) */

#ifdef __cplusplus
}
#endif

#endif /* defined(ETH_COUNT) && (ETH_COUNT > 0) */
#endif /* EM_ETH_H */
