#pragma once

#include <stdio.h>

FILE *file_open(const char *filename, const char *mode);
void file_close(FILE *fptr);
size_t file_length(FILE *fptr);
void file_read(void *buffer, size_t bytes, FILE *fptr);
