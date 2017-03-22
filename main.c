/*
 * main.c
 */

#include <stdio.h>
#include <string.h>
#include "Assembler.h"

char*
read_assembly_file(
		const char *szFileName);

void
display_usage(void);

void
create_entry_file(
		const char *file_name,
		program_object_t prog_obj);

void
create_extern_file(
		program_object_t prog_obj);

void
create_program_file(
		program_object_t prog_obj);

/*
 * Usage: MyAssembler <input file(s).as>
 */
int
main(
	int argc,
	char *argv[])
{
	program_object_t prog_obj = NULL;
	bool success;
	char szFileName[260] = {0};
	char *buffer;

	if (argc != 2)
	{
		fprintf(
				stdout,
				"Usage: MyAssembler <input file(s).as>\n\n");

		return 0;
	}

	strncpy(szFileName, argv[1], strlen(argv[1]));
	buffer = read_assembly_file(szFileName);

	prog_obj = assembler_first_transition(buffer);
	success = assembler_second_transition(buffer, prog_obj);

	create_entry_file("bla.ent", prog_obj);

	if (success)
	{
		fprintf(
			stdout,
			"Program assembled successfully\n");
	}

	return 1;
}

char*
read_assembly_file(
		const char *szFileName)
{
	FILE *fp = NULL;
	long file_size;
	char *content = NULL;

	fp = fopen(szFileName, "rt");

	if (!fp)
	{
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	file_size = ftell(fp);
	rewind(fp);

	content = (char*)calloc(file_size + 1, sizeof(char));

	if (!content)
	{
		fclose(fp);
		return NULL;
	}

	fread(content, file_size, sizeof(char), fp);
	fclose(fp);

	return content;
}

void
display_usage(void)
{
}

void
create_entry_file(
		const char *file_name,
		program_object_t prog_obj)
{
	void *buffer = calloc(1000, sizeof(word)); /* assuming a size of 1000 words will be enough for an entry file */
	FILE *ent_file = NULL;
	symbol_table_entry_t entry = prog_obj->symtab_entry;
	word buffer_offs = {0};

	if (!buffer)
	{
		// TODO: Unable to allocate memory for entry file buffer
		return;
	}

	ent_file = fopen(
			file_name,
			"wb");

	if (!ent_file)
	{
		// TODO: Unable to open file for writing
		return;
	}

	while (entry)
	{
		if (entry->sym->flags.data & SYMBOL_TYPE_ENTRY)
		{
			memcpy(
				(void*)(buffer + buffer_offs.data),
				entry->sym->name,
				strlen(entry->sym->name));

			buffer_offs.data += strlen(entry->sym->name);

			memcpy(
				(void*)(buffer + buffer_offs.data),
				&entry->sym->address,
				sizeof(word));

			buffer_offs.data += sizeof(word);
		}

		entry = entry->next;
	}

	fclose(ent_file);
}
