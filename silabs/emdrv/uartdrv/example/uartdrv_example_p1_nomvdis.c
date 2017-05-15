#include "uartdrv.h"

// Define receive/transmit operation queues
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, rxBufferQueue);
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, txBufferQueue);

// Configuration for USART1, location 1
#define MY_UART                                     \
{                                                   \
  USART1,                                           \
  115200,                                           \
  _USART_ROUTE_LOCATION_LOC1,                       \
  usartStopbits1,                                   \
  usartNoParity,                                    \
  usartOVS16,                                       \
  uartdrvFlowControlHw,                             \
  gpioPortD,                                        \
  2,                                                \
  gpioPortD,                                        \
  3,                                                \
  (UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueue,     \
  (UARTDRV_Buffer_FifoQueue_t *)&txBufferQueue      \
}

UARTDRV_HandleData_t handleData;
UARTDRV_Handle_t handle = &handleData;
uint8_t buffer[64];

void callback(UARTDRV_Handle_t handle,
              Ecode_t transferStatus,
              uint8_t *data,
              UARTDRV_Count_t transferCount)
{
  (void)handle;
  (void)transferStatus;
  (void)data;
  (void)transferCount;
}

void uartdrv_example(void)
{
  // Initialize driver handle
  UARTDRV_InitUart_t initData = MY_UART;
  UARTDRV_InitUart(handle, &initData);

  // Transmit data using a non-blocking transmit function
  UARTDRV_Transmit(handle, buffer, 64, callback);
}
