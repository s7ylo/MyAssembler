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

/* Fix external types */
typedef struct _program_object *program_object_t;

char* is_directive(const char *field);

word_t
handle_directive(
		const char *directive_line,
		program_object_t prog_obj,
		bool is_first_transition);

#endif /* DIRECTIVE_H_ */
