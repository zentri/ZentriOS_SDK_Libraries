NAME := lib_nfc


GLOBAL_INCLUDES := . \
                   ndef
$(NAME)_SOURCES := nfc.c \
                   ndef/ndef.c \
                   nfc_driver_interface.c


include libraries/nfc/drivers.mk