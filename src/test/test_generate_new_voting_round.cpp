#include "constants.h"
#include "testing.h"
#include "voting_round.h"

namespace
{

void generateWithTooFewItems() {
	ASSERT_FALSE(VotingRound::create(getNItems(0), VotingFormat::Full).has_value());
	ASSERT_FALSE(VotingRound::create(getNItems(0), VotingFormat::Reduced).has_value());
	ASSERT_FALSE(VotingRound::create(getNItems(1), VotingFormat::Full).has_value());
	ASSERT_FALSE(VotingRound::create(getNItems(1), VotingFormat::Reduced).has_value());
}
void generateWithOnlyEmptyItems() {
	ASSERT_FALSE(VotingRound::create({ "", "" }, VotingFormat::Full).has_value());
}
void generateWithSomeEmptyItems() {
	ASSERT_FALSE(VotingRound::create({ "item1", "item2", "" }, VotingFormat::Full).has_value());
}
void generateWithFullVoting() {
	for (size_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto const voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Full);
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_EQ(voting_round.value().format(), VotingFormat::Full);
	}
}
void generateWithReducedVotingIfTooFewItemsToReduce() {
	for (size_t number_of_items = 2; number_of_items < kMinimumItemsForPruning; number_of_items++) {
		auto const voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Reduced);
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_EQ(voting_round.value().format(), VotingFormat::Reduced);
	}
}
void generateWithReducedVotingIfEnoughItemsToReduce() {
	for (size_t number_of_items = kMinimumItemsForPruning; number_of_items < 25; number_of_items++) {
		auto const voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Reduced);
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_EQ(voting_round.value().format(), VotingFormat::Reduced);
	}
}
void generateWithFullVotingGivesCorrectAmountOfScheduledVotes() {
	ASSERT_EQ(VotingRound::create(getNItems(2), VotingFormat::Full).value().numberOfScheduledVotes(), 1ui32);
	ASSERT_EQ(VotingRound::create(getNItems(3), VotingFormat::Full).value().numberOfScheduledVotes(), 3ui32);
	ASSERT_EQ(VotingRound::create(getNItems(4), VotingFormat::Full).value().numberOfScheduledVotes(), 6ui32);
	ASSERT_EQ(VotingRound::create(getNItems(5), VotingFormat::Full).value().numberOfScheduledVotes(), 10ui32);
	ASSERT_EQ(VotingRound::create(getNItems(6), VotingFormat::Full).value().numberOfScheduledVotes(), 15ui32);
	ASSERT_EQ(VotingRound::create(getNItems(7), VotingFormat::Full).value().numberOfScheduledVotes(), 21ui32);
	ASSERT_EQ(VotingRound::create(getNItems(8), VotingFormat::Full).value().numberOfScheduledVotes(), 28ui32);
	ASSERT_EQ(VotingRound::create(getNItems(9), VotingFormat::Full).value().numberOfScheduledVotes(), 36ui32);
	ASSERT_EQ(VotingRound::create(getNItems(10), VotingFormat::Full).value().numberOfScheduledVotes(), 45ui32);
	ASSERT_EQ(VotingRound::create(getNItems(20), VotingFormat::Full).value().numberOfScheduledVotes(), 190ui32);
	ASSERT_EQ(VotingRound::create(getNItems(30), VotingFormat::Full).value().numberOfScheduledVotes(), 435ui32);
}
void generateWithReducedVotingGivesCorrectAmountOfScheduledVotes() {
	ASSERT_EQ(VotingRound::create(getNItems(2), VotingFormat::Reduced).value().numberOfScheduledVotes(), 1ui32);
	ASSERT_EQ(VotingRound::create(getNItems(3), VotingFormat::Reduced).value().numberOfScheduledVotes(), 3ui32);
	ASSERT_EQ(VotingRound::create(getNItems(4), VotingFormat::Reduced).value().numberOfScheduledVotes(), 6ui32);
	ASSERT_EQ(VotingRound::create(getNItems(5), VotingFormat::Reduced).value().numberOfScheduledVotes(), 10ui32);
	ASSERT_EQ(VotingRound::create(getNItems(6), VotingFormat::Reduced).value().numberOfScheduledVotes(), 9ui32);
	ASSERT_EQ(VotingRound::create(getNItems(7), VotingFormat::Reduced).value().numberOfScheduledVotes(), 14ui32);
	ASSERT_EQ(VotingRound::create(getNItems(8), VotingFormat::Reduced).value().numberOfScheduledVotes(), 12ui32);
	ASSERT_EQ(VotingRound::create(getNItems(9), VotingFormat::Reduced).value().numberOfScheduledVotes(), 18ui32);
	ASSERT_EQ(VotingRound::create(getNItems(10), VotingFormat::Reduced).value().numberOfScheduledVotes(), 15ui32);
	ASSERT_EQ(VotingRound::create(getNItems(20), VotingFormat::Reduced).value().numberOfScheduledVotes(), 30ui32);
	ASSERT_EQ(VotingRound::create(getNItems(30), VotingFormat::Reduced).value().numberOfScheduledVotes(), 45ui32);
}
void generateWithFullVotingGivesCorrectScheduledVotes() {
	// NOTE: Scheduled votes are an implementation detail. To determine whether the matchups were
	// generated as expected, complete the voting and check the matchups afterwards.

	auto const generate_and_get_votes = [](uint32_t number_of_items) -> Votes {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Full);
		while (voting_round.value().vote(Option::A));
		return voting_round.value().votes();
	};

	ASSERT_EQ(generate_and_get_votes(2), Votes{
		Vote{0, 1, Option::A} });
	ASSERT_EQ(generate_and_get_votes(3), Votes{
		Vote{0, 1, Option::A},
		Vote{0, 2, Option::A},
		Vote{1, 2, Option::A} });
	ASSERT_EQ(generate_and_get_votes(4), Votes{
		Vote{0, 1, Option::A},
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{1, 2, Option::A},
		Vote{1, 3, Option::A},
		Vote{2, 3, Option::A} });
	ASSERT_EQ(generate_and_get_votes(5), Votes{
		Vote{0, 1, Option::A},
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{1, 2, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 4, Option::A},
		Vote{2, 3, Option::A},
		Vote{2, 4, Option::A},
		Vote{3, 4, Option::A} });
	ASSERT_EQ(generate_and_get_votes(6), Votes{
		Vote{0, 1, Option::A},
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{1, 2, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{2, 3, Option::A},
		Vote{2, 4, Option::A},
		Vote{2, 5, Option::A},
		Vote{3, 4, Option::A},
		Vote{3, 5, Option::A},
		Vote{4, 5, Option::A} });
	ASSERT_EQ(generate_and_get_votes(7), Votes{
		Vote{0, 1, Option::A},
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{1, 2, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{2, 3, Option::A},
		Vote{2, 4, Option::A},
		Vote{2, 5, Option::A},
		Vote{2, 6, Option::A},
		Vote{3, 4, Option::A},
		Vote{3, 5, Option::A},
		Vote{3, 6, Option::A},
		Vote{4, 5, Option::A},
		Vote{4, 6, Option::A},
		Vote{5, 6, Option::A} });
	ASSERT_EQ(generate_and_get_votes(8), Votes{
		Vote{0, 1, Option::A},
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{0, 7, Option::A},
		Vote{1, 2, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 7, Option::A},
		Vote{2, 3, Option::A},
		Vote{2, 4, Option::A},
		Vote{2, 5, Option::A},
		Vote{2, 6, Option::A},
		Vote{2, 7, Option::A},
		Vote{3, 4, Option::A},
		Vote{3, 5, Option::A},
		Vote{3, 6, Option::A},
		Vote{3, 7, Option::A},
		Vote{4, 5, Option::A},
		Vote{4, 6, Option::A},
		Vote{4, 7, Option::A},
		Vote{5, 6, Option::A},
		Vote{5, 7, Option::A},
		Vote{6, 7, Option::A} });
	ASSERT_EQ(generate_and_get_votes(9), Votes{
		Vote{0, 1, Option::A},
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{0, 7, Option::A},
		Vote{0, 8, Option::A},
		Vote{1, 2, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 7, Option::A},
		Vote{1, 8, Option::A},
		Vote{2, 3, Option::A},
		Vote{2, 4, Option::A},
		Vote{2, 5, Option::A},
		Vote{2, 6, Option::A},
		Vote{2, 7, Option::A},
		Vote{2, 8, Option::A},
		Vote{3, 4, Option::A},
		Vote{3, 5, Option::A},
		Vote{3, 6, Option::A},
		Vote{3, 7, Option::A},
		Vote{3, 8, Option::A},
		Vote{4, 5, Option::A},
		Vote{4, 6, Option::A},
		Vote{4, 7, Option::A},
		Vote{4, 8, Option::A},
		Vote{5, 6, Option::A},
		Vote{5, 7, Option::A},
		Vote{5, 8, Option::A},
		Vote{6, 7, Option::A},
		Vote{6, 8, Option::A},
		Vote{7, 8, Option::A} });
	ASSERT_EQ(generate_and_get_votes(10), Votes{
		Vote{0, 1, Option::A},
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{0, 7, Option::A},
		Vote{0, 8, Option::A},
		Vote{0, 9, Option::A},
		Vote{1, 2, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 7, Option::A},
		Vote{1, 8, Option::A},
		Vote{1, 9, Option::A},
		Vote{2, 3, Option::A},
		Vote{2, 4, Option::A},
		Vote{2, 5, Option::A},
		Vote{2, 6, Option::A},
		Vote{2, 7, Option::A},
		Vote{2, 8, Option::A},
		Vote{2, 9, Option::A},
		Vote{3, 4, Option::A},
		Vote{3, 5, Option::A},
		Vote{3, 6, Option::A},
		Vote{3, 7, Option::A},
		Vote{3, 8, Option::A},
		Vote{3, 9, Option::A},
		Vote{4, 5, Option::A},
		Vote{4, 6, Option::A},
		Vote{4, 7, Option::A},
		Vote{4, 8, Option::A},
		Vote{4, 9, Option::A},
		Vote{5, 6, Option::A},
		Vote{5, 7, Option::A},
		Vote{5, 8, Option::A},
		Vote{5, 9, Option::A},
		Vote{6, 7, Option::A},
		Vote{6, 8, Option::A},
		Vote{6, 9, Option::A},
		Vote{7, 8, Option::A},
		Vote{7, 9, Option::A},
		Vote{8, 9, Option::A} });
	ASSERT_EQ(generate_and_get_votes(11), Votes{
		Vote{0, 1, Option::A},
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{0, 7, Option::A},
		Vote{0, 8, Option::A},
		Vote{0, 9, Option::A},
		Vote{0, 10, Option::A},
		Vote{1, 2, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 7, Option::A},
		Vote{1, 8, Option::A},
		Vote{1, 9, Option::A},
		Vote{1, 10, Option::A},
		Vote{2, 3, Option::A},
		Vote{2, 4, Option::A},
		Vote{2, 5, Option::A},
		Vote{2, 6, Option::A},
		Vote{2, 7, Option::A},
		Vote{2, 8, Option::A},
		Vote{2, 9, Option::A},
		Vote{2, 10, Option::A},
		Vote{3, 4, Option::A},
		Vote{3, 5, Option::A},
		Vote{3, 6, Option::A},
		Vote{3, 7, Option::A},
		Vote{3, 8, Option::A},
		Vote{3, 9, Option::A},
		Vote{3, 10, Option::A},
		Vote{4, 5, Option::A},
		Vote{4, 6, Option::A},
		Vote{4, 7, Option::A},
		Vote{4, 8, Option::A},
		Vote{4, 9, Option::A},
		Vote{4, 10, Option::A},
		Vote{5, 6, Option::A},
		Vote{5, 7, Option::A},
		Vote{5, 8, Option::A},
		Vote{5, 9, Option::A},
		Vote{5, 10, Option::A},
		Vote{6, 7, Option::A},
		Vote{6, 8, Option::A},
		Vote{6, 9, Option::A},
		Vote{6, 10, Option::A},
		Vote{7, 8, Option::A},
		Vote{7, 9, Option::A},
		Vote{7, 10, Option::A},
		Vote{8, 9, Option::A},
		Vote{8, 10, Option::A},
		Vote{9, 10, Option::A} });
	ASSERT_EQ(generate_and_get_votes(12), Votes{
		Vote{0, 1, Option::A},
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{0, 7, Option::A},
		Vote{0, 8, Option::A},
		Vote{0, 9, Option::A},
		Vote{0, 10, Option::A},
		Vote{0, 11, Option::A},
		Vote{1, 2, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 7, Option::A},
		Vote{1, 8, Option::A},
		Vote{1, 9, Option::A},
		Vote{1, 10, Option::A},
		Vote{1, 11, Option::A},
		Vote{2, 3, Option::A},
		Vote{2, 4, Option::A},
		Vote{2, 5, Option::A},
		Vote{2, 6, Option::A},
		Vote{2, 7, Option::A},
		Vote{2, 8, Option::A},
		Vote{2, 9, Option::A},
		Vote{2, 10, Option::A},
		Vote{2, 11, Option::A},
		Vote{3, 4, Option::A},
		Vote{3, 5, Option::A},
		Vote{3, 6, Option::A},
		Vote{3, 7, Option::A},
		Vote{3, 8, Option::A},
		Vote{3, 9, Option::A},
		Vote{3, 10, Option::A},
		Vote{3, 11, Option::A},
		Vote{4, 5, Option::A},
		Vote{4, 6, Option::A},
		Vote{4, 7, Option::A},
		Vote{4, 8, Option::A},
		Vote{4, 9, Option::A},
		Vote{4, 10, Option::A},
		Vote{4, 11, Option::A},
		Vote{5, 6, Option::A},
		Vote{5, 7, Option::A},
		Vote{5, 8, Option::A},
		Vote{5, 9, Option::A},
		Vote{5, 10, Option::A},
		Vote{5, 11, Option::A},
		Vote{6, 7, Option::A},
		Vote{6, 8, Option::A},
		Vote{6, 9, Option::A},
		Vote{6, 10, Option::A},
		Vote{6, 11, Option::A},
		Vote{7, 8, Option::A},
		Vote{7, 9, Option::A},
		Vote{7, 10, Option::A},
		Vote{7, 11, Option::A},
		Vote{8, 9, Option::A},
		Vote{8, 10, Option::A},
		Vote{8, 11, Option::A},
		Vote{9, 10, Option::A},
		Vote{9, 11, Option::A},
		Vote{10, 11, Option::A} });
}

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(generateWithTooFewItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(generateWithOnlyEmptyItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(generateWithSomeEmptyItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(generateWithFullVoting);
	RUN_TEST_IF_ARGUMENT_EQUALS(generateWithReducedVotingIfTooFewItemsToReduce);
	RUN_TEST_IF_ARGUMENT_EQUALS(generateWithReducedVotingIfEnoughItemsToReduce);
	RUN_TEST_IF_ARGUMENT_EQUALS(generateWithFullVotingGivesCorrectAmountOfScheduledVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(generateWithReducedVotingGivesCorrectAmountOfScheduledVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(generateWithFullVotingGivesCorrectScheduledVotes);
	return true;
}

} // namespace

auto test_generate_new_voting_round(std::string const& test_case) -> int {
	if (run_tests(test_case)) {
		return 1;
	}
	return 0;
}
