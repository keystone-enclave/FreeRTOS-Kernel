// #include <stdio.h>
#include <stdint.h>
// #include <stdlib.h>
// #include <math.h>
#include "printf.h"
#include "malloc.h"
#include "eapp_utils.h"

#define test(p) (primes[p >> 6] & 1 << (p & 0x3f))
#define set(p) (primes[p >> 6] |= 1 << (p & 0x3f))
#define is_prime(p) !test(p)

void EAPP_ENTRY eapp_entry()
{
  uintptr_t cycles1,cycles2;
  printf("[primes]\n");
  asm volatile ("rdcycle %0" : "=r" (cycles1));

  int limit = 33333333;
  int64_t sqrt_limit = 5773.5026; //Should be sqrt(limit)
	size_t primes_size = ((limit >> 6) + 1) * sizeof(uint64_t);
	uint64_t *primes = (uint64_t*)malloc(primes_size);
	int64_t p = 2; 
	while (p <= limit >> 1) {
		for (int64_t n = 2 * p; n <= limit; n += p) if (!test(n)) set(n);
		while (++p <= sqrt_limit && test(p));
	}
	for (int i = limit; i > 0; i--) {
		if (is_prime(i)) {
			printf("%d\n", i);
        asm volatile ("rdcycle %0" : "=r" (cycles2));
        printf("iruntime %lu\r\n",cycles2-cycles1);

			syscall_task_return();
		}
	}


};
