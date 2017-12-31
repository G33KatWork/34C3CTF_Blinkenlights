DEVICES_F4 := 429xx

define DEVICE_INSTANCE_F4
TARGET = stm32f4_device_$(device)
CCSOURCES = Templates/system_stm32f4xx.c
ASOURCES = Templates/gcc/startup_stm32f$(device).s
INCLUDES =	$(ROOT)/middlewares/stm32f4_devices/Include
SRCDIR = Source
include $(ROOT)/build/targets/middleware.mak
endef

$(foreach device, $(DEVICES_F4), $(eval $(DEVICE_INSTANCE_F4)))
