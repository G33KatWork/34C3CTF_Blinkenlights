#Define toolchain config
TOOLCHAIN_TARGET := arm-none-eabi-

#Define used JTAG adapter here, included files for different adapters are located in /build/jtag_adapters
JTAG_ADAPTER := stlink_v2

#enter global defines here
GLOBAL_DEFINES :=

#default firmware to flash/debug using debug or upload targets
DEFAULT_FIRMWARE := LED
