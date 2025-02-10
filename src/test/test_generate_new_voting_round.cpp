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
	ASSERT_FALSE(generateNewVotingRound(getNItems(0), false).has_value());
	ASSERT_FALSE(generateNewVotingRound(getNItems(0), true).has_value());
	ASSERT_FALSE(generateNewVotingRound(getNItems(1), false).has_value());
	ASSERT_FALSE(generateNewVotingRound(getNItems(1), true).has_value());
}
void generateWithOnlyEmptyItems() {
	ASSERT_FALSE(generateNewVotingRound({ "", "" }, false).has_value());
}
void generateWithSomeEmptyItems() {
	ASSERT_FALSE(generateNewVotingRound({ "item1", "item2", "" }, false).has_value());
}
void generateWithFullVoting() {
	for (size_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto const voting_round = generateNewVotingRound(getNItems(number_of_items), false);
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_EQ(voting_round.value().reduced_voting, false);
	}
}
void generateWithReducedVotingIfTooFewItemsToReduce() {
	for (size_t number_of_items = 2; number_of_items < kMinimumItemsForPruning; number_of_items++) {
		auto const voting_round = generateNewVotingRound(getNItems(number_of_items), true);
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_FALSE(voting_round.value().reduced_voting);
	}
}
void generateWithReducedVotingIfEnoughItemsToReduce() {
	for (size_t number_of_items = kMinimumItemsForPruning; number_of_items < 25; number_of_items++) {
		auto const voting_round = generateNewVotingRound(getNItems(number_of_items), true);
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_TRUE(voting_round.value().reduced_voting);
	}
}
void generateWithFullVotingGivesCorrectAmountOfScheduledVotes() {
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(2), false).value()), 1ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(3), false).value()), 3ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(4), false).value()), 6ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(5), false).value()), 10ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(6), false).value()), 15ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(7), false).value()), 21ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(8), false).value()), 28ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(9), false).value()), 36ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(10), false).value()), 45ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(20), false).value()), 190ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(30), false).value()), 435ui32);
}
void generateWithReducedVotingGivesCorrectAmountOfScheduledVotes() {
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(2), true).value()), 1ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(3), true).value()), 3ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(4), true).value()), 6ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(5), true).value()), 10ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(6), true).value()), 9ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(7), true).value()), 14ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(8), true).value()), 12ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(9), true).value()), 18ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(10), true).value()), 15ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(20), true).value()), 30ui32);
	ASSERT_EQ(numberOfScheduledVotes(generateNewVotingRound(getNItems(30), true).value()), 45ui32);
}
void generateWithFullVotingGivesCorrectScheduledVotes() {
	auto const generate_and_get_index_pairs = [](uint32_t number_of_items) -> IndexPairs {
		return generateNewVotingRound(getNItems(number_of_items), false).value().index_pairs;
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