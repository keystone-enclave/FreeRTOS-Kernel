#include <stdlib.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "portmacro.h"

#ifndef FREERTOS_IO_H
#define FREERTOS_IO_H

/* The prototype to which callback functions used to process command line
commands must comply.  pcWriteBuffer is a buffer into which the output from
executing the command can be written, xWriteBufferLen is the length, in bytes of
the pcWriteBuffer buffer, and pcCommandString is the entire string as input by
the user (from which parameters can be extracted).*/
typedef size_t (*pdREAD_CALLBACK)( void * const pvBuffer, const size_t xBytes );
typedef size_t (*pdWRITE_CALLBACK)( const void *pvBuffer, const size_t xBytes );
typedef BaseType_t (*pdIOCTL_CALLBACK)( uint32_t ulRequest, void *pvValue );


/* The structure that defines command line commands.  A command line command
should be defined by declaring a const structure of this type. */
typedef struct xIO_DEVICE
{
	const char * const pcDevice;				/* The device name which open will use. */
	const pdREAD_CALLBACK pxReadCallback;	/* A pointer to the callback function that will perform a device read. */
	const pdWRITE_CALLBACK pxWriteCallback;	/* A pointer to the callback function that will perform a device read. */
	const pdIOCTL_CALLBACK pxIoctlCallback;	/* A pointer to the callback function that will perform a device read. */
} IO_Device_Definition_t;

/* Peripheral handles are void * for data hiding purposes. */
// typedef const void * Peripheral_Descriptor_t;
typedef const IO_Device_Definition_t * Peripheral_Descriptor_t;

Peripheral_Descriptor_t FreeRTOS_open( const char *pcPath, const uint32_t ulFlags );

size_t FreeRTOS_read( Peripheral_Descriptor_t const pxPeripheral, void * const pvBuffer, const size_t xBytes );

size_t FreeRTOS_write( Peripheral_Descriptor_t const pxPeripheral, const void *pvBuffer, const size_t xBytes );

BaseType_t FreeRTOS_ioctl( Peripheral_Descriptor_t const xPeripheral, uint32_t ulRequest, void *pvValue );

BaseType_t FreeRTOS_IORegisterDevice( const IO_Device_Definition_t * const pxDeviceToRegister );

#endif /* FREERTOS_IO_H */