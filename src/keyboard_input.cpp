#include "keyboard_input.h"

#include <conio.h>
#include <iostream>

auto getKey() -> char {
	return _getch();
}
auto getConfirmation() -> bool {
	while (true) {
		char const ch = getKey();
		if (ch == 'n') {
			return false;
		}
		else if (ch == 'y') {
			return true;
		}
	}
	return false;
}
auto getLine() -> std::string {
	std::string input{};
	std::getline(std::cin, input);
	return input;
}
