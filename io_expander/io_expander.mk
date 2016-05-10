NAME := lib_io_expander


$(NAME)_SOURCES := io_expander.c

GLOBAL_INCLUDES := .

$(NAME)_AUTO_PROTOTYPE := 1

ifneq ($(filter DRIVER_TCA6416,$(PROCESSED_SDK_DEFINES)),)
GLOBAL_DEFINES += IPEXPANDER_LIB_TCA6414
$(NAME)_COMPONENTS += drivers/io_expanders/tca6416
endif

ifneq ($(filter DRIVER_ADP5589,$(PROCESSED_SDK_DEFINES)),)
#error
GLOBAL_DEFINES += IPEXPANDER_LIB_ADP5589
$(NAME)_COMPONENTS += drivers/io_expanders/adp5589
endif