/*
 * directive.h
 */

#ifndef DIRECTIVE_H_
#define DIRECTIVE_H_

#include "arch.h"
#include "Assembler.h"

#define AVAILABLE_DIRECTIVE_COUNT (4)

/* assembler's directives */
#define DIRECTIVE_DATA   ".data"
#define DIRECTIVE_STRING ".string"
#define DIRECTIVE_ENTRY  ".entry"
#define DIRECTIVE_EXTERN ".extern"

char* is_directive(const char *field);

#endif /* DIRECTIVE_H_ */
