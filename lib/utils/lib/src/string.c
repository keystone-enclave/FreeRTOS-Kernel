#include "string.h"

/* TODO This is a temporary place to put libc functionality until we
 * decide on a lib to provide such functionality to the runtime */

#include <stdint.h>
#include <ctype.h>

void* memcpy(void* dest, const void* src, size_t len)
{
  const char* s = src;
  char *d = dest;

  if ((((uintptr_t)dest | (uintptr_t)src) & (sizeof(uintptr_t)-1)) == 0) {
    while ((void*)d < (dest + len - (sizeof(uintptr_t)-1))) {
      *(uintptr_t*)d = *(const uintptr_t*)s;
      d += sizeof(uintptr_t);
      s += sizeof(uintptr_t);
    }
  }

  while (d < (char*)(dest + len))
    *d++ = *s++;

  return dest;
}


void* memset(void* dest, int byte, size_t len)
{
  if ((((uintptr_t)dest | len) & (sizeof(uintptr_t)-1)) == 0) {
    uintptr_t word = byte & 0xFF;
    word |= word << 8;
    word |= word << 16;
    word |= word << 16 << 16;

    uintptr_t *d = dest;
    while (d < (uintptr_t*)(dest + len))
      *d++ = word;
  } else {
    char *d = dest;
    while (d < (char*)(dest + len))
      *d++ = byte;
  }
  return dest;
}

int memcmp(const void* s1, const void* s2, size_t n)
{
  unsigned char u1, u2;

  for ( ; n-- ; s1++, s2++) {
    u1 = * (unsigned char *) s1;
    u2 = * (unsigned char *) s2;
    if ( u1 != u2) {
      return (u1-u2);
    }
  }
  return 0;
}


/* Received from https://code.woboq.org/userspace/glibc/string/strcmp.c.html*/
/* Compare S1 and S2, returning less than, equal to or
   greater than zero if S1 is lexicographically less than,
   equal to or greater than S2.  */
int
strcmp (const char *p1, const char *p2)
{
  const unsigned char *s1 = (const unsigned char *) p1;
  const unsigned char *s2 = (const unsigned char *) p2;
  unsigned char c1, c2;
  do
    {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0')
        return c1 - c2;
    }
  while (c1 == c2);
  return c1 - c2;
}
/* Received from https://code.woboq.org/userspace/glibc/string/strcmp.c.html*/
char *
strncpy (char *s1, const char *s2, size_t n)
{
  size_t size = strnlen (s2, n);
  if (size != n)
    memset (s1 + size, '\0', n - size);
  return memcpy (s1, s2, size);
}

size_t
strlen (const char *str)
{
  const char *tmp = str;
  for (;*tmp;tmp++);
  return tmp - str;
}
 
size_t
strnlen(const char *str, size_t maxlen)
{
    size_t i = 0;
    for (; i < maxlen && *str; str++, i++);
    return i;
}

char *
strcpy (char *dest, const char *src)
{
  for (char *d = dest; *src ; d++, src++) {
    *d = *src;
  }
  return dest;
}

/* Received from https://code.woboq.org/userspace/glibc/string/strcmp.c.html*/
/* Compare no more than N characters of S1 and S2,
   returning less than, equal to or greater than zero
   if S1 is lexicographically less than, equal to or
   greater than S2.  */
int
strncmp (const char *s1, const char *s2, size_t n)
{
  unsigned char c1 = '\0';
  unsigned char c2 = '\0';
  if (n >= 4)
    {
      size_t n4 = n >> 2;
      do
        {
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c1 != c2)
            return c1 - c2;
        } while (--n4 > 0);
      n &= 3;
    }
  while (n > 0)
    {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0' || c1 != c2)
        return c1 - c2;
      n--;
    }
  return c1 - c2;
}