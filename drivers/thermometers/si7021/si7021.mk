NAME := drivers_thermometer_si7021

$(NAME)_SOURCES := sensor_api.c 
                   
$(NAME)_INCLUDES := . \
                    ../../hygrometers/si7021

$(NAME)_COMPONENTS := libraries/drivers/hygrometers/si7021