/**
 * UARTDRV test configuration for USART<->LEUART test
 */

/**
 * Interface 0 pin config
 */
#if defined(_LEUART_ROUTELOC0_MASK) // Platform 2
  #define TEST_UART_I0_DEVICE     (LEUART0)
  #define TEST_UART_I0_LOC        (_LEUART_ROUTELOC0_TXLOC_LOC11) , (_LEUART_ROUTELOC0_RXLOC_LOC11)
  #define TEST_UART_I0_CTS_PORT   (gpioPortC) /* CTS port number */
  #define TEST_UART_I0_CTS_PIN    (9)         /* CTS pin number  */
  #define TEST_UART_I0_RTS_PORT   (gpioPortC) /* RTS port number */
  #define TEST_UART_I0_RTS_PIN    (8)         /* RTS pin number  */
#else // Platform 1
  #if defined(EFM32GG_DK3750) || defined(EFM32LG_DK3650) || defined(EFM32WG_DK3850)
    // Location on DK that is not occupied by other test loops
    #define TEST_UART_I0_DEVICE   (LEUART1)
    #define TEST_UART_I0_LOC      (_LEUART_ROUTE_LOCATION_LOC1)
    #define TEST_UART_I0_CTS_PORT (gpioPortA)                   /* CTS port number */
    #define TEST_UART_I0_CTS_PIN  (4)                           /* CTS pin number  */
    #define TEST_UART_I0_RTS_PORT (gpioPortA)                   /* RTS port number */
    #define TEST_UART_I0_RTS_PIN  (7)                           /* RTS pin number  */
  #else
    // Location that exists on all devices
    // TODO: Set up LEUART <-> USART test for STKs
    #define TEST_UART_I0_DEVICE   (LEUART0)
    #define TEST_UART_I0_LOC      (_LEUART_ROUTE_LOCATION_LOC0)
    #define TEST_UART_I0_CTS_PORT (gpioPortD)                   /* CTS port number */
    #define TEST_UART_I0_CTS_PIN  (6)                           /* CTS pin number  */
    #define TEST_UART_I0_RTS_PORT (gpioPortD)                   /* RTS port number */
    #define TEST_UART_I0_RTS_PIN  (7)                           /* RTS pin number  */
  #endif
#endif

/**
 * Interface 1 pin config
 */
#if defined(_USART_ROUTELOC0_MASK)
  #if defined(EFR32MG1_BRD4150A)    /* MG1 dual band 868 */    \
     || defined(EFR32MG1_BRD4150B)  /* MG1 dual band 915 */    \
     || defined(EFR32MG1_BRD4151A)  /* MG1 2.4 20 dBm */       \
     || defined(EFR32MG1_BRD4153A)  /* MG1 2.4 13 dBm */       \
     || defined(EFR32MG12_BRD4156A) /* MG12 dual band QFN64 */
    #define TEST_UART_I1_DEVICE    (USART0)
    #define TEST_UART_I1_LOC       (_USART_ROUTELOC0_TXLOC_LOC1) , (_USART_ROUTELOC0_RXLOC_LOC31)
    #define TEST_UART_I1_CTS_PORT  (gpioPortC)     /* CTS port number */
    #define TEST_UART_I1_CTS_PIN   (11)            /* CTS pin number  */
    #define TEST_UART_I1_CTS_LOC   (_USART_ROUTELOC1_CTSLOC_LOC12)  /* CTS location #12 => PC11 */
    #define TEST_UART_I1_RTS_PORT  (gpioPortC)     /* RTS port number */
    #define TEST_UART_I1_RTS_PIN   (10)            /* RTS pin number  */
    #define TEST_UART_I1_RTS_LOC   (_USART_ROUTELOC1_RTSLOC_LOC10)  /* RTS location #10 => PC10 */
  #elif defined(SLSTK3401A_EFM32PG)
    #define TEST_UART_I1_DEVICE    (USART0)
    #define TEST_UART_I1_LOC       (_USART_ROUTELOC0_TXLOC_LOC1) , (_USART_ROUTELOC0_RXLOC_LOC31)
    #define TEST_UART_I1_CTS_PORT  (gpioPortC)     /* CTS port number */
    #define TEST_UART_I1_CTS_PIN   (10)            /* CTS pin number  */
    #define TEST_UART_I1_CTS_LOC   (_USART_ROUTELOC1_CTSLOC_LOC11)  /* CTS location #11 => PC10 */
    #define TEST_UART_I1_RTS_PORT  (gpioPortC)     /* RTS port number */
    #define TEST_UART_I1_RTS_PIN   (11)            /* RTS pin number  */
    #define TEST_UART_I1_RTS_LOC   (_USART_ROUTELOC1_RTSLOC_LOC11)  /* RTS location #11 => PC11 */
  #elif defined(EFR32MG12_BRD4157B) /* MG12 dual band BGA120 */
    #define TEST_UART_I1_DEVICE    (USART2)
    #define TEST_UART_I1_LOC       (_USART_ROUTELOC0_TXLOC_LOC8) , (_USART_ROUTELOC0_RXLOC_LOC10)
    #define TEST_UART_I1_CTS_PORT  (gpioPortC)     /* CTS port number */
    #define TEST_UART_I1_CTS_PIN   (11)            /* CTS pin number  */
    #define TEST_UART_I1_CTS_LOC   (_USART_ROUTELOC1_CTSLOC_LOC12)  /* CTS location #12 => PC11 */
    #define TEST_UART_I1_RTS_PORT  (gpioPortC)     /* RTS port number */
    #define TEST_UART_I1_RTS_PIN   (10)            /* RTS pin number  */
    #define TEST_UART_I1_RTS_LOC   (_USART_ROUTELOC1_RTSLOC_LOC10)  /* RTS location #10 => PC10 */
  #endif
#elif defined(EFM32GG_DK3750) || defined(EFM32LG_DK3650) || defined(EFM32WG_DK3850)
  // DK: Can connect 2 LEUARTs on protoboard
  #define TEST_UART_I1_DEVICE     (USART2)
  #define TEST_UART_I1_LOC        (_USART_ROUTE_LOCATION_LOC1)    /* LEUART pin location number */
  #define TEST_UART_I1_CTS_PORT   (gpioPortB)                     /* CTS port number            */
  #define TEST_UART_I1_CTS_PIN    (2)                             /* CTS pin number             */
  #define TEST_UART_I1_RTS_PORT   (gpioPortB)                     /* RTS port number            */
  #define TEST_UART_I1_RTS_PIN    (5)                             /* RTS pin number             */
#else
  // TODO: Set up LEUART <-> USART test for STKs
  #undef TEST_UART_I1_DEVICE
#endif




/**
 * Interface 0 config
 */
#define TEST_UART_I0                                                                              \
{                                                                                                 \
  TEST_UART_I0_DEVICE,                                    /* USART port                   */      \
  115200,                                                 /* Baud rate                    */      \
  TEST_UART_I0_LOC,                                       /* USART location number        */      \
  (LEUART_Stopbits_TypeDef)LEUART_CTRL_STOPBITS_ONE,      /* Stop bits                    */      \
  (LEUART_Parity_TypeDef)LEUART_CTRL_PARITY_NONE,         /* Parity                       */      \
  uartdrvFlowControlHw,                                   /* Flow control                 */      \
  TEST_UART_I0_CTS_PORT,                                  /* CTS port number              */      \
  TEST_UART_I0_CTS_PIN,                                   /* CTS pin number               */      \
  TEST_UART_I0_RTS_PORT,                                  /* RTS port number              */      \
  TEST_UART_I0_RTS_PIN,                                   /* RTS pin number               */      \
  (UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueueI0,         /* RX operation queue           */      \
  (UARTDRV_Buffer_FifoQueue_t *)&txBufferQueueI0,         /* TX operation queue           */      \
}

#if defined(TEST_UART_I1_DEVICE)
/**
 * Interface 1 config
 */
#if defined(_USART_ROUTELOC0_MASK)
/**
 * Platform 2
 */
#define TEST_UART_I1                                                                              \
{                                                                                                 \
  TEST_UART_I1_DEVICE,                                    /* USART port                   */      \
  115200,                                                 /* Baud rate                    */      \
  TEST_UART_I1_LOC,                                       /* USART location number        */      \
  (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE,       /* Stop bits                    */      \
  (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,          /* Parity                       */      \
  (USART_OVS_TypeDef)USART_CTRL_OVS_X16,                  /* Oversampling mode            */      \
  false,                                                  /* Majority vote disable        */      \
  uartdrvFlowControlHw,                                   /* Flow control                 */      \
  TEST_UART_I1_CTS_PORT,                                  /* CTS port number              */      \
  TEST_UART_I1_CTS_PIN,                                   /* CTS pin number               */      \
  TEST_UART_I1_RTS_PORT,                                  /* RTS port number              */      \
  TEST_UART_I1_RTS_PIN,                                   /* RTS pin number               */      \
  (UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueueI1,         /* RX operation queue           */      \
  (UARTDRV_Buffer_FifoQueue_t *)&txBufferQueueI1,         /* TX operation queue           */      \
  TEST_UART_I1_CTS_LOC,                                   /* CTS location                 */      \
  TEST_UART_I1_RTS_LOC                                    /* RTS location                 */      \
}
#elif defined(USART_CTRL_MVDIS)
/**
 * Platform 1 (GG)
 */
#define TEST_UART_I1                                                                              \
{                                                                                                 \
  TEST_UART_I1_DEVICE,                                    /* USART port                   */      \
  115200,                                                 /* Baud rate                    */      \
  TEST_UART_I1_LOC,                                       /* USART location number        */      \
  (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE,       /* Stop bits                    */      \
  (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,          /* Parity                       */      \
  (USART_OVS_TypeDef)USART_CTRL_OVS_X16,                  /* Oversampling mode            */      \
  false,                                                  /* Majority vote disable        */      \
  uartdrvFlowControlHw,                                   /* Flow control                 */      \
  TEST_UART_I1_CTS_PORT,                                  /* CTS port number              */      \
  TEST_UART_I1_CTS_PIN,                                   /* CTS pin number               */      \
  TEST_UART_I1_RTS_PORT,                                  /* RTS port number              */      \
  TEST_UART_I1_RTS_PIN,                                   /* RTS pin number               */      \
  (UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueueI1,         /* RX operation queue           */      \
  (UARTDRV_Buffer_FifoQueue_t *)&txBufferQueueI1,         /* TX operation queue           */      \
}
#else
/**
 * Platform 1 (G)
 */
#define TEST_UART_I1                                                                              \
{                                                                                                 \
  TEST_UART_I1_DEVICE,                                    /* USART port                   */      \
  115200,                                                 /* Baud rate                    */      \
  TEST_UART_I1_LOC,                                       /* USART location number        */      \
  (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE,       /* Stop bits                    */      \
  (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,          /* Parity                       */      \
  (USART_OVS_TypeDef)USART_CTRL_OVS_X16,                  /* Oversampling mode            */      \
  uartdrvFlowControlHw,                                   /* Flow control                 */      \
  TEST_UART_I1_CTS_PORT,                                  /* CTS port number              */      \
  TEST_UART_I1_CTS_PIN,                                   /* CTS pin number               */      \
  TEST_UART_I1_RTS_PORT,                                  /* RTS port number              */      \
  TEST_UART_I1_RTS_PIN,                                   /* RTS pin number               */      \
  (UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueueI1,         /* RX operation queue           */      \
  (UARTDRV_Buffer_FifoQueue_t *)&txBufferQueueI1          /* TX operation queue           */      \
}
#endif
#endif
