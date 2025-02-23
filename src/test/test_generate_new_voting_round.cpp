#include "constants.h"
#include "functions.h"
#include "testing.h"

namespace
{

/* -------------- Test helpers -------------- */
auto getNItems(size_t n) -> Items {
	Items items;
	items.reserve(n);
	for (size_t i = 0; i < n; i++) {
		items.push_back("item" + std::to_string(i + 1));
	}
	return items;
}

/* -------------- Tests -------------- */
void generateWithTooFewItems() {
	ASSERT_FALSE(VotingRound::create(getNItems(0), false).has_value());
	ASSERT_FALSE(VotingRound::create(getNItems(0), true).has_value());
	ASSERT_FALSE(VotingRound::create(getNItems(1), false).has_value());
	ASSERT_FALSE(VotingRound::create(getNItems(1), true).has_value());
}
void generateWithOnlyEmptyItems() {
	ASSERT_FALSE(VotingRound::create({ "", "" }, false).has_value());
}
void generateWithSomeEmptyItems() {
	ASSERT_FALSE(VotingRound::create({ "item1", "item2", "" }, false).has_value());
}
void generateWithFullVoting() {
	for (size_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto const voting_round = VotingRound::create(getNItems(number_of_items), false);
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_EQ(voting_round.value().reduced_voting, false);
	}
}
void generateWithReducedVotingIfTooFewItemsToReduce() {
	for (size_t number_of_items = 2; number_of_items < kMinimumItemsForPruning; number_of_items++) {
		auto const voting_round = VotingRound::create(getNItems(number_of_items), true);
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_FALSE(voting_round.value().reduced_voting);
	}
}
void generateWithReducedVotingIfEnoughItemsToReduce() {
	for (size_t number_of_items = kMinimumItemsForPruning; number_of_items < 25; number_of_items++) {
		auto const voting_round = VotingRound::create(getNItems(number_of_items), true);
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_TRUE(voting_round.value().reduced_voting);
	}
}
void generateWithFullVotingGivesCorrectAmountOfScheduledVotes() {
	ASSERT_EQ(VotingRound::create(getNItems(2), false).value().numberOfScheduledVotes(), 1ui32);
	ASSERT_EQ(VotingRound::create(getNItems(3), false).value().numberOfScheduledVotes(), 3ui32);
	ASSERT_EQ(VotingRound::create(getNItems(4), false).value().numberOfScheduledVotes(), 6ui32);
	ASSERT_EQ(VotingRound::create(getNItems(5), false).value().numberOfScheduledVotes(), 10ui32);
	ASSERT_EQ(VotingRound::create(getNItems(6), false).value().numberOfScheduledVotes(), 15ui32);
	ASSERT_EQ(VotingRound::create(getNItems(7), false).value().numberOfScheduledVotes(), 21ui32);
	ASSERT_EQ(VotingRound::create(getNItems(8), false).value().numberOfScheduledVotes(), 28ui32);
	ASSERT_EQ(VotingRound::create(getNItems(9), false).value().numberOfScheduledVotes(), 36ui32);
	ASSERT_EQ(VotingRound::create(getNItems(10), false).value().numberOfScheduledVotes(), 45ui32);
	ASSERT_EQ(VotingRound::create(getNItems(20), false).value().numberOfScheduledVotes(), 190ui32);
	ASSERT_EQ(VotingRound::create(getNItems(30), false).value().numberOfScheduledVotes(), 435ui32);
}
void generateWithReducedVotingGivesCorrectAmountOfScheduledVotes() {
	ASSERT_EQ(VotingRound::create(getNItems(2), true).value().numberOfScheduledVotes(), 1ui32);
	ASSERT_EQ(VotingRound::create(getNItems(3), true).value().numberOfScheduledVotes(), 3ui32);
	ASSERT_EQ(VotingRound::create(getNItems(4), true).value().numberOfScheduledVotes(), 6ui32);
	ASSERT_EQ(VotingRound::create(getNItems(5), true).value().numberOfScheduledVotes(), 10ui32);
	ASSERT_EQ(VotingRound::create(getNItems(6), true).value().numberOfScheduledVotes(), 9ui32);
	ASSERT_EQ(VotingRound::create(getNItems(7), true).value().numberOfScheduledVotes(), 14ui32);
	ASSERT_EQ(VotingRound::create(getNItems(8), true).value().numberOfScheduledVotes(), 12ui32);
	ASSERT_EQ(VotingRound::create(getNItems(9), true).value().numberOfScheduledVotes(), 18ui32);
	ASSERT_EQ(VotingRound::create(getNItems(10), true).value().numberOfScheduledVotes(), 15ui32);
	ASSERT_EQ(VotingRound::create(getNItems(20), true).value().numberOfScheduledVotes(), 30ui32);
	ASSERT_EQ(VotingRound::create(getNItems(30), true).value().numberOfScheduledVotes(), 45ui32);
}
void generateWithFullVotingGivesCorrectScheduledVotes() {
	auto const generate_and_get_index_pairs = [](uint32_t number_of_items) -> IndexPairs {
		return VotingRound::create(getNItems(number_of_items), false).value().index_pairs;
	};

	ASSERT_EQ(generate_and_get_index_pairs(2), IndexPairs{
		IndexPair{0, 1} });
	ASSERT_EQ(generate_and_get_index_pairs(3), IndexPairs{
		IndexPair{0, 1},
		IndexPair{0, 2},
		IndexPair{1, 2} });
	ASSERT_EQ(generate_and_get_index_pairs(4), IndexPairs{
		IndexPair{0, 1},
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{1, 2},
		IndexPair{1, 3},
		IndexPair{2, 3} });
	ASSERT_EQ(generate_and_get_index_pairs(5), IndexPairs{
		IndexPair{0, 1},
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{1, 2},
		IndexPair{1, 3},
		IndexPair{1, 4},
		IndexPair{2, 3},
		IndexPair{2, 4},
		IndexPair{3, 4} });
	ASSERT_EQ(generate_and_get_index_pairs(6), IndexPairs{
		IndexPair{0, 1},
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{1, 2},
		IndexPair{1, 3},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{2, 3},
		IndexPair{2, 4},
		IndexPair{2, 5},
		IndexPair{3, 4},
		IndexPair{3, 5},
		IndexPair{4, 5} });
	ASSERT_EQ(generate_and_get_index_pairs(7), IndexPairs{
		IndexPair{0, 1},
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{1, 2},
		IndexPair{1, 3},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{2, 3},
		IndexPair{2, 4},
		IndexPair{2, 5},
		IndexPair{2, 6},
		IndexPair{3, 4},
		IndexPair{3, 5},
		IndexPair{3, 6},
		IndexPair{4, 5},
		IndexPair{4, 6},
		IndexPair{5, 6} });
	ASSERT_EQ(generate_and_get_index_pairs(8), IndexPairs{
		IndexPair{0, 1},
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{0, 7},
		IndexPair{1, 2},
		IndexPair{1, 3},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{1, 7},
		IndexPair{2, 3},
		IndexPair{2, 4},
		IndexPair{2, 5},
		IndexPair{2, 6},
		IndexPair{2, 7},
		IndexPair{3, 4},
		IndexPair{3, 5},
		IndexPair{3, 6},
		IndexPair{3, 7},
		IndexPair{4, 5},
		IndexPair{4, 6},
		IndexPair{4, 7},
		IndexPair{5, 6},
		IndexPair{5, 7},
		IndexPair{6, 7} });
	ASSERT_EQ(generate_and_get_index_pairs(9), IndexPairs{
		IndexPair{0, 1},
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{0, 7},
		IndexPair{0, 8},
		IndexPair{1, 2},
		IndexPair{1, 3},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{1, 7},
		IndexPair{1, 8},
		IndexPair{2, 3},
		IndexPair{2, 4},
		IndexPair{2, 5},
		IndexPair{2, 6},
		IndexPair{2, 7},
		IndexPair{2, 8},
		IndexPair{3, 4},
		IndexPair{3, 5},
		IndexPair{3, 6},
		IndexPair{3, 7},
		IndexPair{3, 8},
		IndexPair{4, 5},
		IndexPair{4, 6},
		IndexPair{4, 7},
		IndexPair{4, 8},
		IndexPair{5, 6},
		IndexPair{5, 7},
		IndexPair{5, 8},
		IndexPair{6, 7},
		IndexPair{6, 8},
		IndexPair{7, 8} });
	ASSERT_EQ(generate_and_get_index_pairs(10), IndexPairs{
		IndexPair{0, 1},
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{0, 7},
		IndexPair{0, 8},
		IndexPair{0, 9},
		IndexPair{1, 2},
		IndexPair{1, 3},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{1, 7},
		IndexPair{1, 8},
		IndexPair{1, 9},
		IndexPair{2, 3},
		IndexPair{2, 4},
		IndexPair{2, 5},
		IndexPair{2, 6},
		IndexPair{2, 7},
		IndexPair{2, 8},
		IndexPair{2, 9},
		IndexPair{3, 4},
		IndexPair{3, 5},
		IndexPair{3, 6},
		IndexPair{3, 7},
		IndexPair{3, 8},
		IndexPair{3, 9},
		IndexPair{4, 5},
		IndexPair{4, 6},
		IndexPair{4, 7},
		IndexPair{4, 8},
		IndexPair{4, 9},
		IndexPair{5, 6},
		IndexPair{5, 7},
		IndexPair{5, 8},
		IndexPair{5, 9},
		IndexPair{6, 7},
		IndexPair{6, 8},
		IndexPair{6, 9},
		IndexPair{7, 8},
		IndexPair{7, 9},
		IndexPair{8, 9} });
	ASSERT_EQ(generate_and_get_index_pairs(11), IndexPairs{
		IndexPair{0, 1},
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{0, 7},
		IndexPair{0, 8},
		IndexPair{0, 9},
		IndexPair{0, 10},
		IndexPair{1, 2},
		IndexPair{1, 3},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{1, 7},
		IndexPair{1, 8},
		IndexPair{1, 9},
		IndexPair{1, 10},
		IndexPair{2, 3},
		IndexPair{2, 4},
		IndexPair{2, 5},
		IndexPair{2, 6},
		IndexPair{2, 7},
		IndexPair{2, 8},
		IndexPair{2, 9},
		IndexPair{2, 10},
		IndexPair{3, 4},
		IndexPair{3, 5},
		IndexPair{3, 6},
		IndexPair{3, 7},
		IndexPair{3, 8},
		IndexPair{3, 9},
		IndexPair{3, 10},
		IndexPair{4, 5},
		IndexPair{4, 6},
		IndexPair{4, 7},
		IndexPair{4, 8},
		IndexPair{4, 9},
		IndexPair{4, 10},
		IndexPair{5, 6},
		IndexPair{5, 7},
		IndexPair{5, 8},
		IndexPair{5, 9},
		IndexPair{5, 10},
		IndexPair{6, 7},
		IndexPair{6, 8},
		IndexPair{6, 9},
		IndexPair{6, 10},
		IndexPair{7, 8},
		IndexPair{7, 9},
		IndexPair{7, 10},
		IndexPair{8, 9},
		IndexPair{8, 10},
		IndexPair{9, 10} });
	ASSERT_EQ(generate_and_get_index_pairs(12), IndexPairs{
		IndexPair{0, 1},
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{0, 7},
		IndexPair{0, 8},
		IndexPair{0, 9},
		IndexPair{0, 10},
		IndexPair{0, 11},
		IndexPair{1, 2},
		IndexPair{1, 3},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{1, 7},
		IndexPair{1, 8},
		IndexPair{1, 9},
		IndexPair{1, 10},
		IndexPair{1, 11},
		IndexPair{2, 3},
		IndexPair{2, 4},
		IndexPair{2, 5},
		IndexPair{2, 6},
		IndexPair{2, 7},
		IndexPair{2, 8},
		IndexPair{2, 9},
		IndexPair{2, 10},
		IndexPair{2, 11},
		IndexPair{3, 4},
		IndexPair{3, 5},
		IndexPair{3, 6},
		IndexPair{3, 7},
		IndexPair{3, 8},
		IndexPair{3, 9},
		IndexPair{3, 10},
		IndexPair{3, 11},
		IndexPair{4, 5},
		IndexPair{4, 6},
		IndexPair{4, 7},
		IndexPair{4, 8},
		IndexPair{4, 9},
		IndexPair{4, 10},
		IndexPair{4, 11},
		IndexPair{5, 6},
		IndexPair{5, 7},
		IndexPair{5, 8},
		IndexPair{5, 9},
		IndexPair{5, 10},
		IndexPair{5, 11},
		IndexPair{6, 7},
		IndexPair{6, 8},
		IndexPair{6, 9},
		IndexPair{6, 10},
		IndexPair{6, 11},
		IndexPair{7, 8},
		IndexPair{7, 9},
		IndexPair{7, 10},
		IndexPair{7, 11},
		IndexPair{8, 9},
		IndexPair{8, 10},
		IndexPair{8, 11},
		IndexPair{9, 10},
		IndexPair{9, 11},
		IndexPair{10, 11} });
}

}

int main() {
	generateWithTooFewItems();
	generateWithOnlyEmptyItems();
	generateWithSomeEmptyItems();
	generateWithFullVoting();
	generateWithReducedVotingIfTooFewItemsToReduce();
	generateWithReducedVotingIfEnoughItemsToReduce();
	generateWithFullVotingGivesCorrectAmountOfScheduledVotes();
	generateWithReducedVotingGivesCorrectAmountOfScheduledVotes();
	generateWithFullVotingGivesCorrectScheduledVotes();

	return 0;
}