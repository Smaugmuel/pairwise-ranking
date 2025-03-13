#include <clocale>

#include "program_loop.h"

int main() {
	setlocale(LC_ALL, "sv-SE");

	programLoop();

	return 0;
}
