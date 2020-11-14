/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "stack_macros.h"
#include "regs.h"
#include "syscall.h"
#include "elf.h"

uintptr_t xTaskCreateEnclave(uintptr_t start, uintptr_t size,
                              const char *const enclaveName,
                              UBaseType_t uxPriority,
                              TaskHandle_t *const pxCreatedTask)
{

    elf_t elf;
    /* preparation for libelf */
    if (elf_newFile((void *) start, size, &elf))
    {
        return 0;
    }

    struct register_sbi_arg register_args;
    register_args.pc = (uintptr_t) ((char *) start + elf_getEntryPoint(&elf));

    size_t i;
    elf_getSectionNamed(&elf, ".stack", &i);
    register_args.sp = (uintptr_t) ((char *) start + elf_getSectionOffset(&elf, i)); 
    register_args.stack_size = elf_getSectionSize(&elf, i);
 
    register_args.base = start;
    register_args.size = size;
    register_args.enclave = 1; 

    return _create_task_enclave(&register_args, enclaveName, uxPriority, pxCreatedTask);
}