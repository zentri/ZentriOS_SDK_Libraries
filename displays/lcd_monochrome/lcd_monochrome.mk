NAME := lib_displays_lcd_monochrome



GLOBAL_INCLUDES := .
GLOBAL_DEFINES := LIBRARY_LCD_MONOCHROME        \
                  LIBRARY_LCD                   \
                  LCD_FONT8X8

$(NAME)_COMPONENTS := drivers/displays/st7565s \
                      displays/util/fonts
