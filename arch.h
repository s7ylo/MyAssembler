/*
 * arch.h
 */

#ifndef ARCH_H_
#define ARCH_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define WORD_SIZE (15)

typedef struct _word
{
	unsigned short data:WORD_SIZE;
} word, *word_t;

#endif /* ARCH_H_ */
