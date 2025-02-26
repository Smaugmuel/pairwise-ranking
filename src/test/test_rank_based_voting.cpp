#include "constants.h"
#include "helpers.h"
#include "menus.h"
#include "testing.h"
#include "voting_round.h"

namespace
{

// TODO: Move to another test file - probably make a new test
void newRoundFormatStringPresentsAvailableVotingFormatsAndNumberOfVotes() {
	auto const expected_string = [](uint32_t full_votes, uint32_t reduced_votes, uint32_t ranked_votes) -> std::string {
		return
			"Please choose voting format:\n"
			"[F]ull     score-based voting - " + std::to_string(full_votes) + " votes\n"
			"[R]educed  score-based voting - " + std::to_string(reduced_votes) + " votes\n"
			"[I]nsertion rank-based voting - Around " + std::to_string(ranked_votes) + " votes\n"
			"[C]ancel";
	};
	ASSERT_EQ(newRoundFormatString(2), expected_string(1, 1, 2));
	ASSERT_EQ(newRoundFormatString(5), expected_string(10, 10, 11));
	ASSERT_EQ(newRoundFormatString(6), expected_string(15, 9, 15));
	ASSERT_EQ(newRoundFormatString(10), expected_string(45, 15, 33));
}

// TODO: Move to generate_new_voting_round, and combine with other format testing
void createNewRankedVotingRound() {
	auto const voting_round = VotingRound::create(getNItems(2), VotingFormat::Ranked);
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_EQ(voting_round.value().format(), VotingFormat::Ranked);
}

// TODO: Move to test_parse_voting_round
void parseRankedVotingRoundWithNoVotes() {
	auto const voting_round = VotingRound::create(std::vector<std::string>{
		"item1",
		"item2",
		"",
		"12345",
		"ranked"
	});
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_EQ(voting_round.value().format(), VotingFormat::Ranked);
	ASSERT_EQ(voting_round.value().numberOfSortedItems(), 1ui32);
}

// TODO: Move to test_parse_voting_round
void parseRankedVotingRoundWithSomeVotes() {
	auto const voting_round = VotingRound::create(std::vector<std::string>{
		"item1",
		"item2",
		"item3",
		"",
		"12345",
		"ranked",
		"0 1 1",
		"1 2 0"
	});
	ASSERT_TRUE(voting_round.has_value());
	ASSERT_EQ(voting_round.value().format(), VotingFormat::Ranked);
	ASSERT_EQ(voting_round.value().votes(), Votes{
		Vote{ 0, 1, Option::B },
		Vote{ 1, 2, Option::A },
	});
}
void undoRankVote() {

}
void votingIncreasesVoteCounter() {

}
void votingIncreasesNumberOfSortedItemsOnlyWhenBothOptionsAreChanged() {

}
void votingChangesItemsOnlyWhenNumberOfSortedItemsIncreases() {

}
void presentedOptionsDependOnVoting() {

}
void currentRankingListPresentsSortedItemsCorrectly() {

}
void currentRankingListPresentsNumberOfSortedItems() {

}
void convertingRankedVotingRoundToText() {

}
void rankItemsBySortingWhenInitiallyRanked() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto const sorted_items = getNItems(number_of_items);
		auto voting_round{ VotingRound::create(sorted_items, VotingFormat::Ranked) };

		while (voting_round.value().hasRemainingVotes()) {
			auto const matchup = voting_round.value().currentMatchup();

			// Sort by number, since lexicographical comparison would sort like this: item1 < item10 < item2
			auto const item_a_number = std::stoi(matchup.value().item_a.substr(4));
			auto const item_b_number = std::stoi(matchup.value().item_b.substr(4));
			voting_round.value().vote(item_a_number < item_b_number ? Option::A : Option::B);
		}
		ASSERT_EQ(voting_round.value().items(), sorted_items);
	}
}
void rankItemsBySortingWhenInitiallyUnranked() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto const sorted_items = getNItems(number_of_items);
		auto voting_round{ VotingRound::create(sorted_items, VotingFormat::Ranked, 12345) };
		voting_round.value().shuffle();

		// Note that this inequality might not be true for all seeds
		ASSERT_NE(voting_round.value().items(), sorted_items);

		while (voting_round.value().hasRemainingVotes()) {
			auto const matchup = voting_round.value().currentMatchup();

			// Sort by number, since lexicographical comparison would sort like this: item1 < item10 < item2
			auto const item_a_number = std::stoi(matchup.value().item_a.substr(4));
			auto const item_b_number = std::stoi(matchup.value().item_b.substr(4));
			voting_round.value().vote(item_a_number < item_b_number ? Option::A : Option::B);
		}
		ASSERT_EQ(voting_round.value().items(), sorted_items);
	}
}

void voteCounterAndApproximateTotalVotesIsPresented() {
	auto const items = getNItems(10);
	auto voting_round{ VotingRound::create(items, VotingFormat::Ranked) };
	voting_round.value().shuffle();

	auto const get_counter_string_then_vote_A = [&]() -> std::string {
		auto const str = voting_round.value().currentVotingLine().value().substr(0, 9);
		voting_round.value().vote(Option::A);
		return str;
	};

	// NOTE: With the current implementation of ranked voting, 10 items will lead to 19 votes if always voting A
	// In contrast, if always voting B then there will be 25 votes instead.
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "(  1/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "(  2/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "(  3/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "(  4/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "(  5/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "(  6/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "(  7/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "(  8/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "(  9/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "( 10/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "( 11/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "( 12/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "( 13/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "( 14/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "( 15/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "( 16/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "( 17/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "( 18/~33)" });
	ASSERT_EQ(get_counter_string_then_vote_A(), std::string{ "( 19/~33)" });
}

} // namespace

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	RUN_TEST_IF_ARGUMENT_EQUALS(newRoundFormatStringPresentsAvailableVotingFormatsAndNumberOfVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(createNewRankedVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(parseRankedVotingRoundWithNoVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(parseRankedVotingRoundWithSomeVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(rankItemsBySortingWhenInitiallyRanked);
	RUN_TEST_IF_ARGUMENT_EQUALS(rankItemsBySortingWhenInitiallyUnranked);
	RUN_TEST_IF_ARGUMENT_EQUALS(voteCounterAndApproximateTotalVotesIsPresented);
	return 1;
}