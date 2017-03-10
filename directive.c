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
	char *field_cpy = strdup(field);
	char *token;
	int i;

	token = strtok(field_cpy, " \t");

	for (i = 0; i < AVAILABLE_DIRECTIVE_COUNT; i++)
	{
		if (!strcasecmp(token, available_directives[i]))
		{
			free(field_cpy);
			return available_directives[i];
		}
	}

	free(field_cpy);
	return NULL;
}
