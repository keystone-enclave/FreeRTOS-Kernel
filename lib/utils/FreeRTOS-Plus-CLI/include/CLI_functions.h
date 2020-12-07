#include <stdlib.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "portmacro.h"

#ifndef CLI_FUNCTIONS_H
#define CLI_FUNCTIONS_H

/* The structure that defines command line commands.  A command line command
should be defined by declaring a const structure of this type. */
typedef struct xFunction
{
	char * pcFunction;				/* The function name which open will use. */
	void * pxFunctionCallback;			/* A pointer to the callback task function. */
	size_t enclaveSize; 					/* 0 if it is a normal task, the size of the secure task otherwise. */
} Function_Definition_t;


BaseType_t FreeRTOS_run( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

BaseType_t FreeRTOS_RegisterFunction( char * pcFunction, void * pxFunction, size_t enclaveSize );

#endif /* CLI_FUNCTIONS_H */