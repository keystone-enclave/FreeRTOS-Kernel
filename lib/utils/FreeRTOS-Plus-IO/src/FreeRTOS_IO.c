
/* Standard includes. */
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Utils includes. */
#include "FreeRTOS_IO.h"


typedef struct xIO_INPUT_LIST
{
	const IO_Device_Definition_t *pxDeviceDefinition;
	struct xIO_INPUT_LIST *pxNext;
} IO_Definition_List_Item_t;

/* The definition of the list of devices.  Devices that are registered are
added to this list. */
static IO_Definition_List_Item_t xRegisteredDevices =
{
	NULL,	/* The first command in the list is always the help command, defined in this file. */
	NULL			/* The next pointer is initialised to NULL, as there are no other registered commands yet. */
};

/*-----------------------------------------------------------*/

Peripheral_Descriptor_t 
FreeRTOS_open( const char *pcPath, const uint32_t ulFlags )
{
	const char *pcRegisteredDeviceString;
	size_t xDeviceStringLength;

	/* Search for the device string in the list of registered devices. */
	for( IO_Definition_List_Item_t * pxDevice = &xRegisteredDevices; pxDevice != NULL; pxDevice = pxDevice->pxNext )
	{	
		if (!pxDevice->pxDeviceDefinition)
			continue;
		pcRegisteredDeviceString = pxDevice->pxDeviceDefinition->pcDevice;
		xDeviceStringLength = strlen( pcRegisteredDeviceString );

		/* To ensure the string lengths match exactly, so as not to pick up
		a sub-string of a longer device name, check the byte after the expected
		end of the string is either the end of the string or a space before
		a parameter. */
		if( strncmp( pcPath, pcRegisteredDeviceString, xDeviceStringLength ) == 0 )
		{
			if( ( pcPath[ xDeviceStringLength ] == ' ' ) || ( pcPath[ xDeviceStringLength ] == 0x00 ) )
			{
				return pxDevice->pxDeviceDefinition;
			}
		}
	}
	return NULL;
}

size_t 
FreeRTOS_read( Peripheral_Descriptor_t const pxPeripheral, void * const pvBuffer, const size_t xBytes )
{
	if (pxPeripheral)
    	return pxPeripheral->pxReadCallback(pvBuffer, xBytes);
	return 0;
}

size_t 
FreeRTOS_write( Peripheral_Descriptor_t const pxPeripheral, const void *pvBuffer, const size_t xBytes )
{
	if (pxPeripheral)
    	return pxPeripheral->pxWriteCallback(pvBuffer, xBytes);
	return 0;

}

BaseType_t
FreeRTOS_ioctl( Peripheral_Descriptor_t const xPeripheral, uint32_t ulRequest, void *pvValue )
{
	if (xPeripheral)
    	return xPeripheral->pxIoctlCallback(ulRequest, pvValue);
	return pdFAIL;
}

/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_IORegisterDevice( const IO_Device_Definition_t * const pxDeviceToRegister )
{
static IO_Definition_List_Item_t *pxLastDeviceInList = &xRegisteredDevices;
IO_Definition_List_Item_t *pxNewListItem;
BaseType_t xReturn = pdFAIL;

	/* Check the parameter is not NULL. */
	configASSERT( pxDeviceToRegister );

	/* Create a new list item that will reference the command being registered. */
	pxNewListItem = ( IO_Definition_List_Item_t * ) pvPortMalloc( sizeof( IO_Definition_List_Item_t ) );
	configASSERT( pxNewListItem );

	if( pxNewListItem != NULL )
	{
		taskENTER_CRITICAL();
		{
			/* Reference the device being registered from the newly created
			list item. */
			pxNewListItem->pxDeviceDefinition = pxDeviceToRegister;

			/* The new list item will get added to the end of the list, so
			pxNext has nowhere to point. */
			pxNewListItem->pxNext = NULL;

			/* Add the newly created list item to the end of the already existing
			list. */
			pxLastDeviceInList->pxNext = pxNewListItem;

			/* Set the end of list marker to the new list item. */
			pxLastDeviceInList = pxNewListItem;
		}
		taskEXIT_CRITICAL();

		xReturn = pdPASS;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/