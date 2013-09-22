#include <stdio.h>
#include <stdlib.h>

char* fileRead(char *filename, long* file_length)
{
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
	  perror("fopen");
	}

	fseek(file, 0, SEEK_END);
	*file_length = ftell(file);
	if (*file_length == -1) {
	  perror("ftell");
	}
	fseek(file, 0, SEEK_SET);
	char* data = malloc(*file_length);
	if (fread(data, 1, *file_length, file) != (size_t)*file_length) {
	  fprintf(stderr, "couldn't read entire file\n");
	  FREE(data);
	}
	return data;
}

char *fileWrite(FILE *file, char *buffer, int length)
{
	if (file == NULL) {
	  return
	}
	fwrite(buffer, length, 1, file);
}