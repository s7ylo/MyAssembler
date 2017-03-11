/*
 * arch.h
 */

#ifndef ARCH_H_
#define ARCH_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define WORD_SIZE  (15)
#define REGS_COUNT (8)

typedef struct _word
{
	unsigned short data:WORD_SIZE;
} word, *word_t;

/* the cpu architecture */
typedef struct _cpu
{
	word r1;
	word r2;
} cpu, *cpu_t;

#endif /* ARCH_H_ */
