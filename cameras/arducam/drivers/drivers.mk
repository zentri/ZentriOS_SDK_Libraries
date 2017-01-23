NAME := arducam_drivers


$(NAME)_SOURCES := arducam_drivers.c
$(NAME)_INCLUDES := .
$(NAME)_AUTO_PROTOTYPE := 1

ifneq ($(filter DRIVER_CAMERA_OV2640,$(PROCESSED_SDK_DEFINES)),)
$(NAME)_COMPONENTS += libraries/cameras/arducam/drivers/ov2640
endif