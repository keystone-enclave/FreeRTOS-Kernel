#include <FreeRTOS.h>
#include <task.h>
#include <printf.h> 
#include <csr.h>
#include <sbi.h>
#include <syscall.h> 
#include <enclave.h> 
#include <elf.h> 
#include <stdio.h>
#include <string.h>
#include <queue.h>
#include <rl.h>
#include <rl_config.h>
#include <timex.h>


#define test(p) (primes[p >> 6] & 1 << (p & 0x3f))
#define set(p) (primes[p >> 6] |= 1 << (p & 0x3f))
#define is_prime(p) !test(p)

void primes_task(void *pvParameters)
{
  printf("[primes]\n");
     cycles_t st = get_cycles();
  cycles_t et = 0;

  int limit = 333333;
  int64_t sqrt_limit = 577.34577; //Should be sqrt(limit)
	size_t primes_size = ((limit >> 6) + 1) * sizeof(uint64_t);
	uint64_t *primes = (uint64_t*)pvPortMalloc(primes_size);
	int64_t p = 2; 
	while (p <= limit >> 1) {
		for (int64_t n = 2 * p; n <= limit; n += p) if (!test(n)) set(n);
		while (++p <= sqrt_limit && test(p));
	}
	for (int i = limit; i > 0; i--) {
		if (is_prime(i)) {
			printf("%d\n", i);
        et = get_cycles();
        printf("iruntime %lu\r\n",et-st);

			return_general();
		}
	}


};
