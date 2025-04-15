#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

// thanks stackoverflow
char* readfile(FILE *f) {
  // f invalid? fseek() fail?
  if (f == NULL || fseek(f, 0, SEEK_END)) {
    return NULL;
  }

  long length = ftell(f);
  rewind(f);
  // Did ftell() fail?  Is the length too long?
  if (length == -1 || (unsigned long) length >= SIZE_MAX) {
    return NULL;
  }

  // Convert from long to size_t
  size_t ulength = (size_t) length;
  char *buffer = (char*)malloc(ulength + 1);
  // Allocation failed? Read incomplete?
  if (buffer == NULL || fread(buffer, 1, ulength, f) != ulength) {
    free(buffer);
    return NULL;
  }
  buffer[ulength] = '\0'; // Now buffer points to a string

  return buffer;
}


int main(int argc, char* argv[]) {
	long length;
	FILE* config;
	FILE* makefile = fopen("Makefile", "w"); // probably gonna make a pr to change to c++ files
	if(argv[1] != NULL)
		config = fopen(argv[1], "rb");
	else {
		config = fopen("cartridgelists.txt", "rb");
	}
	char* buf = readfile(config);
	if(buf == NULL) {
		std::cerr << "File not accessible.\n";
		return 1;
	}

	fclose(config);
	
	std::string buffer = buf; // strings are way easier tbh
	std::string line;
	std::istringstream buff(buffer);

	std::getline(buff, line);

	std::cout << line;



	return 0;
}
