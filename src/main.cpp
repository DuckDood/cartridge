#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

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
	std::string linker;
	bool running = true;
	bool inMake;
	int i = 0;
	int newline_count = count_newlines(buffer);
	std::string end_all = "all: ";
	std::vector<std::string> names;
	std::vector<std::string> dirnames;
	std::vector<std::string> buildnames;
	std::vector<std::string> depnames; // too many vectors
	std::string nextobj = "";
	std::string token3 = "";
	std::vector<std::string>::iterator iterator;
	std::vector<std::string>::iterator iterator2;
	std::vector<std::string> biops;
	std::vector<std::string> biopsl;
	std::vector<std::string> bivars;
	std::vector<std::string> bivarsl; // about the too many vectors thing...
	bool cmclear = false;
	bool lkclear = false;
	bool noall = false;
	bool nobuild = false;
	std::string bname = "build/";
	std::string sname = "src/";
	std::string oname = "obj/";
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
					temp = oname + temp + ".o: " + token2;
				} else {
					temp = oname + temp + ": " + token2;
				} 
				// getting obj names different from src names correctly
				if(nextobj == "") {
				//	temp3 = "obj/" + temp3 + ".o: " + "src/"+token2;
					temp3 = oname + temp3 + ".o: " + sname+token2;
				} else {
					temp3 = oname + temp3 + ": " + sname+token2;
				}
				temp2 = temp.length();
				nextobj = "";
				if(!noall) {
					names.push_back(temp.substr(0, temp2 - (token2.length()) -2));
				}
					noall = false;
				if(!nobuild) {
					buildnames.push_back(temp.substr(0, temp2 - (token2.length()) -2));
				}
				nobuild=false;
				if(compiler.empty()) {
				compiler = token3 == ".c"? "${CXX}": "${CC}";
				cmclear = true;
				}
				// tabs in front of compiler, not space
 				temp3 += "\n	" + compiler + " " + sname + token2 + " -c -o "+oname + token2.substr(0, token2.rfind('.')) + ".o ";
				if(cmclear) {
					compiler = "";
					cmclear = false;
				}
				token3 = "";
				for(int i = 0; i < biops.size(); i++) {
					token3 += "-D" + biops.at(i) + "=${" + bivars.at(i) + "} ";
				}
				for(int i = 0; i < biopsl.size(); i++) {
					token3 += "-D" + biopsl.at(i) + "=" + bivarsl.at(i) + " ";
				}
				temp3 += token3;
			//	std::cout << temp;
				makefile_string += temp3 + "\n\n";

			} else
			if(token == "nextobj") {
				nextobj = line.substr(temp_get+1, line.length());
			} else
			if(token == "build") {
				token2 = line.substr(temp_get+1, line.length());
				temp = "";
				for(int i = 0; i < buildnames.size(); i++) {
					temp += buildnames.at(i) + " ";
				}	

				token3 = bname+token2 + ": " + temp;
				if(!noall) {
					names.push_back(bname+token2);
				}
				noall = false;
				if(linker.empty()) {
				linker = "${CXX}";
				lkclear = true;
				}
				token3 += "\n	"+linker+" " + temp + " -o "+bname + token2;
				if(lkclear) {
					compiler = "";
					lkclear = false;
				}
				token3 += " "; // adding dependencies with -l
				for(int i = 0; i < depnames.size(); i++) { // yeah
					token3 += "-l" + depnames.at(i) + " ";
				}	
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
			} else
			if(token == "nobuild") {
				nobuild=true;
			} else
			if(token == "dep") {
				token2 = line.substr(temp_get+1, line.length());
				depnames.push_back(token2);
			} else
			if(token == "abuild") {
				token2 = line.substr(temp_get+1, line.length());
				token2.erase(token2.rfind("."), token2.length());
				token2 += ".o";
				token2 = oname + token2;
				buildnames.push_back(token2);
			} else
			if(token == "rmbuild") {
				token2 = line.substr(temp_get+1, line.length());
				if(token2 == "/") {
					buildnames.clear();
					continue;
				}
				token2.erase(token2.rfind("."), token2.length());
				token2 += ".o";
				iterator = std::find(buildnames.begin(), buildnames.end(), oname + token2);
				if(iterator == buildnames.end()) {
					std::cout << "Line:" << i << ", Syntax error: trying to remove a source without being defined\n";
					exit(1);
				}
			
				temp2 = std::distance(buildnames.begin(), iterator);
				buildnames.erase(iterator);
			} else
			if(token == "rmdep") {
				token2 = line.substr(temp_get+1, line.length());
				iterator = std::find(depnames.begin(), depnames.end(), token2);
				if(iterator == depnames.end()) {
					std::cout << "Line:" << i << ", Syntax error: trying to remove a dependency without being defined\n";
					exit(1);
				}
				temp2 = std::distance(depnames.begin(), iterator);
				depnames.erase(iterator);
			} else 
			if(token == "var") {
				line.erase(0, temp_get+1);
				token2 = line.substr(0, line.find(" "));
				line.erase(0, line.find(" ")+1);
				token3 = line;
				temp = "";
				temp += "ifndef " + token2;
				temp += "\n" + token2 + " := " + token3;
				temp += "\nendif\n";
				makefile_string += temp;
			} else 
			if(token == "vare") {
				line.erase(0, temp_get+1);
				token2 = line.substr(0, line.find(" "));
				line.erase(0, line.find(" ")+1);
				token3 = line;
				temp = "";
				temp += "ifndef " + token2;
				temp += "\n" + token2 + " = " + token3;
				temp += "\nendif\n";
				makefile_string += temp;
			} else
			if(token == "bopt") {
				line.erase(0, temp_get+1);
				token2 = line.substr(0, line.find(" "));
				line.erase(0, line.find(" ")+1);
				token3 = line;
				biops.push_back(token2);
				bivars.push_back(token3);
			} else
			if(token == "boptl") {
				line.erase(0, temp_get+1);
				token2 = line.substr(0, line.find(" "));
				line.erase(0, line.find(" ")+1);
				token3 = line;
				biopsl.push_back(token2);
				bivarsl.push_back(token3);
			} else
			if(token == "rmbopt") {
				token2 = line.substr(temp_get+1, line.length());
				iterator = std::find(biops.begin(), biops.end(),token2);
				if(iterator == biops.end()) {
					std::cout << "Line:" << i << ", Syntax error: trying to remove a build option without being defined\n";
					exit(1);
				}
			
				temp2 = std::distance(biops.begin(), iterator);
				iterator2 = bivars.begin();
				std::advance(iterator2, temp2);
				biops.erase(iterator);
				bivars.erase(iterator2);
			} else
			if(token == "rmboptl") {
				token2 = line.substr(temp_get+1, line.length());
				iterator = std::find(biopsl.begin(), biopsl.end(),token2);
				if(iterator == biopsl.end()) {
					std::cout << "Line:" << i << ", Syntax error: trying to remove a build option without being defined\n";
					exit(1);
				}
			
				temp2 = std::distance(biopsl.begin(), iterator);
				iterator2 = bivarsl.begin();
				std::advance(iterator2, temp2);
				biopsl.erase(iterator);
				bivarsl.erase(iterator2);
			} else 
			if(token == "compiler") {
				if(temp_get == std::string::npos) {
					compiler = "";
				} else {
				compiler = line.substr(temp_get+1, line.length());
				}
			} else
			if(token == "linker") {
				if(temp_get == std::string::npos) {
					linker = "";
				} else {
					linker = line.substr(temp_get+1, line.length());
				}
			} else
			if(token == "svar") {
				line.erase(0, temp_get+1);
				token2 = line.substr(0, line.find(" "));
				line.erase(0, line.find(" ")+1);
				token3 = line;
				temp = "";
				temp += "\n" + token2 + " := " + token3;
				temp += "\n";
				makefile_string += temp;
			} else 
			if(token == "svare") {
				line.erase(0, temp_get+1);
				token2 = line.substr(0, line.find(" "));
				line.erase(0, line.find(" ")+1);
				token3 = line;
				temp = "";
				temp += "\n" + token2 + " = " + token3;
				temp += "\n";
				makefile_string += temp;
			} else
			if(token == "bname") {
				if(temp_get == std::string::npos) {
					bname = "build/";
				} else {
					bname = line.substr(temp_get+1, line.length());
				}
			} else
			if(token == "oname") {
				if(temp_get == std::string::npos) {
					oname = "obj/";
				} else {
					oname = line.substr(temp_get+1, line.length());
				}
			} else
			if(token == "sname") {
				if(temp_get == std::string::npos) {
					sname = "src/";
				} else {
					sname = line.substr(temp_get+1, line.length());
				}
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
