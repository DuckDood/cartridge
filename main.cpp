#include <stdint.h>
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
	std::string makefileStr = "";
	std::string line;
	int tempGet;
	std::istringstream buff(buffer);
	std::string token;
	std::string token2;
	bool running = true;
	bool inMake;
	int i = 0;
	while(running) {
	std::getline(buff, line);
		i++;
		tempGet = line.find(':');
		if(tempGet == 0) {
			std::cout << "Line:"<< i << ", Syntax error: Use of Colon suspiciously (in first character of line)\n";
			exit(1);
		}
		if(tempGet == std::string::npos && !inMake) {
			token = line;
			//std::cout << token;
		} else {
			if (!inMake) {
				token = line.substr(0, tempGet);
				token2 = line.substr(tempGet+1, line.length());
				inMake = true;
				makefileStr += line + '\n';
			} else if(line == "end") {
				inMake = false;
				running = false;
			} else {
				makefileStr += line + '\n';
			}
		}
	}

	std::cout << makefileStr;
	fwrite(makefileStr.c_str(), sizeof(char), makefileStr.length(), makefile);


	return 0;
}
