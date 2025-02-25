#include "score.h"
#include "score_helpers.h"
#include "testing.h"
#include "voting_round.h"

namespace
{

/* -------------- Test helpers -------------- */
template<typename T>
auto operator+(std::vector<T> const& a, std::vector<T> const& b) -> decltype(auto) {
	std::vector<T> vec = a;
	vec.insert(vec.end(), b.begin(), b.end());
	return vec;
}

auto findScore(Scores const& scores, Item const& item) -> Score {
	return *std::find_if(scores.begin(), scores.end(), [&](Score const& score) { return score.item == item; });
}

/* -------------- Tests -------------- */

void itemOrderIsShuffledWhenShufflingNewVotingRound() {
	auto const items = getNItems(10);
	auto voting_round = VotingRound::create(items, false, 12345);
	voting_round.value().shuffle();
	Items const expected_item_order{
		"item7",
		"item6",
		"item3",
		"item9",
		"item4",
		"item2",
		"item5",
		"item10",
		"item8",
		"item1"
	};
	ASSERT_EQ(voting_round.value().items(), expected_item_order);
}
void originalItemOrderIsRetainedWhenShufflingNewVotingRound() {
	auto const items = getNItems(10);
	auto voting_round = VotingRound::create(items, false);
	voting_round.value().shuffle();
	ASSERT_EQ(voting_round.value().originalItemOrder(), items);
}
void itemOrderIsShuffledWhenShufflingParsedVotingRound() {
	auto const items = getNItems(10);
	auto const voting_round_text = items + std::vector<std::string>{
		"",
		"12345",
		"full"
	};
	auto const voting_round = VotingRound::create(voting_round_text);
	Items const expected_item_order{
		"item7",
		"item6",
		"item3",
		"item9",
		"item4",
		"item2",
		"item5",
		"item10",
		"item8",
		"item1"
	};
	ASSERT_EQ(voting_round.value().items(), expected_item_order);
}
void originalItemOrderIsRetainedWhenShufflingParsedVotingRound() {
	auto const items = getNItems(10);
	auto const voting_round_text = items + std::vector<std::string>{
		"",
		"12345",
		"full"
	};
	auto const voting_round = VotingRound::create(voting_round_text);
	ASSERT_EQ(voting_round.value().originalItemOrder(), items);
}
void convertingVotingRoundToTextUsesOriginalItemOrder() {
	auto const items = getNItems(10);
	auto voting_round = VotingRound::create(items, false, 12345);
	voting_round.value().shuffle();
	auto const text = voting_round.value().convertToText();
	for (size_t i = 0; i < items.size(); i++) {
		ASSERT_EQ(text[i], items[i]);
	}
}
void scoresAreCalculatedWithCorrectItems() {
	auto const items = getNItems(10);
	auto voting_round = VotingRound::create(items, false, 12345);
	voting_round.value().shuffle();

	// Vote A on each option
	for (uint32_t i = 0; i < voting_round.value().numberOfScheduledVotes(); i++) {
		voting_round.value().vote(Option::A);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());

	// Because of the order in which matchups are generated, voting for A each time
	// will mean the first item gets N-1 wins, where N is the number of items.
	// Similarily, the second item will get N-2 wins, and so on until the
	// Nth item will get 0 wins. The actual order in which the options are presented
	// thus doesn't matter, as the total number of wins will be the same regardless.

	uint32_t const max_wins = voting_round.value().items().size() - 1;
	for (uint32_t i = 0; i < voting_round.value().items().size(); i++) {
		ASSERT_EQ(findScore(scores, voting_round.value().items()[i]).wins, max_wins - i);
	}
}

} // namespace

int main() {
	itemOrderIsShuffledWhenShufflingNewVotingRound();
	originalItemOrderIsRetainedWhenShufflingNewVotingRound();
	itemOrderIsShuffledWhenShufflingParsedVotingRound();
	originalItemOrderIsRetainedWhenShufflingParsedVotingRound();
	convertingVotingRoundToTextUsesOriginalItemOrder();
	scoresAreCalculatedWithCorrectItems();

	return 0;
}