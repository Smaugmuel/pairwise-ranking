#include "testing.h"
#include "voting_round.h"

namespace
{

void noLinesToParse() {
	ASSERT_FALSE(VotingRound::create({}).has_value());
}

// Items tests
void noItemsBeforeEmptyLine() {
	ASSERT_FALSE(VotingRound::create({
		"",
		"1",
		"full" }).has_value());
}
void fewerThanTwoItems() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"",
		"1",
		"full" }).has_value());
}
void itemAppearsMultipleTimes() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item1",
		"",
		"1",
		"full" }).has_value());
}
void anItemIsEmpty() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"",
		"item2",
		"",
		"1",
		"full" }).has_value());
}
void noEmptyLineBetweenItemsAndSeed() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"1",
		"full" }).has_value());
}

// Seed tests
void seedIsMissing() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"full" }).has_value());
}
void seedIsEmpty() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"full" }).has_value());
}
void seedIsNotAPositiveInteger() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"a",
		"full" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"-1",
		"full" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"!",
		"full" }).has_value());
}

// Reduced voting setting tests
void fullVoting() {
	std::optional<VotingRound> const voting_round = VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"full" });
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_EQ(voting_round.value().format(), VotingFormat::Full);
}
void reducedVotingWithTooFewItems() {
	std::optional<VotingRound> const voting_round = VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"reduced" });
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_EQ(voting_round.value().format(), VotingFormat::Reduced);
}
void reducedVotingWithEnoughItems() {
	std::optional<VotingRound> const voting_round = VotingRound::create({
		"item1",
		"item2",
		"item3",
		"item4",
		"item5",
		"item6",
		"",
		"1",
		"reduced" });
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_EQ(voting_round.value().format(), VotingFormat::Reduced);
}
void reducedVotingSettingIsMissing() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1" }).has_value());
}
void reducedVotingSettingIsEmpty() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"" }).has_value());
}
void reducedVotingSettingIsInvalid() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		" " }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"a" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"1" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"reduce" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"reduced " }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"fill" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"full " }).has_value());
}

// Votes tests
void noVotes() {
	std::optional<VotingRound> const voting_round = VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"full" });
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_EQ(voting_round.value().votes().size(), 0ui64);
}
void voteDoesNotHaveThreeIntegers() {
	std::vector<std::string> const base_lines{
		"item1",
		"item2",
		"",
		"1",
		"full" };
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ " " }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "0" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "a" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "a b" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "a 1" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "0 b" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "0 1" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "a b c" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "a b 0" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "a 1 c" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "a 1 0" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "0 b c" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "0 b 0" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "0 1 c" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "0 1 0 d" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "0 1 0 1" }).has_value());
}
void voteOptionIndicesAreGreaterThanNumberOfItems() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"full",
		"0 2 0" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"full",
		"2 0 0" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"item3",
		"",
		"1",
		"full",
		"0 3 0" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"item3",
		"",
		"1",
		"full",
		"3 0 0" }).has_value());
}
void moreVotesThanPossible() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"",
		"1",
		"full",
		"0 1 0",
		"1 0 0" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"item2",
		"",
		"1",
		"full",
		"0 1 0",
		"0 2 0",
		"1 2 0",
		"1 0 0" }).has_value());
}
void sameMatchupMultipleTimes() {
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"item3",
		"",
		"1",
		"full",
		"0 1 0",
		"0 1 0" }).has_value());
	ASSERT_FALSE(VotingRound::create({
		"item1",
		"item2",
		"item3",
		"",
		"1",
		"full",
		"0 1 0",
		"1 0 0" }).has_value());
}
void chosenVoteIsNotZeroOrOne() {
	std::vector<std::string> const base_lines{
		"item1",
		"item2",
		"",
		"1",
		"full" };
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "1 2 2" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "1 2 -0" }).has_value());
	ASSERT_FALSE(VotingRound::create(base_lines + std::string{ "1 2 -1" }).has_value());
}

// Combined tests
auto hasItem(Items const& items, Item const& item) -> bool {
	return std::find(items.begin(), items.end(), item) != items.end();
}
void fourItemsAndReducedVotingAndFourVotes() {
	std::optional<VotingRound> const voting_round = VotingRound::create({
		"item1",
		"item 2",
		"itemthree",
		"item four",
		"",
		"12345678",
		"reduced",
		"1 3 0",
		"0 1 1",
		"2 3 0",
		"0 2 1" });
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item1"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item 2"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "itemthree"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item four"));
	ASSERT_EQ(voting_round.value().originalItemOrder(), Items{ "item1", "item 2", "itemthree", "item four" });
	ASSERT_EQ(voting_round.value().seed(), 12345678ui32);
	ASSERT_EQ(voting_round.value().format(), VotingFormat::Reduced);
	ASSERT_EQ(voting_round.value().numberOfScheduledVotes(), 6ui32);
	ASSERT_EQ(voting_round.value().votes(), Votes{
		Vote{1, 3, Option::A},
		Vote{0, 1, Option::B},
		Vote{2, 3, Option::A},
		Vote{0, 2, Option::B} });
}
void sevenItemsAndReducedVotingAndFourVotes() {
	std::optional<VotingRound> const voting_round = VotingRound::create({
		"item1",
		"item 2",
		"itemthree",
		"item four",
		"i5",
		"sixth",
		"se7en",
		"",
		"12345678",
		"reduced",
		"1 3 0",
		"0 1 1",
		"2 3 0",
		"0 2 1" });
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item1"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item 2"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "itemthree"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item four"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "i5"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "sixth"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "se7en"));
	ASSERT_EQ(voting_round.value().originalItemOrder(), Items{ "item1", "item 2", "itemthree", "item four", "i5", "sixth", "se7en" });
	ASSERT_EQ(voting_round.value().seed(), 12345678ui32);
	ASSERT_EQ(voting_round.value().format(), VotingFormat::Reduced);
	ASSERT_EQ(voting_round.value().numberOfScheduledVotes(), 14ui32);
	ASSERT_EQ(voting_round.value().votes(), Votes{
		Vote{1, 3, Option::A},
		Vote{0, 1, Option::B},
		Vote{2, 3, Option::A},
		Vote{0, 2, Option::B} });
}
void fourItemsAndFullVotingAndOneVote() {
	std::optional<VotingRound> const voting_round = VotingRound::create({
		"item1",
		"item 2",
		"itemthree",
		"item four",
		"",
		"12345678",
		"full",
		"1 3 0" });
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item1"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item 2"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "itemthree"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item four"));
	ASSERT_EQ(voting_round.value().originalItemOrder(), Items{ "item1", "item 2", "itemthree", "item four" });
	ASSERT_EQ(voting_round.value().seed(), 12345678ui32);
	ASSERT_EQ(voting_round.value().format(), VotingFormat::Full);
	ASSERT_EQ(voting_round.value().numberOfScheduledVotes(), 6ui32);
	ASSERT_EQ(voting_round.value().votes(), Votes{ Vote{1, 3, Option::A} });
}
void fourItemsAndFullVotingAndZeroVotes() {
	std::optional<VotingRound> voting_round = VotingRound::create({
		"item1",
		"item 2",
		"itemthree",
		"item four",
		"",
		"12345678",
		"full" });
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item1"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item 2"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "itemthree"));
	ASSERT_TRUE(hasItem(voting_round.value().items(), "item four"));
	ASSERT_EQ(voting_round.value().originalItemOrder(), Items{ "item1", "item 2", "itemthree", "item four" });
	ASSERT_EQ(voting_round.value().seed(), 12345678ui32);
	ASSERT_EQ(voting_round.value().format(), VotingFormat::Full);
	ASSERT_EQ(voting_round.value().numberOfScheduledVotes(), 6ui32);
	ASSERT_TRUE(voting_round.value().votes().empty());
}

} // namespace

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(noLinesToParse);
	RUN_TEST_IF_ARGUMENT_EQUALS(noItemsBeforeEmptyLine);
	RUN_TEST_IF_ARGUMENT_EQUALS(fewerThanTwoItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(itemAppearsMultipleTimes);
	RUN_TEST_IF_ARGUMENT_EQUALS(anItemIsEmpty);
	RUN_TEST_IF_ARGUMENT_EQUALS(noEmptyLineBetweenItemsAndSeed);
	RUN_TEST_IF_ARGUMENT_EQUALS(seedIsMissing);
	RUN_TEST_IF_ARGUMENT_EQUALS(seedIsEmpty);
	RUN_TEST_IF_ARGUMENT_EQUALS(seedIsNotAPositiveInteger);
	RUN_TEST_IF_ARGUMENT_EQUALS(fullVoting);
	RUN_TEST_IF_ARGUMENT_EQUALS(reducedVotingWithTooFewItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(reducedVotingWithEnoughItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(reducedVotingSettingIsMissing);
	RUN_TEST_IF_ARGUMENT_EQUALS(reducedVotingSettingIsEmpty);
	RUN_TEST_IF_ARGUMENT_EQUALS(reducedVotingSettingIsInvalid);
	RUN_TEST_IF_ARGUMENT_EQUALS(noVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(voteDoesNotHaveThreeIntegers);
	RUN_TEST_IF_ARGUMENT_EQUALS(voteOptionIndicesAreGreaterThanNumberOfItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(moreVotesThanPossible);
	RUN_TEST_IF_ARGUMENT_EQUALS(sameMatchupMultipleTimes);
	RUN_TEST_IF_ARGUMENT_EQUALS(chosenVoteIsNotZeroOrOne);
	RUN_TEST_IF_ARGUMENT_EQUALS(fourItemsAndReducedVotingAndFourVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(sevenItemsAndReducedVotingAndFourVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(fourItemsAndFullVotingAndOneVote);
	RUN_TEST_IF_ARGUMENT_EQUALS(fourItemsAndFullVotingAndZeroVotes);
	return true;
}

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	if (run_tests(argv[1])) {
		return 1;
	}
	return 0;
}
