NAME := lib_io_expander


$(NAME)_SOURCES := io_expander.c

GLOBAL_INCLUDES := .

$(NAME)_COMPONENTS := drivers/io_expanders/tca6416
$(NAME)_AUTO_PROTOTYPE := 1