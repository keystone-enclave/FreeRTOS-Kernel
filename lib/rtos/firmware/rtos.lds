OUTPUT_ARCH( "riscv" )
ENTRY( setup_rtos )


SECTIONS
{
  . = 0x80200000;
  _rtos_base = .; 
  PROVIDE(rtos_base = .);
  .text : {
    *(.text.setup_rtos)
    *(.text.main)
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
  /*  KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors)) */
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
    . += 0x8000;
  }
  _rtos_end = .; 
  PROVIDE(rtos_end = .);
  . = ALIGN(0x1000);

  . += 0x8000;

  .receiver : {
    _receiver_start = .; 
    *(.receiver*)  
    . = ALIGN(0x1000);
    _receiver_end = .; 
  } 


 /* .rv8 : {
     _rv8_start = .; 
    *(.miniz*)  
    . = ALIGN(0x1000);
    _rv8_end = .; 
  } */

/*  .fibonacci : {
    _task_1_start = .; 
    *(.fibonacci*)  
    . = ALIGN(0x1000);
    _task_1_end = .; 
  }  

    .attest : {
    _task_2_start = .; 
    *(.attest*)  
    . = ALIGN(0x1000);
    _task_2_end = .; 
  }  */


  /*.agent : {
    _agent_start = .; 
    *(.agent*)  
    . = ALIGN(0x1000);
    _agent_end = .; 
  } */

  .shared_buffer : {
    _shared_buffer_start = .; 
    . = . + 4096; 
    . = ALIGN(0x1000);
    _shared_buffer_end = .; 
  } 

  .simulator : {
    _simulator_start = .; 
    *(.simulator*)  
    . = ALIGN(0x1000);
    _simulator_end = .; 
  } 

}
