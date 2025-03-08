#include "testing.h"

#include <iostream>
#include <sstream>
#include <string>
#include <queue>

#include "program_loop.h"

extern std::queue<char> g_keys;
extern std::queue<std::string> g_lines;

namespace
{

void endToEnd_quit() {
	std::streambuf* backup;
	std::stringstream stream;
	backup = std::cout.rdbuf(stream.rdbuf());

	g_keys.push('q');
	programLoop();

	std::cout.rdbuf(backup);

	std::string const expected_printout{
		"-----------------------\n"
		"|  Pairwise ranking   |\n"
		"|---------------------|\n"
		"| [N]ew voting round  |\n"
		"| [L]oad voting round |\n"
		"|  [C]ombine scores   |\n"
		"|       [Q]uit        |\n"
		"-----------------------\n" 
	};
	ASSERT_EQ(stream.str(), expected_printout);
}
void endToEnd_combineWithInvalidFiles() {
	std::streambuf* backup;
	std::stringstream stream;
	backup = std::cout.rdbuf(stream.rdbuf());

	g_keys.push('c');
	g_lines.push("file1 file2");
	g_lines.push("c");
	g_keys.push('q');
	programLoop();

	std::cout.rdbuf(backup);

	std::string const expected_printout{
		"-----------------------\n"
		"|  Pairwise ranking   |\n"
		"|---------------------|\n"
		"| [N]ew voting round  |\n"
		"| [L]oad voting round |\n"
		"|  [C]ombine scores   |\n"
		"|       [Q]uit        |\n"
		"-----------------------\n"
		"Select two or more score files to combine, or just \'c\' to cancel: "		// TODO: Fix newline issue
		"Error: File 'file1' doesn't exist\n"
		"Error: File 'file2' doesn't exist\n"
		"Error: No scores combined\n"
		"Select two or more score files to combine, or just \'c\' to cancel: "		// TODO: Fix newline issue
		"-----------------------\n"
		"|  Pairwise ranking   |\n"
		"|---------------------|\n"
		"| [N]ew voting round  |\n"
		"| [L]oad voting round |\n"
		"|  [C]ombine scores   |\n"
		"|       [Q]uit        |\n"
		"-----------------------\n"
	};
	std::cout << expected_printout << std::endl;
	std::cout << stream.str() << std::endl;
	ASSERT_EQ(stream.str(), expected_printout);
}

} // namespace

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_quit);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_combineWithInvalidFiles);
	return 1;
}
