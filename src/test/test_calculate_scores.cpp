#include "score.h"
#include "score_helpers.h"
#include "testing.h"
#include "voting_round.h"

namespace
{
auto findScoreForItem(Scores const& scores, std::string const& item) -> Score {
	return *std::find_if(scores.begin(), scores.end(), [&item](Score const& score) {
		return score.item == item;
	});
}

void zeroVotes() {
	VotingRound const voting_round{};
	ASSERT_EQ(calculateScores(voting_round.items(), voting_round.votes()).size(), 0ui64);
}
void oneVoteForA() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(2), VotingFormat::Full)};
	voting_round.value().vote(Option::A);
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 1, 0 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 0, 1 });
}
void oneVoteForB() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(2), VotingFormat::Full) };
	voting_round.value().vote(Option::B);
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 0, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 1, 0 });
}
void allVotesForA() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	while (voting_round.value().hasRemainingVotes()) {
		voting_round.value().vote(Option::A);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());
	
	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 3, 0 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 0, 3 });
}
void allVotesForB() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	while (voting_round.value().hasRemainingVotes()) {
		voting_round.value().vote(Option::B);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 3, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 4, 0 });
}
void votingForItem2AndOptionAIfNotPresent() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	while (voting_round.value().hasRemainingVotes()) {
		auto const current_items = voting_round.value().currentMatchup();

		if (current_items.value().item_a == "item2") {
			voting_round.value().vote(Option::A);
			continue;
		}
		if (current_items.value().item_b == "item2") {
			voting_round.value().vote(Option::B);
			continue;
		}
		voting_round.value().vote(Option::A);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 0, 3 });
}
void votingForItem3AndOptionAIfNotPresent() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	while (voting_round.value().hasRemainingVotes()) {
		auto const current_items = voting_round.value().currentMatchup();

		if (current_items.value().item_a == "item3") {
			voting_round.value().vote(Option::A);
			continue;
		}
		if (current_items.value().item_b == "item3") {
			voting_round.value().vote(Option::B);
			continue;
		}
		voting_round.value().vote(Option::A);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 0, 3 });
}
void votingForItem2AndOptionBIfNotPresent() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	while (voting_round.value().hasRemainingVotes()) {
		auto const current_items = voting_round.value().currentMatchup();

		if (current_items.value().item_a == "item2") {
			voting_round.value().vote(Option::A);
			continue;
		}
		if (current_items.value().item_b == "item2") {
			voting_round.value().vote(Option::B);
			continue;
		}
		voting_round.value().vote(Option::B);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 2, 1 });
}
void votingForItem3AndOptionBIfNotPresent() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	while (voting_round.value().hasRemainingVotes()) {
		auto const current_items = voting_round.value().currentMatchup();

		if (current_items.value().item_a == "item3") {
			voting_round.value().vote(Option::A);
			continue;
		}
		if (current_items.value().item_b == "item3") {
			voting_round.value().vote(Option::B);
			continue;
		}
		voting_round.value().vote(Option::B);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 2, 1 });
}
void votingForItem2AndAgainstItem3AndOptionAIfNeither() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	while (voting_round.value().hasRemainingVotes()) {
		auto const current_items = voting_round.value().currentMatchup();
		
		if (current_items.value().item_a == "item2" || current_items.value().item_b == "item3") {
			voting_round.value().vote(Option::A);
			continue;
		}
		if (current_items.value().item_b == "item2" || current_items.value().item_a == "item3") {
			voting_round.value().vote(Option::B);
			continue;
		}
		voting_round.value().vote(Option::A);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 3, 0 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 0, 3 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 1, 2 });
}
void votingForItem2AndAgainstItem3AndOptionBIfNeither() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	while (voting_round.value().hasRemainingVotes()) {
		auto const current_items = voting_round.value().currentMatchup();

		if (current_items.value().item_a == "item2" || current_items.value().item_b == "item3") {
			voting_round.value().vote(Option::A);
			continue;
		}
		if (current_items.value().item_b == "item2" || current_items.value().item_a == "item3") {
			voting_round.value().vote(Option::B);
			continue;
		}
		voting_round.value().vote(Option::B);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 3, 0 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 0, 3 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 2, 1 });
}
void votingForItem3AndAgainstItem2AndOptionAIfNeither() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	while (voting_round.value().hasRemainingVotes()) {
		auto const current_items = voting_round.value().currentMatchup();

		if (current_items.value().item_a == "item2" || current_items.value().item_b == "item3") {
			voting_round.value().vote(Option::B);
			continue;
		}
		if (current_items.value().item_b == "item2" || current_items.value().item_a == "item3") {
			voting_round.value().vote(Option::A);
			continue;
		}
		voting_round.value().vote(Option::A);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 1, 2 });
}
void votingForItem3AndAgainstItem2AndOptionBIfNeither() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	while (voting_round.value().hasRemainingVotes()) {
		auto const current_items = voting_round.value().currentMatchup();

		if (current_items.value().item_a == "item2" || current_items.value().item_b == "item3") {
			voting_round.value().vote(Option::B);
			continue;
		}
		if (current_items.value().item_b == "item2" || current_items.value().item_a == "item3") {
			voting_round.value().vote(Option::A);
			continue;
		}
		voting_round.value().vote(Option::B);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 2, 1 });
}
void votingForOptionAButNotFullRound() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	for (auto i = 0; i < 4; ++i) {
		voting_round.value().vote(Option::A);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	ASSERT_TRUE(findScoreForItem(scores, "item1").wins >= 1);
	ASSERT_EQ(findScoreForItem(scores, "item1").losses, 0ui32);

	ASSERT_EQ(findScoreForItem(scores, "item4").wins, 0ui32);
	ASSERT_TRUE(findScoreForItem(scores, "item4").losses >= 0);

	ASSERT_TRUE(findScoreForItem(scores, "item2").wins + findScoreForItem(scores, "item2").losses >= 1);
	ASSERT_TRUE(findScoreForItem(scores, "item3").wins + findScoreForItem(scores, "item3").losses >= 1);
}
void votingForOptionBButNotFullRound() {
	std::optional<VotingRound> voting_round{ VotingRound::create(getNItems(4), VotingFormat::Full) };
	for (auto i = 0; i < 4; ++i) {
		voting_round.value().vote(Option::B);
	}
	auto const scores = calculateScores(voting_round.value().items(), voting_round.value().votes());
	ASSERT_EQ(scores.size(), voting_round.value().items().size());

	ASSERT_EQ(findScoreForItem(scores, "item1").wins, 0ui32);
	ASSERT_TRUE(findScoreForItem(scores, "item1").losses >= 1);

	ASSERT_TRUE(findScoreForItem(scores, "item4").wins >= 0);
	ASSERT_EQ(findScoreForItem(scores, "item4").losses, 0ui32);

	ASSERT_TRUE(findScoreForItem(scores, "item2").wins + findScoreForItem(scores, "item2").losses >= 1);
	ASSERT_TRUE(findScoreForItem(scores, "item3").wins + findScoreForItem(scores, "item3").losses >= 1);
}

} // namespace

int main() {
	zeroVotes();
	oneVoteForA();
	oneVoteForB();
	votingForItem2AndOptionAIfNotPresent();
	votingForItem3AndOptionAIfNotPresent();
	votingForItem2AndOptionBIfNotPresent();
	votingForItem3AndOptionBIfNotPresent();
	votingForItem2AndAgainstItem3AndOptionAIfNeither();
	votingForItem2AndAgainstItem3AndOptionBIfNeither();
	votingForItem3AndAgainstItem2AndOptionAIfNeither();
	votingForItem3AndAgainstItem2AndOptionBIfNeither();
	votingForOptionAButNotFullRound();
	votingForOptionBButNotFullRound();
	return 0;
}