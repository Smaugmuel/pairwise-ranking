#include <filesystem>
#include <fstream>

#include "helpers.h"
#include "testing.h"

namespace
{
constexpr auto kTestFileName = "placeholder_file_name.txt";

void savingAndLoadingLines() {
	std::vector<std::string> const original_lines{
		"List",
		"of",
		"lines"
	};
	ASSERT_TRUE(saveFile(kTestFileName, original_lines));
	auto const loaded_lines = loadFile(kTestFileName);
	ASSERT_EQ(loaded_lines, original_lines);
	std::filesystem::remove(kTestFileName);
}
void savingAndLoadingLinesWithEmptyLines() {
	std::vector<std::string> const original_lines{
		"",
		"List",
		"of",
		"lines",
		"",
		"with",
		"some",
		"",
		"",
		"empty",
		"lines",
		""
	};
	saveFile(kTestFileName, original_lines);
	auto const loaded_lines = loadFile(kTestFileName);
	ASSERT_EQ(loaded_lines, original_lines);
	std::filesystem::remove(kTestFileName);
}
void savingAndLoadingOnlyEmptyLines() {
	std::vector<std::string> const original_lines{
		"",
		"",
		"",
		"",
		""
	};
	saveFile(kTestFileName, original_lines);
	auto const loaded_lines = loadFile(kTestFileName);
	ASSERT_EQ(loaded_lines, original_lines);
	std::filesystem::remove(kTestFileName);
}
void savingNoLines() {
	std::vector<std::string> const lines{};
	ASSERT_FALSE(saveFile(kTestFileName, lines));
	ASSERT_FALSE(std::filesystem::exists(kTestFileName));
}
void loadingNoLines() {
	std::ofstream file(kTestFileName);
	file.close();
	ASSERT_TRUE(loadFile(kTestFileName).empty());
	std::filesystem::remove(kTestFileName);
}
void savingToExistingFile() {
	std::vector<std::string> const original_lines{ "Content" };
	saveFile(kTestFileName, original_lines);
	ASSERT_TRUE(saveFile(kTestFileName, { "More", "Content" }));
	auto const loaded_lines = loadFile(kTestFileName);
	ASSERT_EQ(loaded_lines, std::vector<std::string>{ "More", "Content" });
	std::filesystem::remove(kTestFileName);
}
void loadingNonExistingFile() {
	ASSERT_FALSE(std::filesystem::exists(kTestFileName));
	ASSERT_TRUE(loadFile(kTestFileName).empty());
}

} // namespace

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	std::filesystem::remove(kTestFileName);

	RUN_TEST_IF_ARGUMENT_EQUALS(savingAndLoadingLines);
	RUN_TEST_IF_ARGUMENT_EQUALS(savingAndLoadingLinesWithEmptyLines);
	RUN_TEST_IF_ARGUMENT_EQUALS(savingAndLoadingOnlyEmptyLines);
	RUN_TEST_IF_ARGUMENT_EQUALS(savingNoLines);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadingNoLines);
	RUN_TEST_IF_ARGUMENT_EQUALS(savingToExistingFile);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadingNonExistingFile);

	std::filesystem::remove(kTestFileName);
	return 0;
}