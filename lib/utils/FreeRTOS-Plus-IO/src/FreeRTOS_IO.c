#include <stdio.h>

#include "FreeRTOS_IO.h"

Peripheral_Descriptor_t 
FreeRTOS_open( const int8_t *pcPath, const uint32_t ulFlags )
{
    return NULL;
}

size_t 
FreeRTOS_read( Peripheral_Descriptor_t const pxPeripheral, void * const pvBuffer, const size_t xBytes )
{
    // Lazy impl just to grab stdin.
    char * buf = pvBuffer;
    size_t i = 0;
    for (; i < xBytes; i++) {
        do 
        {
            int c = getchar();
            if (c != -1) {
                *buf = (char) c;
                break;
            }
        } while (1);
    }
    return i;
}

size_t 
FreeRTOS_write( Peripheral_Descriptor_t const pxPeripheral, const void *pvBuffer, const size_t xBytes )
{
    // Lazy impl just to set stdout.
    const char * buf = pvBuffer;
    size_t i = 0;
    for (; i < xBytes; i++) {
        putchar(*(buf + i));
    }
    return i;

}

BaseType_t
FreeRTOS_ioctl( Peripheral_Descriptor_t const xPeripheral, uint32_t ulRequest, void *pvValue )
{
    return 0;
}