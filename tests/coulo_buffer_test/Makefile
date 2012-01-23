
TOP_LEVEL := $(shell readlink -f ./$(shell git rev-parse --show-cdup))
LIB_MAPLE_HOME :=  $(TOP_LEVEL)/libmaple

PATH := $(PATH):$(TOP_LEVEL)/arm/bin

BOARD := coulostat

# For standard Leaflabs board these are in makefile.inc
MCU := STM32F103RB
PRODUCT_ID := 0003
ERROR_LED_PORT := GPIOA
ERROR_LED_PIN  := 5
DENSITY := STM32_MEDIUM_DENSITY
FLASH_SIZE := 131072
SRAM_SIZE := 20480


include $(TOP_LEVEL)/support/make/makefile.inc
