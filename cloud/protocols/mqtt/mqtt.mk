NAME := lib_mqtt

$(NAME)_SOURCES :=  
GLOBAL_INCLUDES := .
$(NAME)_AUTO_PROTOTYPE := 1
$(NAME)_COMPONENTS := cloud/protocols/mqtt/mqtt_wrapper
$(NAME)_PREBUILT_LIBRARY := $(NAME)-$(BUILD_TYPE).a