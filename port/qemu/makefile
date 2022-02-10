# DECLARATIONS

# Directory where QEMU_STM32 ARM executable is located - used for running
# program in emulator.
QEMU_ARM_DIR ?= ../qemu_stm32/arm-softmmu/

# OpenOCD interface file used for programming/debugging the micronctroller
OPENOCD_INTERFACE ?= interface/olimex-arm-usb-tiny-h.cfg

# Declare command line tools - assume these are in the path
CC	  = arm-none-eabi-gcc
LD	  = arm-none-eabi-ld
AS	  = arm-none-eabi-as
CP	  = arm-none-eabi-objcopy
OD	  = arm-none-eabi-objdump

# Declare command line flags
CORE_CFLAGS = -I./ -I$(CORE_SRC) -I$(DEVICE_SRC) -I$(STD_PERIPH)/inc -Idemos/common -fno-common -O0 -g -mcpu=cortex-m3 -mthumb 
CFLAGS  = $(CORE_CFLAGS) -c 
CFLAGS_LINK = -Wl,-Tdemos/main.ld -nostartfiles $(CORE_CFLAGS)
ASFLAGS = -mcpu=cortex-m3 -mthumb -g
LDFLAGS = -Tdemos/main.ld
CPFLAGS = -Obinary
ODFLAGS	= -S

# Declare library source paths
SRC = $(realpath .)
CORE_SRC = $(SRC)/libraries/CMSIS/CM3/CoreSupport
DEVICE_SRC = $(SRC)/libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
STD_PERIPH = $(SRC)/libraries/STM32F10x_StdPeriph_Driver
STD_PERIPH_SRC = $(STD_PERIPH)/src

# List common and system library source files
# (i.e. for accessing STM32/Cortex M3 hardware) 
COMMON_FILES = $(CORE_SRC)/core_cm3.c
COMMON_FILES += $(DEVICE_SRC)/system_stm32f10x.c
COMMON_FILES += $(DEVICE_SRC)/startup/gcc_ride7/startup_stm32f10x_md.s
COMMON_FILES += demos/common/stm32_p103.c
COMMON_FILES += demos/common/myprintk.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_rcc.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_gpio.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_usart.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_exti.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_adc.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_tim.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_rtc.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_dac.c
COMMON_FILES += $(STD_PERIPH_SRC)/misc.c

# List all demos
DEMOS = printf_demo

# List all demo folders
DEMO_FOLDERS = $(addprefix demos/,$(DEMOS))

# List FreeRTOS resources
FREE_RTOS_SRC = $(SRC)/libraries/FreeRTOS
FREE_RTOS_SRC_FILES = $(FREE_RTOS_SRC)/croutine.c $(FREE_RTOS_SRC)/list.c $(FREE_RTOS_SRC)/queue.c $(FREE_RTOS_SRC)/tasks.c $(FREE_RTOS_SRC)/croutine.c $(FREE_RTOS_SRC)/stream_buffer.c $(FREE_RTOS_SRC)/portable/GCC/ARM_CM3/port.c
FREE_RTOS_INC = $(FREE_RTOS_SRC)/include/
FREE_RTOS_PORT_INC = $(FREE_RTOS_SRC)/portable/GCC/ARM_CM3/

# List path to demo build output files
OUTPUT_FILES = $(addsuffix /main,$(DEMO_FOLDERS))
ELF_FILES	= $(addsuffix .elf,$(OUTPUT_FILES))
LIST_FILES	= $(addsuffix .list,$(OUTPUT_FILES))
BIN_FILES	= $(addsuffix .bin,$(OUTPUT_FILES))

# Declare target names for each demo
ALL_TARGETS = $(addsuffix _ALL,$(DEMOS))
PROG_TARGETS = $(addsuffix _PROG,$(DEMOS))

QEMU_RUN_TARGETS = $(addsuffix _QEMURUN,$(DEMOS))
QEMU_RUN_PTY_TARGETS = $(addsuffix _QEMURUN_PTY,$(DEMOS))
QEMU_RUN_TEL_TARGETS = $(addsuffix _QEMURUN_TEL,$(DEMOS))
QEMU_DBG_TARGETS = $(addsuffix _QEMUDBG,$(DEMOS))
QEMU_DBG_PTY_TARGETS = $(addsuffix _QEMUDBG_PTY,$(DEMOS))
QEMU_DBG_TEL_TARGETS = $(addsuffix _QEMUDBG_TEL,$(DEMOS))



# TARGETS - See README for descriptions of the targets

# Generic targets
.PHONY: clean $(ALL_TARGETS) $(PROG_TARGETS) openocd_dbg

all: $(ALL_TARGETS)

clean:
	find . -type f -name "*.o" -exec rm {} \;
	find . -type f -name "*.elf" -exec rm {} \;
	find . -type f -name "*.bin" -exec rm {} \;
	find . -type f -name "*.list" -exec rm {} \; 

# Compile targets (builds all output files)
$(ALL_TARGETS): %_ALL : demos/%/main.elf demos/%/main.bin demos/%/main.list

# Targets to program a microntroller using OpenOCD
$(PROG_TARGETS): %_PROG : %_ALL
	-killall -q openocd
	openocd -f $(OPENOCD_INTERFACE) -f openocd/openocd_stm32_p103.cfg -c "program_flash demos/$*/main.bin"

# Target to launch OpenOCD - by default, OpenOCD creates a GDB server at port 3333.
DBG:
	-killall -q openocd
	openocd -f $(OPENOCD_INTERFACE) -f openocd/openocd_stm32_p103.cfg -c "init_stm32"
	
# QEMU run targets
$(QEMU_RUN_TARGETS): %_QEMURUN : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-p103 -kernel demos/$*/main.bin

$(QEMU_RUN_PTY_TARGETS): %_QEMURUN_PTY : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-p103 -kernel demos/$*/main.bin -serial pty
	
$(QEMU_RUN_TEL_TARGETS): %_QEMURUN_TEL : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-p103 -kernel demos/$*/main.bin -serial tcp::7777,server

# QEMU debug targets
$(QEMU_DBG_TARGETS): %_QEMUDBG : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-p103 -gdb tcp::3333 -S -kernel demos/$*/main.bin

$(QEMU_DBG_PTY_TARGETS): %_QEMUDBG_PTY : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-p103 -gdb tcp::3333 -S -kernel demos/$*/main.bin -serial pty
	
$(QEMU_DBG_TEL_TARGETS): %_QEMUDBG_TEL : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-p103 -gdb tcp::3333 -S -kernel demos/$*/main.bin -serial tcp::7777,server
	
# Note: Use this command to run QEMU in low-level debug mode:
#    qemu-system-arm -cpu cortex-m3 -M stm32-p103 -nographic -singlestep -kernel main.bin -d in_asm,out_asm,exec,cpu,int,op,op_opt

# Compile targets to build individual files
$(LIST_FILES): %.list : %.elf
	$(OD) $(ODFLAGS) $< > $@

$(BIN_FILES): %.bin : %.elf
	$(CP) $(CPFLAGS) $< $@

# Targets to build individual demos
demos/blink_flash/main.elf: demos/blink_flash/main.c
demos/blink_flash/main.elf: $(STD_PERIPH_SRC)/stm32f10x_rcc.c
demos/blink_flash/main.elf: $(STD_PERIPH_SRC)/stm32f10x_gpio.c
demos/blink_flash/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/blink_flash/ -o $@ $^


demos/blink_flash_asm/main.elf: demos/blink_flash_asm/main.o
	$(LD) $(LDFLAGS) -o $@ $<

demos/blink_flash_asm/main.o: demos/blink_flash_asm/main.s
	 $(AS) $(ASFLAGS) -o $@ $<


demos/adc_single/main.elf: demos/adc_single/main.c
demos/adc_single/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/adc_single/ -o $@ $^


demos/button/main.elf: demos/button/main.c
demos/button/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/button/ -o $@ $^


demos/button_int/main.elf: demos/button_int/main.c
demos/button_int/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/button_int/ -o $@ $^


demos/button_int_infinite/main.elf: demos/button_int/main.c
demos/button_int_infinite/main.elf: $(COMMON_FILES)
	$(CC) -DDO_NOT_CLEAR_IT_PENDING_FLAG $(CFLAGS_LINK) -Idemos/button_int/ -o $@ $^


demos/c_mem_model/main.o: demos/c_mem_model/main.c
	$(CC) $(CFLAGS_LINK) -Idemos/c_mem_model/ -c -o $@ $^

demos/c_mem_model/main.elf: demos/c_mem_model/main.o
demos/c_mem_model/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/c_mem_model/ -o $@ $^


demos/freertos_singlethread/main.elf: demos/freertos_singlethread/main.c
demos/freertos_singlethread/main.elf: $(COMMON_FILES)
demos/freertos_singlethread/main.elf: $(FREE_RTOS_SRC_FILES)
demos/freertos_singlethread/main.elf: $(FREE_RTOS_SRC)/portable/MemMang/heap_1.c
	$(CC) $(CFLAGS_LINK) -Idemos/freertos_singlethread/ -I$(FREE_RTOS_INC) -I$(FREE_RTOS_PORT_INC) -o $@ $^


demos/freertos_multithread/main.elf: demos/freertos_multithread/main.c
demos/freertos_multithread/main.elf: $(COMMON_FILES)
demos/freertos_multithread/main.elf: $(FREE_RTOS_SRC_FILES)
demos/freertos_multithread/main.elf: $(FREE_RTOS_SRC)/portable/MemMang/heap_1.c
	$(CC) $(CFLAGS_LINK) -Idemos/freertos_multithread/ -I$(FREE_RTOS_INC) -I$(FREE_RTOS_PORT_INC) -o $@ $^

demos/freertos_streambuffer/main.elf: demos/freertos_streambuffer/main.c
demos/freertos_streambuffer/main.elf: $(COMMON_FILES)
demos/freertos_streambuffer/main.elf: $(FREE_RTOS_SRC_FILES)
demos/freertos_streambuffer/main.elf: $(FREE_RTOS_SRC)/portable/MemMang/heap_1.c
	$(CC) $(CFLAGS_LINK) -Idemos/freertos_streambuffer/ -I$(FREE_RTOS_INC) -I$(FREE_RTOS_PORT_INC) -o $@ $^

demos/qemu_test/main.elf: demos/qemu_test/main.c
demos/qemu_test/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/qemu_test/ -o $@ $^


demos/software_int/main.elf: demos/software_int/main.c
demos/software_int/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/software_int/ -o $@ $^


demos/stkalign/main.elf: demos/stkalign/main.c
demos/stkalign/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/stkalign/ -o $@ $^


demos/systick/main.elf: demos/systick/main.c
demos/systick/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/systick/ -o $@ $^


demos/timer/main.elf: demos/timer/main.c
demos/timer/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/timer/ -o $@ $^


demos/uart_echo/main.elf: demos/uart_echo/main.c
demos/uart_echo/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/uart_echo/ -o $@ $^

demos/printf_demo/main.elf: demos/printf_demo/main.c
demos/printf_demo/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/printf_demo/ -o $@ $^

demos/uart_repeat_write/main.elf: demos/uart_repeat_write/main.c
demos/uart_repeat_write/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/uart_repeat_write/ -o $@ $^


demos/uart_echo_int/main.elf: demos/uart_echo_int/main.c
demos/uart_echo_int/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/uart_echo_int/ -o $@ $^

demos/uart_repeat_write_int/main.elf: demos/uart_repeat_write_int/main.c
demos/uart_repeat_write_int/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/uart_repeat_write_int/ -o $@ $^

demos/dac/main.elf: demos/dac/main.c
demos/dac/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/dac/ -o $@ $^

demos/rtc/main.elf: demos/rtc/main.c
demos/rtc/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idemos/rtc/ -o $@ $^

demos/freertos_semaphore1/main.elf: demos/freertos_semaphore1/main.c
demos/freertos_semaphore1/main.elf: $(COMMON_FILES)
demos/freertos_semaphore1/main.elf: $(FREE_RTOS_SRC_FILES)
demos/freertos_semaphore1/main.elf: $(FREE_RTOS_SRC)/portable/MemMang/heap_1.c
	$(CC) $(CFLAGS_LINK) -Idemos/freertos_semaphore1/ -I$(FREE_RTOS_INC) -I$(FREE_RTOS_PORT_INC) -o $@ $^

demos/freertos_cycletask/main.elf: demos/freertos_cycletask/main.c
demos/freertos_cycletask/main.elf: $(COMMON_FILES)
demos/freertos_cycletask/main.elf: $(FREE_RTOS_SRC_FILES)
demos/freertos_cycletask/main.elf: $(FREE_RTOS_SRC)/portable/MemMang/heap_1.c
	$(CC) $(CFLAGS_LINK) -Idemos/freertos_cycletask/ -I$(FREE_RTOS_INC) -I$(FREE_RTOS_PORT_INC) -o $@ $^
