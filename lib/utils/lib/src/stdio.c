#include "sbi.h"

int
getchar()
{
    return sbi_getchar();
}

int
putchar(int character)
{
    return sbi_putchar(character);
}