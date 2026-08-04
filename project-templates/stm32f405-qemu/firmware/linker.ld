ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

STACK_SIZE = 16K;
HEAP_SIZE = 96K;
__stack_top = ORIGIN(RAM) + LENGTH(RAM);
__stack_bottom = __stack_top - STACK_SIZE;

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    . = ALIGN(4);
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
    *(.data*)
    . = ALIGN(8);
    _data_end = .;
  } > RAM AT > FLASH
  _data_load = LOADADDR(.data);

  .bss (NOLOAD) :
  {
    . = ALIGN(8);
    _bss_start = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(8);
    _bss_end = .;
  } > RAM

  .heap (NOLOAD) :
  {
    . = ALIGN(8);
    __heap_start = .;
    . = . + HEAP_SIZE;
    __heap_end = .;
  } > RAM

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

ASSERT(__heap_end <= __stack_bottom, "STM32 RAM regions overlap")
