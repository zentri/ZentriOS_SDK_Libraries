NAME := lib_displays_led_matrix8x8



$(NAME)_SOURCES := led_matrix8x8.c

GLOBAL_INCLUDES := .

$(NAME)_COMPONENTS := drivers/displays/ht16k33 \
                      displays/util/fonts
                      
$(NAME)_AUTO_PROTOTYPE := 1


GLOBAL_DEFINES := LEDMATRIX_FONT8X8