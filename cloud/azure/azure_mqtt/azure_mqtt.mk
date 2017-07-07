NAME := lib_cloud_azure_mqtt



GLOBAL_INCLUDES := .

$(NAME)_SOURCES := azure_mqtt.c


$(NAME)_COMPONENTS := cloud/azure/azure_mqtt/azure_iot_sdk_mqtt_port