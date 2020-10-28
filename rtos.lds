OUTPUT_ARCH( "riscv" )
ENTRY( _start )


SECTIONS
{
  . = 0xffffffffc0000000;
  PROVIDE(rtos_base = .);
  .text : {
    *(.text._start)
    *(.text)
  }
  . = ALIGN(0x1000);
  .rodata :
  {
    *(.rdata)
    *(.rodata)
  }

  .preinit_array  :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  }

  .init_array     :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  }

  .fini_array     :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);
  }

  .data : { 
  *(.data) 
  PROVIDE( __global_pointer$ = . + 0x800 );
  }

  .bss : { *(.bss) }
  . = ALIGN(0x1000);
  .stack : {
    __stack_bottom = .;
    . += 0x8000;
    __stack_top = .;
    _sp = .;
    __freertos_irq_stack_top = .;
  }

  _end = .;
}
