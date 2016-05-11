NAME := drivers_thermometer_pmodgyro

$(NAME)_SOURCES := sensor_api.c
                   
$(NAME)_INCLUDES := . \
                    ../../gyroscopes/pmodgyro

$(NAME)_COMPONENTS := libraries/drivers/gyroscopes/pmodgyro
