#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := pikascript_bl602
PROJECT_PATH := $(abspath .)
PROJECT_BOARD := evb
export PROJECT_PATH PROJECT_BOARD
#CONFIG_TOOLPREFIX :=

-include ./proj_config.mk

ifeq ($(origin BL60X_SDK_PATH), undefined)
BL60X_SDK_PATH_GUESS ?= $(shell pwd)
BL60X_SDK_PATH ?= $(BL60X_SDK_PATH_GUESS)/..
endif

COMPONENTS_BLSYS   := bltime blfdt blmtd bloop loopadc looprt loopset
COMPONENTS_VFS     := romfs

INCLUDE_COMPONENTS += freertos_riscv_ram bl602 bl602_std newlibc hosal mbedtls_lts lwip vfs yloop utils cli blog blog_testc coredump
INCLUDE_COMPONENTS += $(COMPONENTS_NETWORK)
INCLUDE_COMPONENTS += $(COMPONENTS_BLSYS)
INCLUDE_COMPONENTS += $(COMPONENTS_VFS)

EXTRA_COMPONENT_DIRS += $(PROJECT_PATH)/boot
EXTRA_COMPONENT_DIRS += $(PROJECT_PATH)/pikascript

INCLUDE_COMPONENTS += boot
INCLUDE_COMPONENTS += pikascript

CPPFLAGS += -Wno-format

include $(BL60X_SDK_PATH)/make_scripts_riscv/project.mk
