// /* FreeRTOS includes. */
// #include "FreeRTOS.h"

// #include "FreeRTOS_CLI.h"

// BaseType_t FreeRTOS_CLIRegisterFunction( const char * taskName, const char * helpString, const void * prvFunction, size_t elfSize )
// {
//     CLI_Command_Definition_t * pxNewCommandItem = ( CLI_Command_Definition_t * ) pvPortMalloc( sizeof( CLI_Command_Definition_t ) );
//     pxNewCommandItem->pcCommand = taskName;
//     pxNewCommandItem->pcHelpString = helpString;
//     pxNewCommandItem->pxCommandInterpreter = prvFunction;
//     pxNewCommandItem->cExpectedNumberOfParameters = 0;
//     pxNewCommandItem->elfsize = elfSize;

//     return FreeRTOS_CLIRegisterCommand(pxNewCommandItem);
// }