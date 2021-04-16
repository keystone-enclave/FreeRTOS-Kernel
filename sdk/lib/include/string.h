#ifndef __STRING_H__
#define __STRING_H__
#include <stddef.h>
#include <stdint.h>
void* memcpy(void* dest, const void* src, size_t len);
void* memset(void* dest, int byte, size_t len);
int memcmp(const void* ptr1, const void* ptr2, size_t len);

/* Received from https://code.woboq.org/userspace/glibc/string/strcmp.c.html*/
int strcmp (const char *p1, const char *p2);
char *strcpy (char *dest, const char *src);
size_t strlen (const char *str);
/* Set N bytes of S to 0.  */
void bzero (void *s, size_t len);
#endif
