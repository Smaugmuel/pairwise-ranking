#include <algorithm>

#include "calculate_scores.h"
#include "helpers.h"
#include "score_helpers.h"
#include "testing.h"
#include "voting_round.h"

namespace
{

/* -------------- Test helpers -------------- */
auto findScore(Scores const& scores, Item const& item) -> Score {
	return *std::find_if(scores.begin(), scores.end(), [&](Score const& score) { return score.item == item; });
}

/* -------------- Tests -------------- */
void votingRoundIsTheSameAfterLoading() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto const original_items = getNItems(number_of_items);
		auto voting_round = VotingRound::create(original_items, VotingFormat::Full);
		voting_round.value().shuffle();

		// Vote for itemX or Option::A if not present, for half the votes
		auto const item_to_vote_for = "item" + std::to_string(original_items.size() / 2);
		for (uint32_t vote_index = 0; vote_index < voting_round.value().numberOfScheduledVotes() / 2; vote_index++) {
			auto const matchup = voting_round.value().currentMatchup().value();
			if (matchup.item_a == item_to_vote_for) {
				voting_round.value().vote(Option::A);
				continue;
			}
			if (matchup.item_b == item_to_vote_for) {
				voting_round.value().vote(Option::B);
				continue;
			}
			voting_round.value().vote(Option::A);
		}

		auto const file_name = std::string{ "test_votes.txt" };
		ASSERT_TRUE(voting_round.value().save(file_name));

		auto const items = voting_round.value().items();
		auto const voting_format = voting_round.value().format();
		auto const seed = voting_round.value().seed();
		auto const votes = voting_round.value().votes();
		auto const matchup = voting_round.value().currentMatchup();
		auto const voting_line = voting_round.value().currentVotingLine();

		voting_round.reset();

		auto const lines = loadFile(file_name);
		voting_round = VotingRound::create(lines);
		ASSERT_TRUE(voting_round.has_value());

		ASSERT_EQ(voting_round.value().items(), items);
		ASSERT_EQ(voting_round.value().format(), voting_format);
		ASSERT_EQ(voting_round.value().seed(), seed);
		ASSERT_EQ(voting_round.value().votes(), votes);
		ASSERT_EQ(voting_round.value().currentMatchup().value().item_a, matchup.value().item_a);
		ASSERT_EQ(voting_round.value().currentMatchup().value().item_b, matchup.value().item_b);
		ASSERT_EQ(voting_round.value().currentVotingLine().value(), voting_line.value());
	}
}
void scoresAreTheSameDespiteDifferentItemOrderAndSeed() {
	auto const original_items = getNItems(10);
	auto voting_round = VotingRound::create(original_items, VotingFormat::Full);
	voting_round.value().shuffle();
	
	auto const item_to_vote_for = "item" + std::to_string(original_items.size() / 2);

	// Vote for itemX or Option::A if not present, for each vote
	for (uint32_t vote_index = 0; vote_index < voting_round.value().numberOfScheduledVotes(); vote_index++) {
		auto const matchup = voting_round.value().currentMatchup().value();
		if (matchup.item_a == item_to_vote_for) {
			voting_round.value().vote(Option::A);
			continue;
		}
		if (matchup.item_b == item_to_vote_for) {
			voting_round.value().vote(Option::B);
			continue;
		}
		voting_round.value().vote(Option::A);
	}

	auto const seed_1 = voting_round.value().seed();
	auto items_1 = voting_round.value().items();
	auto scores_1 = calculateScores(items_1, voting_round.value().votes());

	// Reset voting round
	voting_round = VotingRound::create(original_items, VotingFormat::Full);
	voting_round.value().shuffle();

	// Vote for itemX or Option::A if not present, for each vote
	for (uint32_t vote_index = 0; vote_index < voting_round.value().numberOfScheduledVotes(); vote_index++) {
		auto const matchup = voting_round.value().currentMatchup().value();
		if (matchup.item_a == item_to_vote_for) {
			voting_round.value().vote(Option::A);
			continue;
		}
		if (matchup.item_b == item_to_vote_for) {
			voting_round.value().vote(Option::B);
			continue;
		}
		voting_round.value().vote(Option::A);
	}

	auto const seed_2 = voting_round.value().seed();
	auto items_2 = voting_round.value().items();
	auto scores_2 = calculateScores(items_2, voting_round.value().votes());

	// Items are the same, but in a different order
	ASSERT_NE(items_1, items_2);
	std::sort(items_1.begin(), items_1.end(), [](Item const& a, Item const& b) {
		return std::stoul(a.substr(4)) < std::stoul(b.substr(4));
	});
	std::sort(items_2.begin(), items_2.end(), [](Item const& a, Item const& b) {
		return std::stoul(a.substr(4)) < std::stoul(b.substr(4));
	});
	ASSERT_EQ(items_1, items_2);

	// Seed is different
	ASSERT_NE(seed_1, seed_2);

	// Overall scores will be different, but the item voted for each time will have the same score
	ASSERT_EQ(scores_1.size(), scores_2.size());
	ASSERT_EQ(findScore(scores_1, item_to_vote_for), findScore(scores_2, item_to_vote_for));
}

} // namespace

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingRoundIsTheSameAfterLoading);
	RUN_TEST_IF_ARGUMENT_EQUALS(scoresAreTheSameDespiteDifferentItemOrderAndSeed);
	return 0;
}
