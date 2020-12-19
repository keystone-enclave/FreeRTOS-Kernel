#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "FreeRTOS_IO.h"
#include "FreeRTOS_CLI.h"

#define MAX_INPUT_LENGTH    50
#define MAX_OUTPUT_LENGTH   100

static const char * const pcWelcomeMessage =
  "FreeRTOS command server.\nType Help to view a list of registered commands.\n> ";

void vCommandConsoleTask( void *pvParameters )
{
Peripheral_Descriptor_t xConsole;
char cRxedChar = 0;
int8_t cInputIndex = 0;
BaseType_t xMoreDataToFollow;
/* The input and output buffers are declared static to keep them off the stack. */
static char pcOutputString[ MAX_OUTPUT_LENGTH ], pcInputString[ MAX_INPUT_LENGTH ];

    /* This code assumes the peripheral being used as the console has already
    been opened and configured, and is passed into the task as the task
    parameter.  Cast the task parameter to the correct type. */
    xConsole = ( Peripheral_Descriptor_t ) pvParameters;
    /* Send a welcome message to the user knows they are connected. */
    FreeRTOS_write( xConsole, pcWelcomeMessage, strlen( pcWelcomeMessage ) );


    for( ;; )
    {
        /* This implementation reads a single character at a time.  Wait in the
        Blocked state until a character is received. */
        FreeRTOS_read( xConsole, &cRxedChar, sizeof( cRxedChar ) );

        if( cRxedChar == '\n' || cRxedChar == '\r' )
        {
            /* A newline character was received, so the input command string is
            complete and can be processed.  Transmit a line separator, just to
            make the output easier to read. */
            FreeRTOS_write( xConsole, "\n", strlen( "\n" ) );

            /* The command interpreter is called repeatedly until it returns
            pdFALSE.  See the "Implementing a command" documentation for an
            exaplanation of why this is. */
            do
            {
                /* Send the command string to the command interpreter.  Any
                output generated by the command interpreter will be placed in the
                pcOutputString buffer. */
                xMoreDataToFollow = FreeRTOS_CLIProcessCommand
                              (
                                  pcInputString,   /* The command string.*/
                                  pcOutputString,  /* The output buffer. */
                                  MAX_OUTPUT_LENGTH/* The size of the output buffer. */
                              );

                /* Write the output generated by the command interpreter to the
                console. */
                FreeRTOS_write( xConsole, pcOutputString, strlen( pcOutputString ) );

            } while( xMoreDataToFollow != pdFALSE );

            /* All the strings generated by the input command have been sent.
            Processing of the command is complete.  Clear the input string ready
            to receive the next command. */
            cInputIndex = 0;
            memset( pcInputString, 0x00, MAX_INPUT_LENGTH );
            FreeRTOS_write( xConsole, "> ", strlen( "> " ) );
        }
        else
        {
            /* The if() clause performs the processing after a newline character
            is received.  This else clause performs the processing if any other
            character is received. */

            if( cRxedChar == '\r' )
            {
                /* Ignore carriage returns. */
            }
            else if( cRxedChar == 127 ) /* Used to be `\b`. */
            {
                /* Backspace was pressed.  Erase the last character in the input
                buffer - if there are any. */
                if( cInputIndex > 0 )
                {
                    cInputIndex--;
                    pcInputString[ cInputIndex ] = '\0';

                    /* Hacky solution to backspaces */
                    FreeRTOS_write( xConsole, "\b", strlen( "\b" ) );
                    cRxedChar = 127;
                    FreeRTOS_write( xConsole, &cRxedChar, strlen( &cRxedChar ) );
                    FreeRTOS_write( xConsole, "\b", strlen( "\b" ) );
                }
            }
            else
            {
                /* A character was entered.  It was not a new line, backspace
                or carriage return, so it is accepted as part of the input and
                placed into the input buffer.  When a n is entered the complete
                string will be passed to the command interpreter. */
                if( cInputIndex < MAX_INPUT_LENGTH )
                {
                    pcInputString[ cInputIndex ] = cRxedChar;
                    cInputIndex++;
                    FreeRTOS_write( xConsole, &cRxedChar, sizeof( cRxedChar ) );
                }
            }
        }
    }
}
