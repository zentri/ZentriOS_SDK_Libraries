NAME := lib_s2c_zap


GLOBAL_INCLUDES := .

$(NAME)_AUTO_PROTOTYPE := 1

$(NAME)_SOURCES  := s2c_api.c \
                    s2c_cloud.c \
                    s2c_commands.c \
                    s2c_file_commands.c \
                    s2c_mobile.c \
                    s2c_network.c \
                    s2c_streams.c


