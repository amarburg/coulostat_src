
TOP_LEVEL := $(shell readlink -f ./$(shell git rev-parse --show-cdup))
LIB_MAPLE_HOME :=  $(TOP_LEVEL)/libmaple

PATH := $(PATH):$(TOP_LEVEL)/arm/bin

include $(TOP_LEVEL)/support/make/makefile.inc
