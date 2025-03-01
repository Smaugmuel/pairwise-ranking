#include "constants.h"
#include "helpers.h"
#include "testing.h"
#include "voting_round.h"

namespace
{

void pruningDuringVotingRoundCreationWithTooFewItems() {
	for (size_t number_of_items = 2; number_of_items < kMinimumItemsForPruning; number_of_items++) {
		auto const voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Reduced).value();
		auto const expected_amount = static_cast<uint32_t>(sumOfFirstIntegers(number_of_items - 1));
		ASSERT_EQ(voting_round.numberOfScheduledVotes(), expected_amount);
	}
}
void pruningAmountDuringVotingRoundCreationDependsOnNumberOfItems() {
	for (size_t number_of_items = kMinimumItemsForPruning; number_of_items < 24; number_of_items++) {
		auto const voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Reduced).value();
		auto const number_of_pruned_votes = number_of_items * pruningAmount(number_of_items);
		auto const number_of_votes_after_pruning = sumOfFirstIntegers(number_of_items - 1) - number_of_pruned_votes;
		ASSERT_EQ(voting_round.numberOfScheduledVotes(), static_cast<uint32_t>(number_of_votes_after_pruning));
	}
}
void parseVotingRoundWithPruning() {
	for (size_t number_of_items = kMinimumItemsForPruning; number_of_items < 24; number_of_items++) {
		auto const voting_round = VotingRound::create(getNItems(number_of_items) + std::vector<std::string>{ "", "1", "reduced" });
		ASSERT_EQ(voting_round.value().format(), VotingFormat::Reduced);
		auto const number_of_pruned_votes = number_of_items * pruningAmount(number_of_items);
		auto const number_of_votes_after_pruning = sumOfFirstIntegers(number_of_items - 1) - number_of_pruned_votes;
		ASSERT_EQ(voting_round.value().numberOfScheduledVotes(), static_cast<uint32_t>(number_of_votes_after_pruning));
	}
}
void pruningRemovesCorrectScheduledVotes() {
	// NOTE: Scheduled votes are an implementation detail. To determine whether the matchups were
	// generated as expected, complete the voting and check the matchups afterwards.

	auto const generate_and_get_votes = [](uint32_t number_of_items) -> Votes {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Reduced);
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
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{2, 4, Option::A},
		Vote{2, 5, Option::A},
		Vote{3, 5, Option::A} });
	ASSERT_EQ(generate_and_get_votes(7), Votes{
		Vote{0, 2, Option::A},
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{2, 4, Option::A},
		Vote{2, 5, Option::A},
		Vote{2, 6, Option::A},
		Vote{3, 5, Option::A},
		Vote{3, 6, Option::A},
		Vote{4, 6, Option::A} });
	ASSERT_EQ(generate_and_get_votes(8), Votes{
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{2, 5, Option::A},
		Vote{2, 6, Option::A},
		Vote{2, 7, Option::A},
		Vote{3, 6, Option::A},
		Vote{3, 7, Option::A},
		Vote{4, 7, Option::A} });
	ASSERT_EQ(generate_and_get_votes(9), Votes{
		Vote{0, 3, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{1, 4, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 7, Option::A},
		Vote{2, 5, Option::A},
		Vote{2, 6, Option::A},
		Vote{2, 7, Option::A},
		Vote{2, 8, Option::A},
		Vote{3, 6, Option::A},
		Vote{3, 7, Option::A},
		Vote{3, 8, Option::A},
		Vote{4, 7, Option::A},
		Vote{4, 8, Option::A},
		Vote{5, 8, Option::A} });
	ASSERT_EQ(generate_and_get_votes(10), Votes{
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 7, Option::A},
		Vote{2, 6, Option::A},
		Vote{2, 7, Option::A},
		Vote{2, 8, Option::A},
		Vote{3, 7, Option::A},
		Vote{3, 8, Option::A},
		Vote{3, 9, Option::A},
		Vote{4, 8, Option::A},
		Vote{4, 9, Option::A},
		Vote{5, 9, Option::A} });
	ASSERT_EQ(generate_and_get_votes(11), Votes{
		Vote{0, 4, Option::A},
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{0, 7, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 7, Option::A},
		Vote{1, 8, Option::A},
		Vote{2, 6, Option::A},
		Vote{2, 7, Option::A},
		Vote{2, 8, Option::A},
		Vote{2, 9, Option::A},
		Vote{3, 7, Option::A},
		Vote{3, 8, Option::A},
		Vote{3, 9, Option::A},
		Vote{3, 10, Option::A},
		Vote{4, 8, Option::A},
		Vote{4, 9, Option::A},
		Vote{4, 10, Option::A},
		Vote{5, 9, Option::A},
		Vote{5, 10, Option::A},
		Vote{6, 10, Option::A} });
	ASSERT_EQ(generate_and_get_votes(12), Votes{
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{0, 7, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 7, Option::A},
		Vote{1, 8, Option::A},
		Vote{2, 7, Option::A},
		Vote{2, 8, Option::A},
		Vote{2, 9, Option::A},
		Vote{3, 8, Option::A},
		Vote{3, 9, Option::A},
		Vote{3, 10, Option::A},
		Vote{4, 9, Option::A},
		Vote{4, 10, Option::A},
		Vote{4, 11, Option::A},
		Vote{5, 10, Option::A},
		Vote{5, 11, Option::A},
		Vote{6, 11, Option::A} });
	ASSERT_EQ(generate_and_get_votes(13), Votes{
		Vote{0, 5, Option::A},
		Vote{0, 6, Option::A},
		Vote{0, 7, Option::A},
		Vote{0, 8, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 7, Option::A},
		Vote{1, 8, Option::A},
		Vote{1, 9, Option::A},
		Vote{2, 7, Option::A},
		Vote{2, 8, Option::A},
		Vote{2, 9, Option::A},
		Vote{2, 10, Option::A},
		Vote{3, 8, Option::A},
		Vote{3, 9, Option::A},
		Vote{3, 10, Option::A},
		Vote{3, 11, Option::A},
		Vote{4, 9, Option::A},
		Vote{4, 10, Option::A},
		Vote{4, 11, Option::A},
		Vote{4, 12, Option::A},
		Vote{5, 10, Option::A},
		Vote{5, 11, Option::A},
		Vote{5, 12, Option::A},
		Vote{6, 11, Option::A},
		Vote{6, 12, Option::A},
		Vote{7, 12, Option::A} });
}

} // namsepace

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	RUN_TEST_IF_ARGUMENT_EQUALS(pruningDuringVotingRoundCreationWithTooFewItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(pruningAmountDuringVotingRoundCreationDependsOnNumberOfItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(parseVotingRoundWithPruning);
	RUN_TEST_IF_ARGUMENT_EQUALS(pruningRemovesCorrectScheduledVotes);
	return 0;
}