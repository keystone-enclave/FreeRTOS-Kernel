
#ifndef _ENCLAVE_H
#define _ENCLAVE_H


uintptr_t xTaskCreateEnclave(uintptr_t start, uintptr_t size,
                            const char * const pcName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask);


#endif