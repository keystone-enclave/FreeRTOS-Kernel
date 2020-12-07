
/* Standard includes. */
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "enclave.h"

/* Utils includes. */
#include "CLI_functions.h"
#include "FreeRTOS_CLI.h"


typedef struct xFunction_INPUT_LIST
{
	const Function_Definition_t *pxFunctionDefinition;
	struct xFunction_INPUT_LIST *pxNext;
} Function_Definition_List_Item_t;

/* The definition of the list of Functions.  Functions that are registered are
added to this list. */
static Function_Definition_List_Item_t xRegisteredFunctions =
{
	NULL,	/* The first command in the list is always the help command, defined in this file. */
	NULL			/* The next pointer is initialised to NULL, as there are no other registered commands yet. */
};

static const CLI_Command_Definition_t prvRunTaskCommandDefinition =
{
	( const char * const ) "run",
	( const char * const ) "run:\r\n run's the specified task\r\n\r\n",
	FreeRTOS_run, /* The function to run. */
	1 /* A single parameter should be entered. */
};

/*-----------------------------------------------------------*/

BaseType_t 
FreeRTOS_run( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char *pcRegisteredFunctionString;
	size_t xFunctionStringLength;

	/* Search for the Function string in the list of registered Functions. */
	for( Function_Definition_List_Item_t * pxFunction = &xRegisteredFunctions; pxFunction != NULL; pxFunction = pxFunction->pxNext )
	{	
		if (!pxFunction->pxFunctionDefinition)
			continue;
		pcRegisteredFunctionString = pxFunction->pxFunctionDefinition->pcFunction;
		xFunctionStringLength = strlen( pcRegisteredFunctionString );

		/* To ensure the string lengths match exactly, so as not to pick up
		a sub-string of a longer Function name, check the byte after the expected
		end of the string is either the end of the string or a space before
		a parameter. */
		if( strncmp( pcWriteBuffer, pcRegisteredFunctionString, xFunctionStringLength ) == 0 )
		{
			if( ( pcWriteBuffer[ xFunctionStringLength ] == ' ' ) || ( pcWriteBuffer[ xFunctionStringLength ] == 0x00 ) )
			{
				if (pxFunction->pxFunctionDefinition->enclaveSize) {
   					xTaskCreateEnclave((uintptr_t) pxFunction->pxFunctionDefinition->pxFunctionCallback, pxFunction->pxFunctionDefinition->enclaveSize, pxFunction->pxFunctionDefinition->pcFunction, 2, NULL); 
				} else {
					xTaskCreate(pxFunction->pxFunctionDefinition->pxFunctionCallback, pxFunction->pxFunctionDefinition->pcFunction, configMINIMAL_STACK_SIZE, NULL, 2, NULL);
				}
				return pdPASS;
			}
		}
	}
	return pdFAIL;
}

/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_RegisterFunction( char * pcFunction, void * pxFunction, size_t enclaveSize )
{
static Function_Definition_List_Item_t *pxLastFunctionInList = &xRegisteredFunctions;
Function_Definition_List_Item_t *pxNewListItem;
BaseType_t xReturn = pdFAIL;
static int hasBeenInitialized = 0;
if (!hasBeenInitialized) {
	FreeRTOS_CLIRegisterCommand( &prvRunTaskCommandDefinition );
	hasBeenInitialized = 1;
	printf("RUN INIT\n");
}

	Function_Definition_t * pxFunctionToRegister = (Function_Definition_t *) pvPortMalloc( sizeof( Function_Definition_t ) );
	pxFunctionToRegister->pcFunction = pcFunction;
	pxFunctionToRegister->pxFunctionCallback = pxFunction;
	pxFunctionToRegister->enclaveSize = enclaveSize;

	/* Check the parameter is not NULL. */
	configASSERT( pxFunctionToRegister );

	/* Create a new list item that will reference the command being registered. */
	pxNewListItem = ( Function_Definition_List_Item_t * ) pvPortMalloc( sizeof( Function_Definition_List_Item_t ) );
	configASSERT( pxNewListItem );

	if( pxNewListItem != NULL )
	{
		taskENTER_CRITICAL();
		{
			/* Reference the Function being registered from the newly created
			list item. */
			pxNewListItem->pxFunctionDefinition = pxFunctionToRegister;

			/* The new list item will get added to the end of the list, so
			pxNext has nowhere to point. */
			pxNewListItem->pxNext = NULL;

			/* Add the newly created list item to the end of the already existing
			list. */
			pxLastFunctionInList->pxNext = pxNewListItem;

			/* Set the end of list marker to the new list item. */
			pxLastFunctionInList = pxNewListItem;
		}
		taskEXIT_CRITICAL();

		xReturn = pdPASS;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/