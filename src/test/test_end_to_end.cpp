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

	ASSERT_EQ(stream.str(), std::string{"Welcome! Press H if you need help: 113\n"});
}
void endToEnd_combineWithInvalidFiles() {
	std::streambuf* backup;
	std::stringstream stream;
	backup = std::cout.rdbuf(stream.rdbuf());

	g_keys.push('c');
	g_lines.push("file1 file2");
	g_keys.push('q');
	programLoop();

	std::cout.rdbuf(backup);

	std::string const expected_printout{
		"Welcome! Press H if you need help: 99\n"
		"Specify two or more files, e.g. \"file_1.txt file_2.txt\", without the quotation marks (\"): "
		"Error: File file1 doesn't exist\n"
		"Error: File file2 doesn't exist\n"
		"Error: Not all files exist. No scores combined\n"
		"Error: Failed to combine scores\n"
		"Press H if you need help: 113\n"
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
