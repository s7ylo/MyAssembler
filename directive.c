/*
 * directive.c
 */

#include "directive.h"

/* all available guidance statements */
static char *available_directives[AVAILABLE_DIRECTIVE_COUNT] =
{
		".data",
		".string",
		".extern",
		".entry"
};

char* is_directive(const char *field)
{
	int i;

	for (i = 0; i < AVAILABLE_DIRECTIVE_COUNT; i++)
	{
		if (!strcasecmp(field, available_directives[i]))
		{
			return available_directives[i];
		}
	}

	return NULL;
}
