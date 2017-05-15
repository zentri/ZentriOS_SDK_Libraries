/**
 * UARTDRV test configuration for LEUART<->LEUART test
 */

/**
 * Interface 0 pin config
 */
#if defined(_LEUART_ROUTELOC0_MASK) // Platform 2
  #define TEST_UART_I0_DEVICE     (LEUART0)
  #define TEST_UART_I0_LOC        (_LEUART_ROUTELOC0_TXLOC_LOC0) , (_LEUART_ROUTELOC0_RXLOC_LOC0)
  #define TEST_UART_I0_CTS_PORT   (gpioPortA) /* CTS port number */
  #define TEST_UART_I0_CTS_PIN    (3)         /* CTS pin number  */
  #define TEST_UART_I0_RTS_PORT   (gpioPortA) /* RTS port number */
  #define TEST_UART_I0_RTS_PIN    (4)         /* RTS pin number  */
#else // Platform 1
    #define TEST_UART_I0_DEVICE   (LEUART0)
    #define TEST_UART_I0_LOC      (_LEUART_ROUTE_LOCATION_LOC0)
    #define TEST_UART_I0_CTS_PORT (gpioPortD)                   /* CTS port number */
    #define TEST_UART_I0_CTS_PIN  (6)                           /* CTS pin number  */
    #define TEST_UART_I0_RTS_PORT (gpioPortD)                   /* RTS port number */
    #define TEST_UART_I0_RTS_PIN  (7)                           /* RTS pin number  */
#endif

/**
 * Interface 1 pin config
 */
#if defined(EFM32GG_DK3750) || defined(EFM32LG_DK3650) || defined(EFM32WG_DK3850)
  // DK: Can connect 2 LEUARTs on protoboard
  #define TEST_UART_I1_DEVICE     (LEUART1)
  #define TEST_UART_I1_LOC        (_LEUART_ROUTE_LOCATION_LOC0) /* USART pins location number */
  #define TEST_UART_I1_CTS_PORT   (gpioPortC)                   /* CTS port number */
  #define TEST_UART_I1_CTS_PIN    (8)                           /* CTS pin number  */
  #define TEST_UART_I1_RTS_PORT   (gpioPortC)                   /* RTS port number */
  #define TEST_UART_I1_RTS_PIN    (9)                           /* RTS pin number  */
#else
  // (W)STK: Cannot connect 2 LEUARTs with 4 pins
  // TODO: Maybe solder headers on kits in regression test?
  #undef TEST_UART_I1_DEVICE
#endif


#if defined(LEUART_USE_LFXO)
#define TEST_BAUDRATE 9600
#else
#define TEST_BAUDRATE 115200
#endif

/**
 * Interface 0 config
 */
#define TEST_UART_I0                                                                              \
{                                                                                                 \
  TEST_UART_I0_DEVICE,                                    /* USART port                   */      \
  TEST_BAUDRATE,                                          /* Baud rate                    */      \
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
#define TEST_UART_I1                                                                              \
{                                                                                                 \
  TEST_UART_I1_DEVICE,                                    /* USART port                   */      \
  TEST_BAUDRATE,                                          /* Baud rate                    */      \
  TEST_UART_I1_LOC,                                       /* USART location number        */      \
  (LEUART_Stopbits_TypeDef)LEUART_CTRL_STOPBITS_ONE,      /* Stop bits                    */      \
  (LEUART_Parity_TypeDef)LEUART_CTRL_PARITY_NONE,         /* Parity                       */      \
  uartdrvFlowControlHw,                                   /* Flow control                 */      \
  TEST_UART_I1_CTS_PORT,                                  /* CTS port number              */      \
  TEST_UART_I1_CTS_PIN,                                   /* CTS pin number               */      \
  TEST_UART_I1_RTS_PORT,                                  /* RTS port number              */      \
  TEST_UART_I1_RTS_PIN,                                   /* RTS pin number               */      \
  (UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueueI1,         /* RX operation queue           */      \
  (UARTDRV_Buffer_FifoQueue_t *)&txBufferQueueI1          /* TX operation queue           */      \
}
#endif
