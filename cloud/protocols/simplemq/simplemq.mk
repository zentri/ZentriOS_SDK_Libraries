NAME := lib_cloud_protocols_simplemq



$(NAME)_SOURCES := SMQClient.c \
                   arch/zos/seZOS.c

GLOBAL_INCLUDES := . \
                    arch/zos
                    
$(NAME)_AUTO_PROTOTYPE := 1