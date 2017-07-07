NAME := lib_cloud_azure_mqtt_sdk_port


GLOBAL_DEFINES := DONT_USE_UPLOADTOBLOB MINIMAL_LOGERROR






AZURE_SDK_ROOT_PATH := ../../azure-iot-sdk-c

AZURE_SDK_C_UTIL_PATH := $(AZURE_SDK_ROOT_PATH)/c-utility
AZURE_SDK_C_UTIL_INC  := $(AZURE_SDK_C_UTIL_PATH)/inc
AZURE_SDK_C_UTIL_SRC  := $(AZURE_SDK_C_UTIL_PATH)/src/map.c \
                         $(AZURE_SDK_C_UTIL_PATH)/src/buffer.c \
                         $(AZURE_SDK_C_UTIL_PATH)/src/constbuffer.c \
                         $(AZURE_SDK_C_UTIL_PATH)/src/crt_abstractions.c \
                         $(AZURE_SDK_C_UTIL_PATH)/src/strings.c \
                         $(AZURE_SDK_C_UTIL_PATH)/src/string_tokenizer.c \
                         $(AZURE_SDK_C_UTIL_PATH)/src/doublylinkedlist.c \
                         $(AZURE_SDK_C_UTIL_PATH)/src/singlylinkedlist.c \
                         $(AZURE_SDK_C_UTIL_PATH)/src/urlencode.c \
                         $(AZURE_SDK_C_UTIL_PATH)/src/sastoken.c

AZURE_SDK_UMQTT_PATH := $(AZURE_SDK_ROOT_PATH)/umqtt
AZURE_SDK_UMQTT_INC  := $(AZURE_SDK_UMQTT_PATH)/inc
AZURE_SDK_UMQTT_SRC  := $(AZURE_SDK_UMQTT_PATH)/src/mqtt_client.c \
                        $(AZURE_SDK_UMQTT_PATH)/src/mqtt_codec.c \
                        $(AZURE_SDK_UMQTT_PATH)/src/mqtt_message.c

AZURE_SDK_IOTHUB_CLIENT_PATH := $(AZURE_SDK_ROOT_PATH)/iothub_client
AZURE_SDK_IOTHUB_CLIENT_INC  := $(AZURE_SDK_IOTHUB_CLIENT_PATH)/inc
AZURE_SDK_IOTHUB_CLIENT_SRC  := $(AZURE_SDK_IOTHUB_CLIENT_PATH)/src/iothub_message.c \
                                $(AZURE_SDK_IOTHUB_CLIENT_PATH)/src/iothub_client_authorization.c


AZURE_SDK_INCLUDES := $(AZURE_SDK_C_UTIL_INC) \
                      $(AZURE_SDK_UMQTT_INC) \
                      $(AZURE_SDK_IOTHUB_CLIENT_INC)
                      
AZURE_SDK_SOURCES := $(AZURE_SDK_C_UTIL_SRC) \
                     $(AZURE_SDK_UMQTT_SRC) \
                     $(AZURE_SDK_IOTHUB_CLIENT_SRC)

GLOBAL_INCLUDES := $(AZURE_SDK_INCLUDES)

$(NAME)_INCLUDES := .

$(NAME)_SOURCES := iothub_client_LL.c \
                   iothubtransport_mqtt_common.c \
                   iothub_mqtt_internal.c \
                   tickcounter.c \
                   agenttime.c \
                   xio.c \
                   utils.c \
                   $(AZURE_SDK_SOURCES)
                   
                   
$(NAME)_CFLAGS := -Wno-unused-variable

