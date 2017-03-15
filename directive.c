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
	char *field_cpy_e;
	char *token;
	int i;

	token = strtok_r(field_cpy, " \t", &field_cpy_e);

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

void handle_data_directive(
		const char *directive_line,
		program_object_t prog_obj)
{
	char *directive_line_cpy = strdup(directive_line);
	char *directive_line_e;
	char *token;
	char *string_value = NULL;
	u_short string_length;
	u_short i;

	/* extract the directive (.string or .data) */
	token = strtok_r(
			directive_line_cpy,
			" \t",
			&directive_line_e);

	if (!strcasecmp(token, DIRECTIVE_DATA))
	{
		token = strtok_r(
				NULL,
				" \t,",
				&directive_line_e);

		while (token)
		{
			prog_obj->prog_image.data_image[prog_obj->prog_image.data_image_length.data++].data = strtoul(token, NULL, 10);

			/* increase data counter by one */
			prog_obj->dc->data++;

			/* move to the next number (in case it has another one) */
			token = strtok_r(
					NULL,
					" \t,",
					&directive_line_e);
		}
	}
	else /* DIRECTIVE_STRING */
	{
		string_length = (strrchr(directive_line, '"') - strchr(directive_line, '"')) - 1;
		string_value = strchr(directive_line, '"') + 1;

		for (i = 0; i < string_length; i++)
		{
			prog_obj->prog_image.data_image[prog_obj->prog_image.data_image_length.data++].data = string_value[i];
		}

		/* set the null terminator */
		prog_obj->prog_image.data_image[prog_obj->prog_image.data_image_length.data++].data = 0;
		prog_obj->dc->data += string_length + 1;
	}
}

u_short handle_directive(
		const char *directive_line,
		program_object_t prog_obj,
		bool is_first_transition)
{
	char *directive_line_cpy = strdup(directive_line); // TODO: remember to free this at the end
	char *directive_line_e;
	char *token;

	token = strtok_r(
			directive_line_cpy,
			" \t",
			&directive_line_e);

	if (is_first_transition)
	{
		/* in the first transition we ignore the '.entry' statement */
		if (!strcasecmp(token, DIRECTIVE_EXTERN))
		{
			token = strtok_r(
					NULL,
					" \t",
					&directive_line_e);

			handle_symbol(
					prog_obj,
					token,
					true,
					false,
					0);

			return 0;
		}
		else if (!strcasecmp(token, DIRECTIVE_STRING) ||
				  !strcasecmp(token, DIRECTIVE_DATA))
		{
			handle_data_directive(directive_line, prog_obj);
		}
	}
	else
	{
		// TODO: Shouldn't be 0
		return 0;
	}

	// TODO: Shouldn't be 0
	return 0;
}
