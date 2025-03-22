#include "testing.h"

#include <string>
#include <queue>

#include "mocks/output_catcher.h"
#include "program_loop.h"

extern std::queue<char> g_keys;
extern std::queue<std::string> g_lines;

namespace
{

void endToEnd_quit() {
	g_keys.push('q');

	OutputCatcher output;

	programLoop();

	ASSERT_EQ(output, std::string{"Welcome! Press H if you need help: 113\n"});
}
void endToEnd_combineWithInvalidFiles() {
	g_keys.push('c');
	g_lines.push("file1 file2");
	g_keys.push('q');

	OutputCatcher output;

	programLoop();

	std::string const expected_printout{
		"Welcome! Press H if you need help: 99\n"
		"Specify two or more files, e.g. \"file_1.txt file_2.txt\", without the quotation marks (\"): "
		"Error: File file1 doesn't exist\n"
		"Error: File file2 doesn't exist\n"
		"Error: Not all files exist. No scores combined\n"
		"Error: Failed to combine scores\n"
		"Press H if you need help: 113\n"
	};
	ASSERT_EQ(output, expected_printout);
}

} // namespace

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_quit);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_combineWithInvalidFiles);
	return 1;
}
