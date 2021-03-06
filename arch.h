/*
 * arch.h
 */

#ifndef ARCH_H_
#define ARCH_H_

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <stdarg.h>
#include "prototype.h"

#define WORD_SIZE  (15)
#define REGS_COUNT (8)

typedef unsigned short ushort;

typedef struct _word
{
	/* Compiling with -pedantic causes this warning to pop
	 * and there is nothing we can do about it
	 * so forgive me for this warning
	 */
	ushort data:WORD_SIZE;
} word, *word_t;

/* the cpu architecture */
typedef struct _cpu
{
	word r0;
	word r1;
	word r2;
	word r3;
	word r4;
	word r5;
	word r6;
	word r7;
} cpu, *cpu_t;

enum RegId
{
	R0 = 0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	MaxReg
};

#endif /* ARCH_H_ */
