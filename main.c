/*
 * main.c
 */

#include <stdio.h>
#include <string.h>
#include "Assembler.h"

#define MAX_PATH 260

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
		const char *file_name,
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
	char asm_file_name[MAX_PATH] = {0};
	char obj_file_name[MAX_PATH] = {0};
	char ent_file_name[MAX_PATH] = {0};
	char ext_file_name[MAX_PATH] = {0};
	char *buffer = NULL;

	if (argc != 2)
	{
		fprintf(
				stdout,
				"Usage: MyAssembler <input file(s).as>\n\n");

		return 0;
	}

	strncpy(
		asm_file_name,
		argv[1],
		strlen(argv[1]));

	/* prepare output file names */
	/* program object file */
	strncpy(
		obj_file_name,
		asm_file_name,
		strrchr(asm_file_name, '.') - asm_file_name);

	sprintf(
		obj_file_name,
		"%s.ob",
		obj_file_name);

	/* externals file name */
	strncpy(
		ext_file_name,
		asm_file_name,
		strrchr(asm_file_name, '.') - asm_file_name);

	sprintf(
		ext_file_name,
		"%s.ext",
		ext_file_name);

	/* entries file name */
	strncpy(
		ent_file_name,
		asm_file_name,
		strrchr(asm_file_name, '.') - asm_file_name);

	sprintf(
		ent_file_name,
		"%s.ent",
		ent_file_name);

	/* read program's code */
	buffer = read_assembly_file(asm_file_name);

	prog_obj = assembler_first_transition(buffer);
	success = assembler_second_transition(buffer, prog_obj);

	create_entry_file(
			ent_file_name,
			prog_obj);

	create_extern_file(
			ext_file_name,
			prog_obj);

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

			if (strlen(entry->sym->name) % 2 == 0)
			{
				buffer_offs.data += strlen(entry->sym->name) + sizeof(word);
			}
			else
			{
				buffer_offs.data += strlen(entry->sym->name) + 1;
			}

			memcpy(
				(void*)(buffer + buffer_offs.data),
				&entry->sym->address,
				sizeof(word));

			buffer_offs.data += sizeof(word);
		}

		entry = entry->next;
	}

	fwrite(
		buffer,
		sizeof(word),
		(buffer_offs.data / 2),
		ent_file);

	fclose(ent_file);
}

void
create_extern_file(
		const char *file_name,
		program_object_t prog_obj)
{
	void *buffer = calloc(1000, sizeof(word)); /* assuming a size of 1000 words will be enough for an entry file */
	FILE *ext_file = NULL;
	symbol_table_entry_t entry = prog_obj->extsymtab_entry;
	word buffer_offs = {0};

	if (!buffer)
	{
		// TODO: Unable to allocate memory for entry file buffer
		return;
	}

	ext_file = fopen(
			file_name,
			"wb");

	if (!ext_file)
	{
		// TODO: Unable to open file for writing
		return;
	}

	while (entry)
	{
		memcpy(
			(void*)(buffer + buffer_offs.data),
			entry->sym->name,
			strlen(entry->sym->name));

		if (strlen(entry->sym->name) % 2 == 0)
		{
			buffer_offs.data += strlen(entry->sym->name) + sizeof(word);
		}
		else
		{
			buffer_offs.data += strlen(entry->sym->name) + 1;
		}

		memcpy(
			(void*)(buffer + buffer_offs.data),
			&entry->sym->address,
			sizeof(word));

		buffer_offs.data += sizeof(word);

		entry = entry->next;
	}

	fwrite(
		buffer,
		sizeof(word),
		(buffer_offs.data / 2),
		ext_file);

	fclose(ext_file);
}
