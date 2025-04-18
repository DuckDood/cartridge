#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>

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
// stackoverflow clutch
int count_newlines(std::string s) {
  int count = 0;

  for (int i = 0; i < s.size(); i++)
    if (s[i] == '\n') count++;

  return count;
}


int main(int argc, char* argv[]) {
	long length;
	FILE* config;
	FILE* makefile = fopen("Makefile", "w"); // probably gonna make a pr to change to c++ file handling
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
	std::string makefile_string = "";
	std::string line;
	int temp_get;
	int temp2;
	std::istringstream buff(buffer);
	std::string token;
	std::string token2;
	std::string temp = "";
	std::string temp3 = "";
	std::string compiler;
	bool running = true;
	bool inMake;
	int i = 0;
	int newline_count = count_newlines(buffer);
	std::string end_all = "all: ";
	std::vector<std::string> names;
	std::vector<std::string> dirnames;
	std::vector<std::string> buildnames;
	std::string nextobj = "";
	std::string token3 = "";
	bool noall = false;
	while(running) {
	
		i++;
		if(i >= newline_count) running = false;	
		std::getline(buff, line);
	
		temp_get = line.find(':');
		if(temp_get == 0) {
			std::cout << "Line:"<< i << ", Syntax error: Use of Colon suspiciously (in first character of line)\n";
			exit(1);
		}
		if(temp_get == std::string::npos && !inMake) {
			temp_get = line.find(' ');
			token = line.substr(0, temp_get);
			token2 = line.substr(temp_get+1, line.length());
			if(token == "include") {
				token3 = token2.substr(token2.rfind("."), token2.length());
			//	token2 = "src/" + token2;
				temp = token2.substr(0, token2.rfind("."));
				temp3=temp;
				if(nextobj != "") {
					temp = nextobj;
				}
				//std::cout << temp << "\n";
				if(nextobj == "") {
					temp = "obj/" + temp + ".o: " + token2;
				} else {
					temp = "obj/" + temp + ": " + token2;
				}
				if(nextobj == "") {
					temp3 = "obj/" + temp3 + ".o: " + "src/"+token2;
				} else {
					temp3 = "obj/" + temp3 + ": " + "src/"+token2;
				}
				temp2 = temp.length();
				nextobj = "";
				if(!noall) {
					names.push_back(temp.substr(0, temp2 - (token2.length()) -2));
					noall = false;
				}
				compiler = token3 == ".c"? "${CC}": "${CXX}"; // should change to fallback on c, not other way around.
				// tabs in front of compiler, not space
 				temp3 += "\n	" + compiler + " " + "src/" + token2 + " -c -o obj/" + token2.substr(0, token2.rfind('.')) + ".o";
			//	std::cout << temp;
				makefile_string += temp3 + "\n\n";

			} else
			if(token == "nextobj") {
				nextobj = line.substr(temp_get+1, line.length());
			} else
			if(token == "build") {
				token2 = line.substr(temp_get+1, line.length());
				temp = "";
				for(int i = 0; i < names.size(); i++) {
					temp += names.at(i) + " ";
				}	

				token3 = "build/"+token2 + ": " + temp;
				if(!noall) {
					names.push_back("build/"+token2);
				}
				noall = false;
				token3 += "\n	g++ " + temp + " -o build/" + token2;
				token3 += "\n\n";
				//std::cout << token3 << "\n";
				makefile_string += token3;
			} else
			if(token == "noall") {
				noall = true;
			} else
			if(token == "adddir") {
				token2 = line.substr(temp_get+1, line.length());
				if(token2.at(token2.length()-1) != '/') {
					std::cout << "Line:" << i << ", Syntax error: no slash at end of directory.\n";
					exit(1);
				}
				temp = "";
				if(!noall) {
					dirnames.push_back(token2);
				}
				noall = false;
				temp += token2;
				temp += ":\n	mkdir -p " + token2 + "\n\n";
				makefile_string += temp;
			}
		} else {
			if(!inMake) {
				token = line.substr(0, temp_get);
				if(token.at(token.length()-1) == '/') {
					if(!noall) {
						dirnames.push_back(token);
					}
					
				} else {
					if(!noall) {
						names.push_back(token);
					}
				}
				noall = false;
				//names.push_back(token);
				token2 = line.substr(temp_get+1, line.length());
				inMake = true;
				makefile_string += line + '\n';
			} else if(line == "end") {
				inMake = false;
			} else {
				makefile_string += line + '\n';
			}
		}
	}

	//std::cout << makefile_string;
	for(int i = 0; i < dirnames.size(); i++) {
		end_all += dirnames.at(i) + " ";
	}	
	for(int i = 0; i < names.size(); i++) {
		end_all += names.at(i) + " ";
	}	
	end_all += '\n';
	makefile_string = end_all + makefile_string;
	fwrite(makefile_string.c_str(), sizeof(char), makefile_string.length(), makefile);
	fclose(makefile);


	return 0;
}
