#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "FreeRTOS_IO.h"
#include "FreeRTOS_CLI.h"

/*-----------------------------------------------------------*/
/* Create the cli function */
/* This function implements the behaviour of a command, so must have the correct
prototype. */
static BaseType_t prvTaskStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *const pcHeader = "Task          State  Priority  Stack	#\n************************************************\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Generate a table of task stats. */
	strcpy( pcWriteBuffer, pcHeader );
	// vTaskList( pcWriteBuffer + strlen( pcHeader ) );

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

/* Structure that defines the "task-stats" command line command. */
static const CLI_Command_Definition_t xTaskStats =
{
	"task-stats", /* The command string to type. */
	"\ntask-stats:\n Displays a table showing the state of each FreeRTOS task\n\n",
	prvTaskStatsCommand, /* The function to run. */
	0 /* No parameters are expected. */
};
/*-----------------------------------------------------------*/

static portBASE_TYPE prvThreeParameterEchoCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
char *pcParameterString;
portBASE_TYPE xParameterStringLength, xReturn;
static portBASE_TYPE xParameterNumber = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( xParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf( ( char * ) pcWriteBuffer, "The three parameters were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		xParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameterString = ( char * ) FreeRTOS_CLIGetParameter
									(
										pcCommandString,		/* The command string itself. */
										xParameterNumber,		/* Return the next parameter. */
										&xParameterStringLength	/* Store the parameter string length. */
									);

		/* Sanity check something was returned. */
		configASSERT( pcParameterString );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
		sprintf( ( char * ) pcWriteBuffer, "%d: ", xParameterNumber );
		strncat( ( char * ) pcWriteBuffer, ( const char * ) pcParameterString, xParameterStringLength );
		strncat( ( char * ) pcWriteBuffer, "\r\n", strlen( "\r\n" ) );

		/* If this is the last of the three parameters then there are no more
		strings to return after this one. */
		if( xParameterNumber == 3L )
		{
			/* If this is the last of the three parameters then there are no more
			strings to return after this one. */
			xReturn = pdFALSE;
			xParameterNumber = 0L;
		}
		else
		{
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			xParameterNumber++;
		}
	}

	return xReturn;
}

/* Structure that defines the "echo_3_parameters" command line command.  This
takes exactly three parameters that the command simply echos back one at a
time. */
static const CLI_Command_Definition_t prvThreeParameterEchoCommandDefinition =
{
	( const char * const ) "echo-3-parameters",
	( const char * const ) "echo-3-parameters <param1> <param2> <param3>:\r\n Expects three parameters, echos each in turn\r\n\r\n",
	prvThreeParameterEchoCommand, /* The function to run. */
	3 /* Three parameters are expected, which can take any value. */
};
/*-----------------------------------------------------------*/

static portBASE_TYPE prvMultiParameterEchoCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
char *pcParameterString;
portBASE_TYPE xParameterStringLength, xReturn;
static portBASE_TYPE xParameterNumber = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( xParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf( ( char * ) pcWriteBuffer, "The parameters were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		xParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameterString = ( char * ) FreeRTOS_CLIGetParameter
									(
										pcCommandString,		/* The command string itself. */
										xParameterNumber,		/* Return the next parameter. */
										&xParameterStringLength	/* Store the parameter string length. */
									);

		if( pcParameterString != NULL )
		{
			/* Return the parameter string. */
			memset( pcWriteBuffer, 0x00, xWriteBufferLen );
			sprintf( ( char * ) pcWriteBuffer, "%d: ", xParameterNumber );
			strncat( ( char * ) pcWriteBuffer, ( const char * ) pcParameterString, xParameterStringLength );
			strncat( ( char * ) pcWriteBuffer, "\r\n", strlen( "\r\n" ) );

			/* There might be more parameters to return after this one. */
			xReturn = pdTRUE;
			xParameterNumber++;
		}
		else
		{
			/* No more parameters were found.  Make sure the write buffer does
			not contain a valid string. */
			pcWriteBuffer[ 0 ] = 0x00;

			/* No more data to return. */
			xReturn = pdFALSE;

			/* Start over the next time this command is executed. */
			xParameterNumber = 0;
		}
	}

	return xReturn;
}

/* Structure that defines the "echo_parameters" command line command.  This
takes a variable number of parameters that the command simply echos back one at
a time. */
static const CLI_Command_Definition_t prvMultiParameterEchoCommandDefinition =
{
	( const char * const ) "echo-parameters",
	( const char * const ) "echo-parameters <...>:\r\n Take variable number of parameters, echos each in turn\r\n\r\n",
	prvMultiParameterEchoCommand, /* The function to run. */
	-1 /* The user can enter any number of commands. */
};

/*-----------------------------------------------------------*/
/*
 * Holds the handle of the task created by the create-task command.
 */
static TaskHandle_t xCreatedTaskHandle = NULL;

void vOutputString( const uint8_t * const pucMessage )
{
	/* Obtaining the write mutex prevents strings output using this function
	from corrupting strings output by the command interpreter task (and visa
	versa).  It does not, however, prevent complete strings output using this
	function intermingling with complete strings output from the command
	interpreter as the command interpreter only holds the mutex on an output
	string by output string basis. */

	Peripheral_Descriptor_t xUART = FreeRTOS_open("/dev/uart", 0);

	if( xUART != NULL )
	{
		// if( FreeRTOS_ioctl( xUART, ioctlOBTAIN_WRITE_MUTEX, cmd500ms ) == pdPASS )
		{
			FreeRTOS_write( xUART, pucMessage, strlen( ( char * ) pucMessage ) );
		}
	}
}

static void prvCreatedTask( void *pvParameters )
{
int32_t lParameterValue;
static uint8_t pucLocalBuffer[ 60 ];
void vOutputString( const uint8_t * const pucMessage );

	/* Cast the parameter to an appropriate type. */
	lParameterValue = ( int32_t ) pvParameters;

	memset( ( void * ) pucLocalBuffer, 0x00, sizeof( pucLocalBuffer ) );
	sprintf( ( char * ) pucLocalBuffer, "Created task running.  Received parameter %d\r\n\r\n", ( long ) lParameterValue );
	vOutputString( pucLocalBuffer );

	for( ;; )
	{
		vTaskDelay( portMAX_DELAY );
	}
}
/*-----------------------------------------------------------*/
static portBASE_TYPE prvCreateTaskCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
char *pcParameterString;
portBASE_TYPE xParameterStringLength;
static const char *pcSuccessMessage = ( char * ) "Task created\r\n";
static const char *pcFailureMessage = ( char * ) "Task not created\r\n";
static const char *pcTaskAlreadyCreatedMessage = ( char * ) "The task has already been created. Execute the delete-task command first.\r\n";
int32_t lParameterValue;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Obtain the parameter string. */
	pcParameterString = ( char * ) FreeRTOS_CLIGetParameter
								(
									pcCommandString,		/* The command string itself. */
									1,						/* Return the first parameter. */
									&xParameterStringLength	/* Store the parameter string length. */
								);

	/* Turn the parameter into a number. */
	lParameterValue = ( int32_t ) atol( ( const char * ) pcParameterString );

	/* Attempt to create the task. */
	if( xCreatedTaskHandle != NULL )
	{
		strcpy( ( char * ) pcWriteBuffer, ( const char * ) pcTaskAlreadyCreatedMessage );
	}
	else
	{
		if( xTaskCreate( prvCreatedTask, ( const char * ) "Created", configMINIMAL_STACK_SIZE,  ( void * ) lParameterValue, tskIDLE_PRIORITY, &xCreatedTaskHandle ) == pdPASS )
		{
			strcpy( ( char * ) pcWriteBuffer, ( const char * ) pcSuccessMessage );
		}
		else
		{
			strcpy( ( char * ) pcWriteBuffer, ( const char * ) pcFailureMessage );
		}
	}

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

/* Structure that defines the "create-task" command line command.  This takes a
single parameter that is passed into a newly created task.  The task then
periodically writes to the console.  The parameter must be a numerical value. */
static const CLI_Command_Definition_t prvCreateTaskCommandDefinition =
{
	( const char * const ) "create-task",
	( const char * const ) "create-task <param>:\r\n Creates a new task that periodically writes the parameter to the CLI output\r\n\r\n",
	prvCreateTaskCommand, /* The function to run. */
	1 /* A single parameter should be entered. */
};
/*-----------------------------------------------------------*/

static portBASE_TYPE prvDeleteTaskCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
static const char *pcSuccessMessage = ( char * ) "Task deleted\r\n";
static const char *pcFailureMessage = ( char * ) "The task was not running.  Execute the create-task command first.\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* See if the task is running. */
	if( xCreatedTaskHandle != NULL )
	{
		vTaskDelete( xCreatedTaskHandle );
		xCreatedTaskHandle = NULL;
		strcpy( ( char * ) pcWriteBuffer, ( const char * ) pcSuccessMessage );
	}
	else
	{
		strcpy( ( char * ) pcWriteBuffer, ( const char * ) pcFailureMessage );
	}

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

/* Structure that defines the "delete-task" command line command.  This deletes
the task that was previously created using the "create-command" command. */
static const CLI_Command_Definition_t prvDeleteTaskCommandDefinition =
{
	( const char * const ) "delete-task",
	( const char * const ) "delete-task:\r\n Deletes the task created by the create-task command\r\n\r\n",
	prvDeleteTaskCommand, /* The function to run. */
	0 /* A single parameter should be entered. */
};

/*-----------------------------------------------------------*/
void vRegisterCLICommands( void )
{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand( &xTaskStats );
	FreeRTOS_CLIRegisterCommand( &prvThreeParameterEchoCommandDefinition );
	FreeRTOS_CLIRegisterCommand( &prvMultiParameterEchoCommandDefinition );
	FreeRTOS_CLIRegisterCommand( &prvCreateTaskCommandDefinition );
	FreeRTOS_CLIRegisterCommand( &prvDeleteTaskCommandDefinition );
}