#include "functions.h"
#include "testing.h"

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

int main() {
	noScores();
	alreadySorted();
	reversedOrder();
	winLossDifferenceIsZeroAndTotalIsDifferent();
	winLossDifferenceIsZeroAndTotalIsTheSame();
	winLossDifferenceIsEqualAndPositive();
	winLossDifferenceIsEqualAndNegative();
	shortItemNameAndWinsAndLosses();
	longItemNameAndWinsAndLosses();
	return 0;
}