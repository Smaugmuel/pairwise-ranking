#include "testing.h"
#include "voting_round.h"

namespace
{

void counterLengthEqualsTotalLength() {
	ASSERT_EQ(
		VotingRound::create({ "i1", "i2", "i3", "i4" }, VotingFormat::Full).value().currentVotingLine().value(),
		std::string{ "(1/6) A: \'i1\'. B: \'i2\'." });
}
void counterLengthIsLessThanTotalLength() {
	ASSERT_EQ(
		VotingRound::create({ "i1", "i2", "i3", "i4", "i5" }, VotingFormat::Full).value().currentVotingLine().value(),
		std::string{ "( 1/10) A: \'i1\'. B: \'i2\'." });
	ASSERT_EQ(
		VotingRound::create({
			"it1", "it2", "it3", "it4", "it5",
			"it6", "it7", "it8", "it9", "i10",
			"i11", "i12", "i13", "i14", "i15", }, VotingFormat::Full).value().currentVotingLine().value(),
			std::string{ "(  1/105) A: \'it1\'. B: \'it2\'." });
}
void itemIsShorterThanLongestItem() {
	ASSERT_EQ(
		VotingRound::create({
			"item1", "shortItem2", "longestItem3" }, VotingFormat::Full).value().currentVotingLine().value(),
		std::string{ "(1/3) A: \'item1\'.        B: \'shortItem2\'.  " });
	ASSERT_EQ(
		VotingRound::create({
			"longestItem1", "i2", "item3" }, VotingFormat::Full).value().currentVotingLine().value(),
		std::string{ "(1/3) A: \'longestItem1\'. B: \'i2\'.          " });
}
void itemLengthIsEqualToHeaderLength() {
	ASSERT_EQ(
		VotingRound::create({
			"a", "b", "c" }, VotingFormat::Full).value().currentVotingLine().value(),
			std::string{ "(1/3) A: \'a\'. B: \'b\'." });
	ASSERT_EQ(
		VotingRound::create({
			"i1", "i2", "i3" }, VotingFormat::Full).value().currentVotingLine().value(),
			std::string{ "(1/3) A: \'i1\'. B: \'i2\'." });
	ASSERT_EQ(
		VotingRound::create({
			"item1", "item2", "item3" }, VotingFormat::Full).value().currentVotingLine().value(),
			std::string{ "(1/3) A: \'item1\'. B: \'item2\'." });
}
void votingIsCompleted() {
	auto voting_round = VotingRound::create(getNItems(2), VotingFormat::Full);
	voting_round.value().vote(Option::A);
	ASSERT_FALSE(voting_round.value().currentVotingLine().has_value());
}
void incompleteVotingIncreasesCounterAndChangesItem() {
	auto voting_round = VotingRound::create(getNItems(3), VotingFormat::Full);
	voting_round.value().vote(Option::A);
	ASSERT_EQ(voting_round.value().currentVotingLine().value(),
		std::string{ "(2/3) A: \'item1\'. B: \'item3\'." });
	voting_round.value().vote(Option::A);
	ASSERT_EQ(voting_round.value().currentVotingLine().value(),
		std::string{ "(3/3) A: \'item2\'. B: \'item3\'." });
}

} // namespace

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(counterLengthEqualsTotalLength);
	RUN_TEST_IF_ARGUMENT_EQUALS(counterLengthIsLessThanTotalLength);
	RUN_TEST_IF_ARGUMENT_EQUALS(itemIsShorterThanLongestItem);
	RUN_TEST_IF_ARGUMENT_EQUALS(itemLengthIsEqualToHeaderLength);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingIsCompleted);
	RUN_TEST_IF_ARGUMENT_EQUALS(incompleteVotingIncreasesCounterAndChangesItem);
	return true;
}

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	if (run_tests(argv[1])) {
		return 1;
	}
	return 0;
}
