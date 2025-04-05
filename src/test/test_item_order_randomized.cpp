#include "calculate_scores.h"
#include "score.h"
#include "testing.h"
#include "voting_round.h"

namespace
{

/* -------------- Test helpers -------------- */
auto findScore(Scores const& scores, Item const& item) -> Score {
	return *std::find_if(scores.begin(), scores.end(), [&](Score const& score) { return score.item == item; });
}

/* -------------- Tests -------------- */
void itemOrderIsShuffledWhenShufflingNewVotingRound() {
	auto const items = getNItems(10);
	auto voting_round = VotingRound::create(items, VotingFormat::Full, 12345);
	voting_round.value().shuffle();

	Items const expected_item_order{
		"item6",
		"item10",
		"item9",
		"item5",
		"item1",
		"item3",
		"item8",
		"item2",
		"item7",
		"item4"
	};

	ASSERT_EQ(voting_round.value().items(), expected_item_order);
}
void originalItemOrderIsRetainedWhenShufflingNewVotingRound() {
	auto const items = getNItems(10);
	auto voting_round = VotingRound::create(items, VotingFormat::Full);
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
		"item6",
		"item10",
		"item9",
		"item5",
		"item1",
		"item3",
		"item8",
		"item2",
		"item7",
		"item4"
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
	auto voting_round = VotingRound::create(items, VotingFormat::Full, 12345);
	voting_round.value().shuffle();
	auto const text = voting_round.value().convertToText();
	for (size_t i = 0; i < items.size(); i++) {
		ASSERT_EQ(text[i], items[i]);
	}
}
void scoresAreCalculatedWithCorrectItems() {
	auto const items = getNItems(10);
	auto voting_round = VotingRound::create(items, VotingFormat::Full, 12345);
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

auto run_tests(char* argv[]) -> int {
	RUN_TEST_IF_ARGUMENT_EQUALS(itemOrderIsShuffledWhenShufflingNewVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(originalItemOrderIsRetainedWhenShufflingNewVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(itemOrderIsShuffledWhenShufflingParsedVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(originalItemOrderIsRetainedWhenShufflingParsedVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(convertingVotingRoundToTextUsesOriginalItemOrder);
	RUN_TEST_IF_ARGUMENT_EQUALS(scoresAreCalculatedWithCorrectItems);
	return 1;
}

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	if (run_tests(argv) != 0) {
		return 1;
	}
	return 0;
}
