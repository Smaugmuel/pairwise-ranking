#include <filesystem>
#include <fstream>

#include "score_helpers.h"
#include "testing.h"

namespace
{

constexpr auto kTestFileName = "placeholder_file_name.txt";

void saveWhenNoScores() {
	std::filesystem::remove(kTestFileName);

	ASSERT_FALSE(saveScores({}, kTestFileName));
	ASSERT_FALSE(std::filesystem::exists(kTestFileName));
}
void saveWhenSomeScores() {
	std::filesystem::remove(kTestFileName);

	Scores const scores{ Score{ "item1", 3, 4 }, Score{ "item3", 1, 3 } , Score{ "item2", 7, 0 } };
	ASSERT_TRUE(saveScores(scores, kTestFileName));
	ASSERT_TRUE(std::filesystem::exists(kTestFileName));

	std::filesystem::remove(kTestFileName);
}

} // namespace

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(saveWhenNoScores);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveWhenSomeScores);
	return true;
}

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	std::filesystem::remove(kTestFileName);
	if (run_tests(argv[1])) {
		return 1;
	}
	std::filesystem::remove(kTestFileName);
	return 0;
}
