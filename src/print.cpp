#include "print.h"

#include <iostream>

void print(std::string const& str, bool add_newline) {
	std::cout << str;
	if (add_newline) {
		std::cout << std::endl;
	}
}
void printError(std::string const& err) {
	print("Error: " + err);
}
