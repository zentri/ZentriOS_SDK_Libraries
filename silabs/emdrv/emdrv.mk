NAME := Silicon_Labs_emdrv_library

$(NAME)_REFERENCED_COMPONENTS := libraries.silabs.emlib

#$(NAME)_SOURCES := gpiointerrupt/src/gpiointerrupt.c \
                   nvm/src/nvm.c \
                   nvm/src/nvm_hal.c \
                   rtcdrv/src/rtcdriver.c \
                   sleep/src/sleep.c \
                   spidrv/src/spidrv.c \
                   uartdrv/src/uartdrv.c \
                   ustimer/src/ustimer.c
        
$(NAME)_GLOBAL_INCLUDES := dmadrv/inc \
                           dmadrv/config \
                           nvm/inc \
                           nvm/config \
                           rtcdrv/inc \
                           rtcdrv/config \
                           sleep/inc \
                           spidrv/inc \
                           spidrv/config \
                           tempdrv/inc \
                           uartdrv/inc \
                           uartdrv/config \
                           ustimer/inc \
                           gpiointerrupt/inc \
                           common/inc

$(NAME)_INCLUDES := dmadrv/test \
                    tempdrv/config \
                    ustimer/config
                    
$(NAME)_SUPPORTED_FEATURES := efm_emu \
                              DMA_driver \
                              RTC_driver \
                              NVMHAL_driver \
                              GPIO_interrupt

$(NAME)_efm_emu_SOURCES        := tempdrv/src/tempdrv.c
$(NAME)_DMA_driver_SOURCES     := dmadrv/src/dmadrv.c
$(NAME)_RTC_driver_SOURCES     := rtcdrv/src/rtcdriver.c
$(NAME)_NVMHAL_driver_SOURCES  := nvm/src/nvm_hal.c
$(NAME)_GPIO_interrupt_SOURCES := gpiointerrupt/src/gpiointerrupt.c
