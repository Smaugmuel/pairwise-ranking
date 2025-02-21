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
auto hasAnyAdjacentPair(VotingRound const& voting_round) -> bool {
	auto number_of_items = voting_round.index_pairs.size();
	auto is_adjacent = [number_of_items](IndexPair const& index_pair) -> bool {
		if (std::labs(static_cast<int>(index_pair.first) - static_cast<int>(index_pair.second)) == 1) {
			return true;
		}
		if (index_pair.first == 0 && index_pair.second == number_of_items - 1) {
			return true;
		}
		return false;
	};
	auto it = std::find_if(voting_round.index_pairs.begin(), voting_round.index_pairs.end(), is_adjacent);
	return it != voting_round.index_pairs.end();
}

template<typename T>
auto operator+(std::vector<T> const& a, std::vector<T> const& b) -> decltype(auto) {
	std::vector<T> vec;
	vec.reserve(a.size() + b.size());
	vec.insert(vec.end(), a.begin(), a.end());
	vec.insert(vec.end(), b.begin(), b.end());
	return vec;
}

/* -------------- Tests -------------- */
void pruningDuringVotingRoundCreationWithTooFewItems() {
	for (size_t number_of_items = 2; number_of_items < kMinimumItemsForPruning; number_of_items++) {
		auto const voting_round = generateNewVotingRound(getNItems(number_of_items), true).value();
		auto const expected_amount = static_cast<uint32_t>(sumOfFirstIntegers(number_of_items - 1));
		ASSERT_EQ(numberOfScheduledVotes(voting_round), expected_amount);
	}
}
void pruningAfterVotingRoundCreationWithTooFewItems() {
	for (size_t number_of_items = 2; number_of_items < kMinimumItemsForPruning; number_of_items++) {
		auto voting_round = generateNewVotingRound(getNItems(number_of_items), false).value();
		pruneVotes(voting_round);
		auto const expected_amount = static_cast<uint32_t>(sumOfFirstIntegers(number_of_items - 1));
		ASSERT_EQ(numberOfScheduledVotes(voting_round), expected_amount);
	}
}
void pruningDuringVotingRoundCreationRemovesAdjacentPairs() {
	for (size_t number_of_items = kMinimumItemsForPruning; number_of_items < 24; number_of_items++) {
		auto const voting_round = generateNewVotingRound(getNItems(number_of_items), true).value();
		ASSERT_FALSE(hasAnyAdjacentPair(voting_round));
	}
}
void pruningAfterVotingRoundCreationRemovesAdjacentPairs() {
	for (size_t number_of_items = kMinimumItemsForPruning; number_of_items < 24; number_of_items++) {
		auto voting_round = generateNewVotingRound(getNItems(number_of_items), false).value();
		pruneVotes(voting_round);
		ASSERT_FALSE(hasAnyAdjacentPair(voting_round));
	}
}
void pruningAmountDuringVotingRoundCreationDependsOnNumberOfItems() {
	for (size_t number_of_items = kMinimumItemsForPruning; number_of_items < 24; number_of_items++) {
		auto const voting_round = generateNewVotingRound(getNItems(number_of_items), true).value();
		auto const number_of_pruned_votes = number_of_items * pruningAmount(number_of_items);
		auto const number_of_votes_after_pruning = sumOfFirstIntegers(number_of_items - 1) - number_of_pruned_votes;
		ASSERT_EQ(numberOfScheduledVotes(voting_round), static_cast<uint32_t>(number_of_votes_after_pruning));
	}
}
void pruningAmountAfterVotingRoundCreationDependsOnNumberOfItems() {
	for (size_t number_of_items = kMinimumItemsForPruning; number_of_items < 24; number_of_items++) {
		auto voting_round = generateNewVotingRound(getNItems(number_of_items), false).value();
		pruneVotes(voting_round);
		auto const number_of_pruned_votes = number_of_items * pruningAmount(number_of_items);
		auto const number_of_votes_after_pruning = sumOfFirstIntegers(number_of_items - 1) - number_of_pruned_votes;
		ASSERT_EQ(numberOfScheduledVotes(voting_round), static_cast<uint32_t>(number_of_votes_after_pruning));
	}
}
void pruningWhenVotesAlreadyExist() {
	auto voting_round = generateNewVotingRound(getNItems(15), false);
	vote(voting_round, Option::A);
	pruneVotes(voting_round.value());
	ASSERT_FALSE(voting_round.value().reduced_voting);
	ASSERT_EQ(numberOfScheduledVotes(voting_round.value()), static_cast<uint32_t>(sumOfFirstIntegers(voting_round.value().items.size() - 1)));
}
void pruningWhenAlreadyPruned() {
	auto voting_round = generateNewVotingRound(getNItems(15), true);
	auto const number_of_votes_total_before = numberOfScheduledVotes(voting_round.value());
	pruneVotes(voting_round.value());
	ASSERT_TRUE(voting_round.value().reduced_voting);
	ASSERT_EQ(numberOfScheduledVotes(voting_round.value()), number_of_votes_total_before);
}
void parseVotingRoundWithPruning() {
	for (size_t number_of_items = kMinimumItemsForPruning; number_of_items < 24; number_of_items++) {
		auto const voting_round = parseVotingRoundFromText(getNItems(number_of_items) + std::vector<std::string>{ "", "1", "reduced" });
		ASSERT_TRUE(voting_round.value().reduced_voting);
		auto const number_of_pruned_votes = number_of_items * pruningAmount(number_of_items);
		auto const number_of_votes_after_pruning = sumOfFirstIntegers(number_of_items - 1) - number_of_pruned_votes;
		ASSERT_EQ(numberOfScheduledVotes(voting_round.value()), static_cast<uint32_t>(number_of_votes_after_pruning));
	}
}
void pruningRemovesCorrectScheduledVotes() {
	auto const generate_and_get_index_pairs = [](uint32_t number_of_items) -> IndexPairs {
		return generateNewVotingRound(getNItems(number_of_items), true).value().index_pairs;
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
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{1, 3},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{2, 4},
		IndexPair{2, 5},
		IndexPair{3, 5} });
	ASSERT_EQ(generate_and_get_index_pairs(7), IndexPairs{
		IndexPair{0, 2},
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{1, 3},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{2, 4},
		IndexPair{2, 5},
		IndexPair{2, 6},
		IndexPair{3, 5},
		IndexPair{3, 6},
		IndexPair{4, 6} });
	ASSERT_EQ(generate_and_get_index_pairs(8), IndexPairs{
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{2, 5},
		IndexPair{2, 6},
		IndexPair{2, 7},
		IndexPair{3, 6},
		IndexPair{3, 7},
		IndexPair{4, 7} });
	ASSERT_EQ(generate_and_get_index_pairs(9), IndexPairs{
		IndexPair{0, 3},
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{1, 4},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{1, 7},
		IndexPair{2, 5},
		IndexPair{2, 6},
		IndexPair{2, 7},
		IndexPair{2, 8},
		IndexPair{3, 6},
		IndexPair{3, 7},
		IndexPair{3, 8},
		IndexPair{4, 7},
		IndexPair{4, 8},
		IndexPair{5, 8} });
	ASSERT_EQ(generate_and_get_index_pairs(10), IndexPairs{
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{1, 7},
		IndexPair{2, 6},
		IndexPair{2, 7},
		IndexPair{2, 8},
		IndexPair{3, 7},
		IndexPair{3, 8},
		IndexPair{3, 9},
		IndexPair{4, 8},
		IndexPair{4, 9},
		IndexPair{5, 9} });
	ASSERT_EQ(generate_and_get_index_pairs(11), IndexPairs{
		IndexPair{0, 4},
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{0, 7},
		IndexPair{1, 5},
		IndexPair{1, 6},
		IndexPair{1, 7},
		IndexPair{1, 8},
		IndexPair{2, 6},
		IndexPair{2, 7},
		IndexPair{2, 8},
		IndexPair{2, 9},
		IndexPair{3, 7},
		IndexPair{3, 8},
		IndexPair{3, 9},
		IndexPair{3, 10},
		IndexPair{4, 8},
		IndexPair{4, 9},
		IndexPair{4, 10},
		IndexPair{5, 9},
		IndexPair{5, 10},
		IndexPair{6, 10} });
	ASSERT_EQ(generate_and_get_index_pairs(12), IndexPairs{
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{0, 7},
		IndexPair{1, 6},
		IndexPair{1, 7},
		IndexPair{1, 8},
		IndexPair{2, 7},
		IndexPair{2, 8},
		IndexPair{2, 9},
		IndexPair{3, 8},
		IndexPair{3, 9},
		IndexPair{3, 10},
		IndexPair{4, 9},
		IndexPair{4, 10},
		IndexPair{4, 11},
		IndexPair{5, 10},
		IndexPair{5, 11},
		IndexPair{6, 11} });
	ASSERT_EQ(generate_and_get_index_pairs(13), IndexPairs{
		IndexPair{0, 5},
		IndexPair{0, 6},
		IndexPair{0, 7},
		IndexPair{0, 8},
		IndexPair{1, 6},
		IndexPair{1, 7},
		IndexPair{1, 8},
		IndexPair{1, 9},
		IndexPair{2, 7},
		IndexPair{2, 8},
		IndexPair{2, 9},
		IndexPair{2, 10},
		IndexPair{3, 8},
		IndexPair{3, 9},
		IndexPair{3, 10},
		IndexPair{3, 11},
		IndexPair{4, 9},
		IndexPair{4, 10},
		IndexPair{4, 11},
		IndexPair{4, 12},
		IndexPair{5, 10},
		IndexPair{5, 11},
		IndexPair{5, 12},
		IndexPair{6, 11},
		IndexPair{6, 12},
		IndexPair{7, 12} });
}

} // namsepace

int main() {
	pruningDuringVotingRoundCreationWithTooFewItems();
	pruningAfterVotingRoundCreationWithTooFewItems();
	pruningDuringVotingRoundCreationRemovesAdjacentPairs();
	pruningAfterVotingRoundCreationRemovesAdjacentPairs();
	pruningAmountDuringVotingRoundCreationDependsOnNumberOfItems();
	pruningAmountAfterVotingRoundCreationDependsOnNumberOfItems();
	pruningWhenVotesAlreadyExist();
	pruningWhenAlreadyPruned();
	parseVotingRoundWithPruning();
	pruningRemovesCorrectScheduledVotes();
	return 0;
}