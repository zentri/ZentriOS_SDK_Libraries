NAME := mcu_silabs_em32_drivers

$(NAME)_GLOBAL_INCLUDES := inc


$(NAME)_SUPPORTED_FEATURES := EFM32GG \
                              EFR32MG12P

$(NAME)_EFM32GG_REQUIRED_COMPONENTS    := $(COMPONENT_PATH).MCUs.EFM32GG
$(NAME)_EFR32MG12P_REQUIRED_COMPONENTS := $(COMPONENT_PATH).MCUs.EFR32MG12P

$(NAME)_SOURCES := src/em_acmp.c \
                   src/em_adc.c \
                   src/em_aes.c \
                   src/em_assert.c \
                   src/em_burtc.c \
                   src/em_can.c \
                   src/em_core.c \
                   src/em_cryotimer.c \
                   src/em_crypto.c \
                   src/em_csen.c \
                   src/em_dac.c \
                   src/em_dbg.c \
                   src/em_dma.c \
                   src/em_ebi.c \
                   src/em_emu.c \
                   src/em_eth.c \
                   src/em_gpcrc.c \
                   src/em_gpio.c \
                   src/em_i2c.c \
                   src/em_idac.c \
                   src/em_lcd.c \
                   src/em_ldma.c \
                   src/em_lesense.c \
                   src/em_letimer.c \
                   src/em_leuart.c \
                   src/em_mpu.c \
                   src/em_msc.c \
                   src/em_opamp.c \
                   src/em_pcnt.c \
                   src/em_prs.c \
                   src/em_qspi.c \
                   src/em_rmu.c \
                   src/em_rtc.c \
                   src/em_rtcc.c \
                   src/em_system.c \
                   src/em_timer.c \
                   src/em_usart.c \
                   src/em_vcmp.c \
                   src/em_vdac.c \
                   src/em_wdog.c
