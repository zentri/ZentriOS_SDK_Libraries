#ifndef __EMDRV_DMA_H_
#define __EMDRV_DMA_H_

#include <stdint.h>

/*****************************************************************************/
/* Prototypes                                                                */
/*****************************************************************************/
#include "types/em_types.h"
#include "ecode/emdrv_ecode.h"

/*****************************************************************************/
/* Include module configuration                                              */
/*****************************************************************************/
#include "dma_config.h"


/*****************************************************************************/
/* Module status codes                                                       */
/*****************************************************************************/
#define DMA_OK                          0x00000000
#define DMA_ERROR_DRIVER_NOT_ENABLED    ( ECODE_EMDRV_DMA_BASE | 0x0001 )
#define DMA_ERROR_CONFIGURATION_INVALID ( ECODE_EMDRV_DMA_BASE | 0x0002 )
#define DMA_ERROR_DEVICE_HANDLE_INVALID ( ECODE_EMDRV_DMA_BASE | 0x0003 )
#define DMA_ERROR_TRANSFER_TIMEOUT      ( ECODE_EMDRV_DMA_BASE | 0x0004 )
#define DMA_ERROR_BUS_FAILURE           ( ECODE_EMDRV_DMA_BASE | 0x0005 )
#define DMA_ERROR_PARAMETER_INVALID     ( ECODE_EMDRV_DMA_BASE | 0x0006 )



/*****************************************************************************/
/* Internal Typedefs  (will be moved to internal h-file later.               */
/*****************************************************************************/

/**
 * @brief
 *   DMA channel descriptor.
 * @details
 *   This data structure is internal to the driver in is not visible to the
 *   user.
 */
typedef struct
{
  DMA_ChannelConfig_T  cfg;
} DMA_Channel_T;


/**
 * @brief
 *   DMA transfer descriptor.
 * @details
 *   This data structure is internal to the driver in is not visible to the
 *   user.
 */
typedef struct
{
  void                     *srcAddress;
  void                     *destAddress;
  unsigned int              size;
  DMA_Callback_Desc_T       callback;
  
  /* Type of DMA transfer. Basic, Auto-request, ping-pong or scatter-gather
     in memory or peripheral. */
  DMA_CycleCtrl_TypeDef     dmaType;
  /** Arbitration rate */
  DMA_ArbiterConfig_TypeDef arbRate;
  /** Privileged access control for source read accesses and
      destination write accesses. */
  uint8_t                   privileged;

} DMA_XferDescriptor_T;




/*****************************************************************************/
/* Public Typedefs                                                           */
/*****************************************************************************/

/**
 * @brief
 *   DMA channel capabilities data structure.
 * @details
 *   This data structure holds all channel specific configuration parameters.
 *   It is used when requesting a DMA channel with given capabilities from
 *   the system.
 */
typedef struct
{
  /** Loop mode support */
  bool     LoopMode;
  /** 2D transfer support. */
  bool     TwoDimensionalMode;
} DMA_ChannelCapabilities_T


/**
 * @brief
 *   DMA channel handle.
 * @details
 *   This handle is used to represent a specific DMA channel when calling
 *   the interface functions of the DMA driver.
 */
typedef void* DMA_Channel;



/**
 * @brief
 *   DMA channel configuration data structure.
 * @details
 *   This data structure holds all channel specific configuration parameters.
 */
typedef struct
{

  /** Priority of channel. */
  int      Priority;

} DMA_ChannelConfig_T;


/**
 * @brief
 *   DMA transfer handle.
 * @details
 *   This handle is used to represent a DMA transfer when calling
 *   the interface functions of the DMA driver.
 */
typedef void* DMA_Xfer;


/**
 * @brief
 *   DMA Transfer completion callback function pointer.
 * @details
 *   Parameters:
 *   @li channel - The DMA channel the callback function is invoked for.
 *   @li userArg - User definable reference that may be used to pass information
 *     to be used by the callback handler. If used, the referenced data must be
 *     valid at the point when the interrupt handler invokes the callback.
 *     If callback changes  any data in the provided user structure, remember
 *     that those changes are done in interrupt context, and proper protection
 *     of data may be required.
 */
typedef void (*DMA_Callback_T)(DMA_Xfer xfer, void *userArg);


/**
 * @brief
 *   Callback structure that can be used to define DMA complete actions.
 * @details
 *   A reference to this structure is only stored in the primary descriptor
 *   for a channel (if callback feature is used). If callback is required
 *   for both primary and alternate descriptor completion, this must be
 *   handled by one common callback, using the provided 'primary' parameter
 *   with the callback function.
 */
typedef struct
{
  /**
   * Pointer to callback function to invoke when DMA transfer cycle done.
   * Notice that this function is invoked in interrupt context, and therefore
   * should be short and non-blocking.
   */
  DMA_Callback_T   callback;

  /** User defined pointer to provide with callback function. */
  void            *userArg;
} DMA_Callback_Desc_T;


/**
 * @brief
 *   DMA Descriptor attribute identifier list.
 * @details
 *   List of DMA descriptor attributes that may be configured by calling the
 *   DMA_XferAttributeSet function.
 */
typedef enum
{
  SourceBuffer,
  DestinationBuffer,
  Size,
  BytesXfered,                   /** Number of bytes transferred. */
  IoDirection,                   /** Direction of data stream when DMAing
                                     to/from a peripheral device. */
  DMA_CycleCtrl_Type,            /** Type of DMA transfer. Basic, Auto-request,
                                     ping-pong or scatter-gather
                                     in memory or peripheral. */
  DMA_ArbitrationRate,           /** Arbitration rate */
  DMA_PrivilegedSrcReadAccess,   /** Privileged access control for source
                                     read access */
  DMA_PrivilegedDestWriteAccess, /** Privileged access control for
                                     destination write accesses. */
  DMA_Loop,                      /** Restarts DMA loop from nMinus-1
                                     indefinitely, until stopped by user. */
  TwoDimensionalCopy,            /** 2D DMA to copy "rectangles" in memory.
                                     Height, width and stride values are
                                     specified in the DMA_2D_T structure
                                     pointed to by the value parameter of the
                                     DMA_XferAttributeSet function. */

} DMA_XferAttribute_T;


/**
 * @brief
 *   DMA 2D copy descriptor.
 * @details
 *   TBW
 */
typedef struct
{
  int   width;      /* Width of rectangle to copy. */
  int   height;     /* Height of rectangle to copy. */
  int   srcStride;  /* Stride in the source buffer. Ref. user manual. */
  int   dstStride;  /* Stride in the destination buffer. Ref. user manual. */
} DMA_2D_T;


/**
 * @brief
 *   List of peripheral signals capable of requesting DMA. 
 * @details
 *   TBW
 */
typedef enum
{
  DMA_Peripheral_NONE,
  DMA_Peripheral_ADC0,
  DMA_Peripheral_DAC0,
  DMA_Peripheral_USART0,
  DMA_Peripheral_USART1,
  DMA_Peripheral_USART2,
  DMA_Peripheral_LEUART0,
  DMA_Peripheral_LEUART1,
  DMA_Peripheral_TIMER0,
  DMA_Peripheral_TIMER1,
  DMA_Peripheral_TIMER2,
  DMA_Peripheral_TIMER3,
  DMA_Peripheral_UART0,
  DMA_Peripheral_UART1,
  DMA_Peripheral_MSC,
  DMA_Peripheral_AES,
  DMA_Peripheral_LESENSE,
  DMA_Peripheral_EBI
} DMA_PeripheralId_T;

/**
 * @brief
 *   List of peripheral signals capable of requesting DMA. 
 * @details
 *   TBW
 */
typedef enum
{
  DMA_Peripheral_NONE,
  DMA_Peripheral_ADC0_SINGLE,
  DMA_Peripheral_ADC0_SCAN,
  DMA_Peripheral_DAC0_CH0,
  DMA_Peripheral_DAC0_CH1,
  DMA_Peripheral_USART0_RXDATAV,
  DMA_Peripheral_USART0_TXBL,
  DMA_Peripheral_USART0_TXEMPTY,
  DMA_Peripheral_USART1_RXDATAV,
  DMA_Peripheral_USART1_TXBL,
  DMA_Peripheral_USART1_TXEMPTY,
  DMA_Peripheral_USART1_RXDATAVRIGHT,
  DMA_Peripheral_USART1_TXBLRIGHT,
  DMA_Peripheral_USART2_RXDATAV,
  DMA_Peripheral_USART2_TXBL,
  DMA_Peripheral_USART2_TXEMPTY,
  DMA_Peripheral_USART2_RXDATAVRIGHT,
  DMA_Peripheral_USART2_TXBLRIGHT,
  DMA_Peripheral_LEUART0_RXDATAV,
  DMA_Peripheral_LEUART0_TXBL,
  DMA_Peripheral_LEUART0_TXEMPTY,
  DMA_Peripheral_LEUART1_RXDATAV,
  DMA_Peripheral_LEUART1_TXBL,
  DMA_Peripheral_LEUART1_TXEMPTY,
  DMA_Peripheral_TIMER0_UFOF,
  DMA_Peripheral_TIMER0_CC0,
  DMA_Peripheral_TIMER0_CC1,
  DMA_Peripheral_TIMER0_CC2,
  DMA_Peripheral_TIMER1_UFOF,
  DMA_Peripheral_TIMER1_CC0,
  DMA_Peripheral_TIMER1_CC1,
  DMA_Peripheral_TIMER1_CC2,
  DMA_Peripheral_TIMER2_UFOF,
  DMA_Peripheral_TIMER2_CC0,
  DMA_Peripheral_TIMER2_CC1,
  DMA_Peripheral_TIMER2_CC2,
  DMA_Peripheral_TIMER3_UFOF,
  DMA_Peripheral_TIMER3_CC0,
  DMA_Peripheral_TIMER3_CC1,
  DMA_Peripheral_TIMER3_CC2,
  DMA_Peripheral_UART0_RXDATAV,
  DMA_Peripheral_UART0_TXBL,
  DMA_Peripheral_UART0_TXEMPTY,
  DMA_Peripheral_UART1_RXDATAV,
  DMA_Peripheral_UART1_TXBL,
  DMA_Peripheral_UART1_TXEMPTY,
  DMA_Peripheral_MSC_WDATA,
  DMA_Peripheral_AES_DATAWR,
  DMA_Peripheral_AES_XORDATAWR,
  DMA_Peripheral_AES_DATARD,
  DMA_Peripheral_AES_KEYWR,
  DMA_Peripheral_LESENSE_BUFDATAV,
  DMA_Peripheral_EBI_PXL0EMPTY,
  DMA_Peripheral_EBI_PXL1EMPTY,
  DMA_Peripheral_EBI_PXLFULL,
  DMA_Peripheral_EBI_DDEMPTY
} DMA_PeripheralRequestSignal_T;


/**
 * @brief
 *   DMA peripheral I/O direction type.
 * @details
 *   TBW
 */
typedef enum
{
  DMA_PERIPHERAL_INPUT,
  DMA_PERIPHERAL_OUTPUT
} DMA_IoDirection_T;

/**
 * @brief
 *   DMA peripheral structure.
 * @details
 *   TBW
 */
typedef struct
{
  DMA_PeripheralId_T              peripheralId;
  DMA_PeripheralRequestSignal_T   requestSignal;
  DMA_IoDirection_T               direction;
} DMA_Peripheral_T;


/**
 * @brief
 *   DMA buffer descriptor
 * @details
 *   TBW
 */
typedef struct
{
  void     *data;  /* Pointer to data buffer. */
  uint32_t  size;  /* Size of data buffer. */
} DMA_Buffer_T;


/**
 * @brief
 *   DMA split stream configuration
 * @details
 *   TBW
 */
typedef struct
{
  uint32_t       splitStreams;  /* # of streams */
  uint32_t       blockSize;     /* size of each stream block "stripe size" */
  DMA_Buffer_T*  bufferList;
} DMA_SplitStreamCfg_T;


/**
 * @brief
 *   List of peripheral signals capable of requesting DMA. 
 * @details
 *   TBW
 */
typedef enum
{
  DMA_UNINITIALIZED,
  DMA_READY,
  DMA_IN_PROGRESS,
  DMA_DONE
} DMA_XferStatus_T;


/*****************************************************************************/
/* Public API                                                                */
/*****************************************************************************/
EMSTATUS       DMA_Init             ( void );
EMSTATUS       DMA_DeInit           ( void );

EMSTATUS       DMA_channelAcquire   ( DMA_Channel         *channel, DMA_channelCapabilities_t *channelCaps );
EMSTATUS       DMA_channelRelease   ( DMA_Channel          channel );

EMSTATUS       DMA_channelConfig    ( DMA_Channel          channel,
                                      DMA_ChannelConfig_T* cfg );

EMSTATUS       DMA_xferPrepare      ( DMA_Channel          channel,
                                      DMA_Xfer             xfer,
                                      void*                src,
                                      void*                dest,
                                      uint32_t             size );

/**
 * Use cases:
 *
 * - Memory to memory (fire and forget)
 * - Device to memory (continous streaming)
 * - Query how much data transfered since last time?
 */


EMSTATUS       DMA_xferCreate       ( DMA_Channel             channel,
                                      DMA_Xfer*               xfer);
EMSTATUS       DMA_xferFree         ( DMA_Xfer                xfer );


/* Regular MEMCOPY */
EMSTATUS       DMA_memcpy           ( DMA_Channel             channel,
                                      void*                   src,
                                      void*                   dest,
                                      uint32_t                size,
                                      DMA_Xfer                xfer ); /* May be NULL. */

/* Stream data to/from a peripheral device. */
EMSTATUS       DMA_PeripheralStream ( DMA_Channel             channel,
                                      DMA_Buffer_T            buffer,
                                      DMA_Peripheral_T        peripheral,
                                      bool                    continous,
                                      DMA_Xfer                xfer ); /* May be NULL. */

/* Split data stream to/from a peripheral device to a set of buffers */
EMSTATUS       DMA_PeripheralSplitStream( DMA_Channel             channel,
                                          DMA_SplitStreamCfg_T    splitStreamCfg,
                                          DMA_Peripheral_T        peripheral,
                                          bool                    continous,
                                          DMA_Xfer                xfer ); /* May be NULL. */

OR use more generic scatter/gather interface.


EMSTATUS       DMA_memcpy2D         ( DMA_Channel             channel,
                                      void*                   src,
                                      void*                   dest,
                                      DMA_2D_T                desc2D,
                                      DMA_Xfer                xfer ); /* May be NULL. */



EMSTATUS       DMA_xferAttributeGet ( DMA_Xfer                xfer,
                                      DMA_XferAttribute_T     attr,
                                      void*                   value);
OR
EMSTATUS       DMA_xferStatusGet    ( DMA_Xfer                xfer,
                                      DMA_XferStatus_T*       status);

EMSTATUS       DMA_xferProgressGet  ( DMA_Xfer                xfer,
                                      uint32_t*               bytesXfered);
OR
EMSTATUS       DMA_xferedBytes      ( DMA_Xfer                xfer,
                                      uint32_t*               bytesXfered);



EMSTATUS       DMA_xferAttributeSet ( DMA_Xfer                xfer,
                                      DMA_XferAttribute_T     attr,
                                      void*                   value);
OR
EMSTATUS       DMA_callbackSet      ( DMA_Xfer                xfer.
                                      DMA_Callback_Desc_T*    callbackDesc);
etc.


#endif
