NAME := lib_camera_arducam

$(NAME)_SOURCES :=  arducam.c    \
                    drivers/arducam_drivers.c \
                    drivers/ov2640_driver.c

GLOBAL_INCLUDES := .
$(NAME)_AUTO_PROTOTYPE := 1