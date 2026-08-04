ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 128K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

STACK_SIZE = 4K;
_flash_start = ORIGIN(FLASH);
__stack_top = ORIGIN(RAM) + LENGTH(RAM);
__stack_bottom = __stack_top - STACK_SIZE;

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(128);
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    . = ALIGN(4);
    __testcase_flash_start = .;
    *pikapython_program.c.obj(.text*)
    *pikapython_program.c.obj(.rodata*)
    . = ALIGN(4);
    __testcase_flash_end = .;
    *(.text*)
    *(.rodata*)
    KEEP(*(.init))
    KEEP(*(.fini))
    . = ALIGN(4);
  } > FLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH

  .ARM.exidx :
  {
    __exidx_start = .;
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    __exidx_end = .;
  } > FLASH

  .data :
  {
    . = ALIGN(8);
    _data_start = .;
    __testcase_data_start = .;
    *pikapython_program.c.obj(.data*)
    . = ALIGN(8);
    __testcase_data_end = .;
    *(.data*)
    . = ALIGN(8);
    _data_end = .;
  } > RAM AT > FLASH
  _data_load = LOADADDR(.data);
  _flash_image_end = LOADADDR(.data) + SIZEOF(.data);

  .bss (NOLOAD) :
  {
    . = ALIGN(8);
    _bss_start = .;
    __testcase_bss_start = .;
    *pikapython_program.c.obj(.bss*)
    . = ALIGN(8);
    __testcase_bss_end = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(8);
    _bss_end = .;
  } > RAM

  .heap (NOLOAD) :
  {
    . = ALIGN(8);
    __heap_start = .;
  } > RAM
  __heap_end = __stack_bottom;

  .stack __stack_bottom (NOLOAD) :
  {
    . = ALIGN(8);
    . = . + STACK_SIZE;
  } > RAM

  /DISCARD/ :
  {
    *(.comment*)
    *(.note*)
  }
}

ASSERT(__heap_start <= __heap_end,
       "STM32F103RB RAM is too small for static data and stack")
ASSERT(__stack_top <= ORIGIN(RAM) + LENGTH(RAM),
       "STM32F103RB stack exceeds RAM")
ASSERT(_flash_image_end <= ORIGIN(FLASH) + LENGTH(FLASH),
       "STM32F103RB Flash overflow")
