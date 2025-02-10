#include "functions.h"
#include "testing.h"

namespace
{

void counterLengthEqualsTotalLength() {
	ASSERT_EQ(
		currentVotingLine(generateNewVotingRound({ "i1", "i2", "i3", "i4" }, false).value()).value(),
		std::string{ "(1/6) H: help. A: \'i1\'. B: \'i2\'. Your choice: " });
}
void counterLengthIsLessThanTotalLength() {
	ASSERT_EQ(
		currentVotingLine(generateNewVotingRound({ "i1", "i2", "i3", "i4", "i5" }, false).value()).value(),
		std::string{ "( 1/10) H: help. A: \'i1\'. B: \'i2\'. Your choice: " });
	ASSERT_EQ(
		currentVotingLine(generateNewVotingRound({
			"it1", "it2", "it3", "it4", "it5",
			"it6", "it7", "it8", "it9", "i10",
			"i11", "i12", "i13", "i14", "i15", }, false).value()).value(),
			std::string{ "(  1/105) H: help. A: \'it1\'. B: \'it2\'. Your choice: " });
}
void itemIsShorterThanLongestItem() {
	ASSERT_EQ(
		currentVotingLine(generateNewVotingRound({
			"item1", "shortItem2", "longestItem3" }, false).value()).value(),
		std::string{ "(1/3) H: help. A: \'item1\'.        B: \'shortItem2\'.   Your choice: " });
	ASSERT_EQ(
		currentVotingLine(generateNewVotingRound({
			"longestItem1", "i2", "item3" }, false).value()).value(),
		std::string{ "(1/3) H: help. A: \'longestItem1\'. B: \'i2\'.           Your choice: " });
}
void itemLengthIsEqualToHeaderLength() {
	ASSERT_EQ(
		currentVotingLine(generateNewVotingRound({
			"a", "b", "c" }, false).value()).value(),
			std::string{ "(1/3) H: help. A: \'a\'. B: \'b\'. Your choice: " });
	ASSERT_EQ(
		currentVotingLine(generateNewVotingRound({
			"i1", "i2", "i3" }, false).value()).value(),
			std::string{ "(1/3) H: help. A: \'i1\'. B: \'i2\'. Your choice: " });
	ASSERT_EQ(
		currentVotingLine(generateNewVotingRound({
			"item1", "item2", "item3" }, false).value()).value(),
			std::string{ "(1/3) H: help. A: \'item1\'. B: \'item2\'. Your choice: " });
}
void votingIsCompleted() {
	auto voting_round = generateNewVotingRound({ "item1", "item2" }, false);
	vote(voting_round, Option::A);
	ASSERT_FALSE(currentVotingLine(voting_round.value()).has_value());
}
void incompleteVotingIncreasesCounterAndChangesItem() {
	auto voting_round = generateNewVotingRound({ "item1", "item2", "item3" }, false);
	vote(voting_round, Option::A);
	ASSERT_EQ(currentVotingLine(voting_round.value()).value(),
		std::string{ "(2/3) H: help. A: \'item1\'. B: \'item3\'. Your choice: " });
	vote(voting_round, Option::A);
	ASSERT_EQ(currentVotingLine(voting_round.value()).value(),
		std::string{ "(3/3) H: help. A: \'item2\'. B: \'item3\'. Your choice: " });
}

}

int main() {
	counterLengthEqualsTotalLength();
	counterLengthIsLessThanTotalLength();
	itemIsShorterThanLongestItem();
	itemLengthIsEqualToHeaderLength();
	votingIsCompleted();
	incompleteVotingIncreasesCounterAndChangesItem();
	return 0;
}