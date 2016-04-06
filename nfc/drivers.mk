

ifneq ($(filter DRIVER_NFC_NT3H1X,$(PROCESSED_SDK_DEFINES)),)
$(NAME)_COMPONENTS += libraries/drivers/nfc/nt3h1x
endif

