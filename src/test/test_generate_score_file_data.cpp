#include "testing.h"
#include "score_helpers.h"

namespace
{

void noScores() {
	ASSERT_EQ(generateScoreFileData({}), std::vector<std::string>{});
}
void oneScore() {
	ASSERT_EQ(generateScoreFileData({ Score{ "item1", 3, 6 } }), std::vector<std::string>{
		"3 6 item1"
	});
}
void oneScoreWithSpacesInItemName() {
	ASSERT_EQ(generateScoreFileData({ Score{ "item with space", 3, 6 } }), std::vector<std::string>{
		"3 6 item with space"
	});
}
void multipleScores() {
	ASSERT_EQ(generateScoreFileData({ Score{ "item1", 3, 6 }, Score{ "item2", 71, 4 } }), std::vector<std::string>{
		"3 6 item1",
			"71 4 item2"
	});
}

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(noScores);
	RUN_TEST_IF_ARGUMENT_EQUALS(oneScore);
	RUN_TEST_IF_ARGUMENT_EQUALS(oneScoreWithSpacesInItemName);
	RUN_TEST_IF_ARGUMENT_EQUALS(multipleScores);
	return true;
}

} // namespace

auto test_generate_score_file_data(std::string const& test_case) -> int {
	if (run_tests(test_case)) {
		return 1;
	}
	return 0;
}
