#include "stdio.h"

#include "FreeRTOS_IO.h"

/*-----------------------------------------------------------*/
size_t 
uart_read( void * const pvBuffer, const size_t xBytes )
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
uart_write( const void *pvBuffer, const size_t xBytes )
{
    // Lazy impl just to set stdout.
    const char * buf = pvBuffer;
    size_t i = 0;
    for (; i < xBytes; i++) {
        putchar(*(buf + i));
    }
    return i;

}

/* Structure that defines the "Uart" device. */
static const IO_Device_Definition_t xUart =
{
	"/dev/uart",    /* The device string to type into open. */
	uart_read,     /* The function to run if the IO read command is called. */
	uart_write,    /* The function to run if the IO write command is called. */
	NULL            /* The function to run if the IO ioctl command is called. Note uart does not support ioctl. */
};
/*-----------------------------------------------------------*/

void vRegisterIODevices( void )
{
    FreeRTOS_IORegisterDevice(&xUart);
}

/*-----------------------------------------------------------*/