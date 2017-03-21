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

/* handle_data_directive return value is the size of the data in words
 */
static word_t handle_data_directive(
		const char *directive_line,
		program_object_t prog_obj)
{
	char *directive_line_cpy = strdup(directive_line);
	char *directive_line_e;
	char *token;
	char *string_value = NULL;
	ushort string_length; /* TODO: Consider using word */
	ushort i;
	word_t data_size = (word_t)calloc(1, sizeof(word));

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
			prog_obj->prog_image.data_section[prog_obj->prog_image.data_section_size.data++].data = strtoul(token, NULL, 10);

			/* increase data counter by one */
			prog_obj->dc->data++;
			data_size->data++;

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
			prog_obj->prog_image.data_section[prog_obj->prog_image.data_section_size.data++].data = string_value[i];
		}

		/* set the null terminator */
		prog_obj->prog_image.data_section[prog_obj->prog_image.data_section_size.data++].data = 0;
		prog_obj->dc->data += (string_length + 1);
		data_size->data = (string_length + 1);
	}

	return data_size;
}

word_t handle_directive(
		const char *directive_line,
		program_object_t prog_obj,
		bool is_first_transition)
{
	char *directive_line_cpy = strdup(directive_line);
	char *directive_line_e;
	char *token;
	symbol_t sym = NULL;
	word flags = {0};

	token = strtok_r(
			directive_line_cpy,
			" \t",
			&directive_line_e);

	if (is_first_transition)
	{
		if (!strcasecmp(token, DIRECTIVE_STRING) ||
			!strcasecmp(token, DIRECTIVE_DATA))
		{
			free(directive_line_cpy);
			return handle_data_directive(directive_line, prog_obj);
		}

		if (!strcasecmp(token, DIRECTIVE_EXTERN))
		{
			/* extract the symbol name */
			token = strtok_r(
					NULL,
					" \t",
					&directive_line_e);

			flags.data = SYMBOL_TYPE_EXTERN;

			handle_symbol(
					prog_obj,
					token,
					&flags,
					NULL, /* data size */
					0);
		}
		else /* in case of .entry */
		{
			/* extract the symbol name */
			token = strtok_r(
					NULL,
					" \t",
					&directive_line_e);

			/* check if symbol exist before adding it into the symbol table */
			sym = lookup_symbol_by_name(
					prog_obj->sym_tbl,
					token);

			if (sym)
			{
				sym->flags.data |= SYMBOL_TYPE_ENTRY;
			}
			else /* in case it doesn't exist, add to symbol table */
			{
				flags.data = SYMBOL_TYPE_ENTRY;

				handle_symbol(
						prog_obj,
						token,
						&flags,
						NULL, /* data size */
						0);
			}
		}
	}
	else /* second transition */
	{
		free(directive_line_cpy);
		return NULL;
	}

	/* TODO: This */
	free(directive_line_cpy);
	return NULL;
}
