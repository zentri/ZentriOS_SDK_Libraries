NAME := lib_camera_arducam

$(NAME)_SOURCES :=  arducam.c

GLOBAL_INCLUDES := .
$(NAME)_AUTO_PROTOTYPE := 1

$(NAME)_COMPONENTS := libraries/cameras/arducam/drivers