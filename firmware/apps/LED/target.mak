# Target file name.
TARGET = LED

# List C source files here.
CCSOURCES = main.c interrupts.c clocks.c uart.c ws2811.c newlib.c font.c hsv.c

# List C++ source files here.
CXXSOURCES =

# List Files to be assembled here
ASOURCES = 

# C compiler flags
CFLAGS  = -std=gnu99 -ggdb -O0 -Werror -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -fdata-sections -ffunction-sections

# C++ compiler flags
CXXFLAGS =

# GAS flags
ASFLAGS = -fdata-sections -ffunction-sections

# Linker flags
LDFLAGS := -lnosys -lc -Wl,--gc-sections
LDFLAGS += -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += -Wl,-T$(ROOT)/misc/linker/f4/STM32F429ZI_FLASH.ld,-Map,$(SELF_DIR)/$(TARGET).map

# Additional include paths to consider
INCLUDES = $(SELF_DIR)/inc \
           $(ROOT)/libs/libdummy/inc

# Middlewares to add
# source files are built with this target (specific for this target, so local header files are consideres)
# include paths are added automatically
MIDDLEWARES = cmsis stm32f4_device_429xx stm32f4_hal

# Additional local static libs to link against
LIBS = $(BINARY-libdummy)

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DSTM32F429xx -DUSE_HAL_DRIVER

include $(ROOT)/build/targets/executable.mak
