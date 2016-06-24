########################################
# This file was automatially generated
#

# The name of the project component
NAME := STM32F4xx_Libraries

# The name of the project
PROJECT_NAME := STM32F4xx_Libraries

# The platform of the target device
PLATFORM := 

#$(NAME)_PREBUILT_LIBRARY := $(NAME)-$(BUILD_TYPE).a

# Enable/disable automatic generation of static function prototypes in project source code
$(NAME)_AUTO_PROTOTYPE :=

# Automatically include source files found in the project directory
$(NAME)_AUTO_SOURCES := 

# List of files to include in the project build. Paths relative to the project's directory
#$(NAME)_SOURCES := 

# List of regular expressions to use for including source files into the build
$(NAME)_AUTO_INCLUDE := 

# List of regular expressions to use for excluding source files into the build
$(NAME)_AUTO_EXCLUDE := 

# List of referenced libraries
$(NAME)_COMPONENTS := 

# Pre-processor symbols for this project component only (not referenced libraries)
$(NAME)_DEFINES := 

# Pre-processor symbols for the entire build (this project and all referenced libraries)
GLOBAL_DEFINES := CORE_M4

# Includes file paths for project component only (not referenced libraries)
$(NAME)_INCLUDES := 

# Includes file paths for the entire build (this project and all referenced libraries)
GLOBAL_INCLUDES := . \
                    inc \

$(NAME)_SOURCES := \
                   src/misc.c \
                   src/stm32f4xx_adc.c \
                   src/stm32f4xx_can.c \
                   src/stm32f4xx_crc.c \
                   src/stm32f4xx_dac.c \
                   src/stm32f4xx_dbgmcu.c \
                   src/stm32f4xx_dma.c \
                   src/stm32f4xx_exti.c \
                   src/stm32f4xx_flash.c \
                   src/stm32f4xx_fsmc.c \
                   src/stm32f4xx_gpio.c \
                   src/stm32f4xx_rng.c \
                   src/stm32f4xx_i2c.c \
                   src/stm32f4xx_iwdg.c \
                   src/stm32f4xx_pwr.c \
                   src/stm32f4xx_rcc.c \
                   src/stm32f4xx_rtc.c \
                   src/stm32f4xx_sdio.c \
                   src/stm32f4xx_spi.c \
                   src/stm32f4xx_syscfg.c \
                   src/stm32f4xx_tim.c \
                   src/stm32f4xx_usart.c \
                   src/stm32f4xx_wwdg.c

# C compiler flags for project component only (not referenced libraries)
$(NAME)_CFLAGS := 

# C compiler flags for the entire build (this project and all referenced libraries)
GLOBAL_CFLAGS := 

# Assembler flags for project component only (not referenced libraries)
$(NAME)_ASMFLAGS := 

# Assembler flags for the entire build (this project and all referenced libraries)
GLOBAL_ASMFLAGS := 

# Linker flags for the entire build (this project and all referenced libraries)
GLOBAL_LDFLAGS := 

# Path to resource manifest .json file (path is relative to project directory)
$(NAME)_RESOURCE_MANIFEST := 

# Paths to app settings .ini files  (paths are relative to project directory)
$(NAME)_APP_SETTINGS := 

