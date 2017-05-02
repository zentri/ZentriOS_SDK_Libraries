NAME := atmel_mcu_drivers


$(NAME)_GLOBAL_INCLUDES := . \
                    common/boards \
                    common/services/clock \
                    common/services/ioport \
                    common/utils \
                    sam/drivers/adc \
                    sam/drivers/crccu \
                    sam/drivers/dacc \
                    sam/drivers/efc \
                    sam/drivers/gpbr \
                    sam/drivers/matrix \
                    sam/drivers/pdc \
                    sam/drivers/pio \
                    sam/drivers/pmc \
                    sam/drivers/pwm \
                    sam/drivers/rstc \
                    sam/drivers/rtc \
                    sam/drivers/rtt \
                    sam/drivers/spi \
                    sam/drivers/supc \
                    sam/drivers/tc \
                    sam/drivers/twi \
                    sam/drivers/uart \
                    sam/drivers/usart \
                    sam/drivers/wdt \
                    sam/utils \
                    sam/utils/cmsis/sam4s/include \
                    sam/utils/cmsis/sam4s/source/templates \
                    sam/utils/header_files \
                    sam/utils/preprocessor \
                    thirdparty/CMSIS/Include