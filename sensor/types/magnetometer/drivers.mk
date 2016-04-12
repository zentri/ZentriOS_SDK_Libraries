

ifneq ($(filter DRIVER_MAGNETOMETER_MPU9250,$(PROCESSED_SDK_DEFINES)),)
GLOBAL_DEFINES += SENSOR_LIB_MAGNETOMETER
$(NAME)_COMPONENTS += libraries/drivers/magnetometers/mpu9250
endif

ifneq ($(filter DRIVER_MAGNETOMETER_FXOS8700CQ,$(PROCESSED_SDK_DEFINES)),)
GLOBAL_DEFINES += SENSOR_LIB_MAGNETOMETER
$(NAME)_COMPONENTS += libraries/drivers/magnetometers/fxos8700cq
endif

ifneq ($(filter SENSOR_LIB_MAGNETOMETER,$(GLOBAL_DEFINES) $(PROCESSED_SDK_DEFINES)),)
GLOBAL_INCLUDES += types/magnetometer
endif