#ifndef __STRING_H__
#define __STRING_H__
#include <stddef.h>
#include <stdint.h>
void* memcpy(void* dest, const void* src, size_t len);
void* memset(void* dest, int byte, size_t len);
int memcmp(const void* ptr1, const void* ptr2, size_t len);

/* Received from https://code.woboq.org/userspace/glibc/string/strcmp.c.html*/
int strcmp (const char *p1, const char *p2);
int strncmp (const char *s1, const char *s2, size_t n);

size_t strlen (const char *str);
size_t strnlen(const char *str, size_t maxlen);
char * strncpy (char *s1, const char *s2, size_t n);
#endif
