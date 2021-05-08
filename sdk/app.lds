OUTPUT_ARCH( "riscv" )

SECTIONS 
{
  . = 0x00001000;
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

  .data : { 
    *(.data) 
  }

  . = ALIGN(0x1000);

  .stack : {
      *(.stack_zone*)
  }

  . = ALIGN(0x1000);

    .malloc_zone :
  {
    __malloc_start = .;
    . = . + (400352);
    __malloc_zone_stop = .;
  }
  
  . = ALIGN(0x1000);

  .debug : { *(.debug) }

  _end = .;
}