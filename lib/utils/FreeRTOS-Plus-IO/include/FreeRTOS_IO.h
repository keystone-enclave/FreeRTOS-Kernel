#include <stdlib.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "portmacro.h"

/* The peripherals the IO library can interface to. */
typedef enum
{
	eUART_TYPE = 0,
} Peripheral_Types_t;

/* Peripheral handles are void * for data hiding purposes. */
typedef const void * Peripheral_Descriptor_t;

Peripheral_Descriptor_t FreeRTOS_open( const int8_t *pcPath, const uint32_t ulFlags );

size_t FreeRTOS_read( Peripheral_Descriptor_t const pxPeripheral, void * const pvBuffer, const size_t xBytes );

size_t FreeRTOS_write( Peripheral_Descriptor_t const pxPeripheral, const void *pvBuffer, const size_t xBytes );

BaseType_t FreeRTOS_ioctl( Peripheral_Descriptor_t const xPeripheral, uint32_t ulRequest, void *pvValue );