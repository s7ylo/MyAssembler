/*
 * main.c
 */

#include <stdio.h>
#include <string.h>
#include "Assembler.h"

char* read_file_content(const char *szFileName);

/*
 * Usage: MyAssembler <input file(s).as>
 */
int main(int argc, char *argv[])
{
	char szFileName[260] = {0};
	char *buffer;

	if (argc != 2)
	{
		fprintf(stdout, "Usage: MyAssembler <input file(s).as>\n\n");
		return 0;
	}

	strncpy(szFileName, argv[1], strlen(argv[1]));
	buffer = read_file_content(szFileName);

	assembler_first_transition(buffer);
	// translate_source_code(buffer);

	return 1;
}

char* read_file_content(const char *szFileName)
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
