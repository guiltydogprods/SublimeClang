#include "file.h"

FILE *file_open(const char *filename, const char *mode)
{
	return fopen(filename, mode);
}

void file_close(FILE *fptr)
{
	fclose(fptr);
}

size_t file_length(FILE *fptr)
{
	fseek(fptr, 0, SEEK_END);
	size_t length = ftell(fptr);
	rewind(fptr);

	return length;
}

void file_read(void *buffer, size_t bytes, FILE *fptr)
{
	fread(buffer, sizeof(char), bytes, fptr);	
}
