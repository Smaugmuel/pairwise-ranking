#include "testing.h"
#include "score_helpers.h"

namespace
{

void noScores() {
	ASSERT_EQ(createScoreTable({}), std::string{
		"------------------------\n"
		"| Item | Wins | Losses |\n"
		"|------|------|--------|\n"
		"------------------------\n" });
}
void alreadySorted() {
	Scores const scores{
		{ "item1", 3, 0 },
		{ "item2", 2, 1 },
		{ "item3", 1, 2 },
		{ "item4", 0, 3 } };
	ASSERT_EQ(createScoreTable(scores), std::string{
		"-------------------------\n"
		"|  Item | Wins | Losses |\n"
		"|-------|------|--------|\n"
		"| item1 |    3 |      0 |\n"
		"| item2 |    2 |      1 |\n"
		"| item3 |    1 |      2 |\n"
		"| item4 |    0 |      3 |\n"
		"-------------------------\n" });
}
void reversedOrder() {
	Scores const scores{
		{ "item1", 0, 3 },
		{ "item2", 1, 2 },
		{ "item3", 2, 1 },
		{ "item4", 3, 0 } };
	ASSERT_EQ(createScoreTable(scores), std::string{
		"-------------------------\n"
		"|  Item | Wins | Losses |\n"
		"|-------|------|--------|\n"
		"| item4 |    3 |      0 |\n"
		"| item3 |    2 |      1 |\n"
		"| item2 |    1 |      2 |\n"
		"| item1 |    0 |      3 |\n"
		"-------------------------\n" });
}
void winLossDifferenceIsZeroAndTotalIsDifferent() {
	Scores const scores{
		{ "item2", 3, 3 },
		{ "item1", 1, 1 } };
	ASSERT_EQ(createScoreTable(scores), std::string{
		"-------------------------\n"
		"|  Item | Wins | Losses |\n"
		"|-------|------|--------|\n"
		"| item1 |    1 |      1 |\n"
		"| item2 |    3 |      3 |\n"
		"-------------------------\n" });
}
void winLossDifferenceIsZeroAndTotalIsTheSame() {
	Scores const scores{
		{ "item2", 2, 2 },
		{ "item1", 2, 2 } };
	ASSERT_EQ(createScoreTable(scores), std::string{
		"-------------------------\n"
		"|  Item | Wins | Losses |\n"
		"|-------|------|--------|\n"
		"| item1 |    2 |      2 |\n"
		"| item2 |    2 |      2 |\n"
		"-------------------------\n" });
}
void winLossDifferenceIsEqualAndPositive() {
	Scores const scores{
		{ "item1", 12, 6 },
		{ "item2", 7, 1 },
		{ "item3", 9, 3 } };
	ASSERT_EQ(createScoreTable(scores), std::string{
		"-------------------------\n"
		"|  Item | Wins | Losses |\n"
		"|-------|------|--------|\n"
		"| item2 |    7 |      1 |\n"
		"| item3 |    9 |      3 |\n"
		"| item1 |   12 |      6 |\n"
		"-------------------------\n" });
}
void winLossDifferenceIsEqualAndNegative() {
	Scores const scores{
		{ "item1", 6, 12 },
		{ "item2", 1, 7 },
		{ "item3", 3, 9 } };
	ASSERT_EQ(createScoreTable(scores), std::string{
		"-------------------------\n"
		"|  Item | Wins | Losses |\n"
		"|-------|------|--------|\n"
		"| item1 |    6 |     12 |\n"
		"| item3 |    3 |      9 |\n"
		"| item2 |    1 |      7 |\n"
		"-------------------------\n" });
}
void shortItemNameAndWinsAndLosses() {
	ASSERT_EQ(createScoreTable(Scores{ { "i1", 3, 0 } }), std::string{
		"------------------------\n"
		"| Item | Wins | Losses |\n"
		"|------|------|--------|\n"
		"|   i1 |    3 |      0 |\n"
		"------------------------\n" });
}
void longItemNameAndWinsAndLosses() {
	ASSERT_EQ(createScoreTable(Scores{ { "longItemName", 123456, 123456789 } }), std::string{
		"-------------------------------------\n"
		"|         Item |   Wins |    Losses |\n"
		"|--------------|--------|-----------|\n"
		"| longItemName | 123456 | 123456789 |\n"
		"-------------------------------------\n" });
}

// TODO: Add more sorting tests for corner cases:
// - Same item name
// - TBD

} // namespace

auto run_tests(char* argv[]) -> int {
	RUN_TEST_IF_ARGUMENT_EQUALS(noScores);
	RUN_TEST_IF_ARGUMENT_EQUALS(alreadySorted);
	RUN_TEST_IF_ARGUMENT_EQUALS(reversedOrder);
	RUN_TEST_IF_ARGUMENT_EQUALS(winLossDifferenceIsZeroAndTotalIsDifferent);
	RUN_TEST_IF_ARGUMENT_EQUALS(winLossDifferenceIsZeroAndTotalIsTheSame);
	RUN_TEST_IF_ARGUMENT_EQUALS(winLossDifferenceIsEqualAndPositive);
	RUN_TEST_IF_ARGUMENT_EQUALS(winLossDifferenceIsEqualAndNegative);
	RUN_TEST_IF_ARGUMENT_EQUALS(shortItemNameAndWinsAndLosses);
	RUN_TEST_IF_ARGUMENT_EQUALS(longItemNameAndWinsAndLosses);
	return 1;
}

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	if (run_tests(argv) != 0) {
		return 1;
	}
	return 0;
}
