NAME := drivers_thermometer_hih6130

$(NAME)_SOURCES := sensor_api.c
                   
$(NAME)_INCLUDES := . \
                    ../../hygrometers/hih6130

$(NAME)_COMPONENTS := libraries/drivers/hygrometers/hih6130
