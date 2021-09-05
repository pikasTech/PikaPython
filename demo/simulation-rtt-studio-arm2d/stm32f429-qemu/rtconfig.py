import os

# toolchains options
ARCH = 'arm'
CPU = 'cortex-m4'
CROSS_TOOL = 'gcc'

# cross_tool provides the cross compiler
# EXEC_PATH is the compiler execute path, for example, CodeSourcery, Keil MDK, IAR
PLATFORM = 'gcc'
EXEC_PATH = ''

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

PREFIX = 'arm-none-eabi-'
CC = PREFIX + 'gcc'
AS = PREFIX + 'gcc'
AR = PREFIX + 'ar'
CXX = PREFIX + 'g++'
LINK = PREFIX + 'gcc'
TARGET_EXT = 'elf'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'
DEVICE = ''
CFLAGS = ''
AFLAGS = ''
LFLAGS = '-T linkscripts//STM32F429ZI//link.lds'
CPATH = ''
LPATH = ''
CXXFLAGS = ''
POST_ACTION = ''
