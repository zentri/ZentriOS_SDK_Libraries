NAME := lib_sensor


GLOBAL_INCLUDES := .
$(NAME)_SOURCES := sensor.c \
                   internal/hygrometer.c \
                   internal/thermometer.c




include libraries/sensor/types/accelerometer/drivers.mk
include libraries/sensor/types/gyroscope/drivers.mk
include libraries/sensor/types/hygrometer/drivers.mk
include libraries/sensor/types/magnetometer/drivers.mk
include libraries/sensor/types/thermometer/drivers.mk
include libraries/sensor/types/airquality/drivers.mk
include libraries/sensor/types/light/drivers.mk
include libraries/sensor/types/capacitativebuttons/drivers.mk