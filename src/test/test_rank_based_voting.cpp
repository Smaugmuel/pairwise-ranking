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
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto const voting_round = VotingRound::create(getNItems(2), VotingFormat::Ranked);
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_EQ(voting_round.value().format(), VotingFormat::Ranked);
	}
}
// TODO: Move to test_parse_voting_round
void parseRankedVotingRoundWithNoVotes() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto const voting_round = VotingRound::create(
			getNItems(number_of_items) + std::vector<std::string>{
			"",
			"12345",
			"ranked"
		});
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_EQ(voting_round.value().format(), VotingFormat::Ranked);
		ASSERT_EQ(voting_round.value().numberOfSortedItems(), 1ui32);
		ASSERT_TRUE(voting_round.value().votes().empty());
	}
}
// TODO: Move to test_parse_voting_round
void parseRankedVotingRoundWithSomeVotes() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto const voting_round = VotingRound::create(
			getNItems(number_of_items) + std::vector<std::string>{
			"",
			"12345",
			"ranked",
			"0 1 1"
		});
		ASSERT_TRUE(voting_round.has_value());
		ASSERT_EQ(voting_round.value().format(), VotingFormat::Ranked);
		ASSERT_EQ(voting_round.value().votes(), Votes{
			Vote{ 0, 1, Option::B },
		});
	}
}
// TODO: Move to test_parse_voting_round
void parseRankedVotingRoundWithTooManyVotes() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto const voting_round = VotingRound::create(
			getNItems(number_of_items) + std::vector<std::string>{
			"",
			"12345",
			"ranked",
			"0 1 1",
			"0 2 1",
			"1 2 1",
			"2 1 0"
		});
		ASSERT_FALSE(voting_round.has_value());
	}
}
// TODO: Move to test_undo
void undoWhenNoRankVote() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);
		voting_round.value().shuffle();

		for (uint32_t repetition = 0; repetition < 100; repetition++) {
			ASSERT_FALSE(voting_round.value().undoVote());
		}
		ASSERT_EQ(voting_round.value().numberOfSortedItems(), 1ui32);
	}
}
// TODO: Move to test_undo
void undoFirstRankVoteReducesNumberOfSortedItems() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);
		voting_round.value().shuffle();

		voting_round.value().vote(static_cast<Option>(number_of_items % 2));

		ASSERT_EQ(voting_round.value().numberOfSortedItems(), 2ui32);
		voting_round.value().undoVote();
		ASSERT_EQ(voting_round.value().numberOfSortedItems(), 1ui32);
	}
}
// TODO: Move to test_undo
void undoLastRankVoteReducesNumberOfSortedItems() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);
		voting_round.value().shuffle();

		uint32_t vote_counter{ 0 };
		while(voting_round.value().hasRemainingVotes()) {
			voting_round.value().vote(static_cast<Option>(vote_counter % 2));
			vote_counter++;
		}
		ASSERT_EQ(voting_round.value().numberOfSortedItems(), number_of_items);
		voting_round.value().undoVote();
		ASSERT_EQ(voting_round.value().numberOfSortedItems(), number_of_items - 1);
	}
}
// TODO: Move to test_undo
void undoAllRankVotesThenVoteTheSame() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);
		voting_round.value().shuffle();

		uint32_t vote_counter = 0;
		while (voting_round.value().hasRemainingVotes()) {
			voting_round.value().vote(static_cast<Option>(vote_counter % 2));
			vote_counter++;
		}

		auto const votes_before = voting_round.value().votes();

		while (!voting_round.value().votes().empty()) {
			voting_round.value().undoVote();
		}

		vote_counter = 0;
		while (voting_round.value().hasRemainingVotes()) {
			voting_round.value().vote(static_cast<Option>(vote_counter % 2));
			vote_counter++;
		}

		auto const votes_after = voting_round.value().votes();
		ASSERT_EQ(votes_before, votes_after);
	}
}
// TODO: Move to test_undo
void undoRankVoteDisplaysPreviousOptions() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);
		voting_round.value().shuffle();

		for (uint32_t vote_count = 0; vote_count < number_of_items - 2; vote_count++) {
			voting_round.value().vote(Option::A);
		}
		auto const matchup = voting_round.value().currentMatchup();

		voting_round.value().vote(Option::A);
		voting_round.value().undoVote();
		auto const matchup_after_voting_a_then_undoing = voting_round.value().currentMatchup();
		ASSERT_EQ(matchup.value().item_a, matchup_after_voting_a_then_undoing.value().item_a);
		ASSERT_EQ(matchup.value().item_b, matchup_after_voting_a_then_undoing.value().item_b);

		voting_round.value().vote(Option::B);
		voting_round.value().undoVote();
		auto const matchup_after_voting_b_then_undoing = voting_round.value().currentMatchup();
		ASSERT_EQ(matchup.value().item_a, matchup_after_voting_b_then_undoing.value().item_a);
		ASSERT_EQ(matchup.value().item_b, matchup_after_voting_b_then_undoing.value().item_b);
	}
}

void numberOfSortedItemsIncrementsWhenRightOptionChanges() {
	for (uint32_t number_of_items = 3; number_of_items < 25; number_of_items++) {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);
		voting_round.value().shuffle();

		auto right_option = voting_round.value().currentMatchup().value().item_b;
		auto number_of_sorted_items = uint32_t{ 1 };
		while (voting_round.value().hasRemainingVotes()) {
			if (voting_round.value().currentMatchup().value().item_b != right_option) {
				number_of_sorted_items++;
				right_option = voting_round.value().currentMatchup().value().item_b;
			}
			ASSERT_EQ(voting_round.value().numberOfSortedItems(), number_of_sorted_items);

			voting_round.value().vote(Option::A);
		}
		ASSERT_EQ(voting_round.value().numberOfSortedItems(), number_of_items);
	}
}
void presentedOptionsDependOnVoting() {
	for (uint32_t number_of_items = 3; number_of_items < 25; number_of_items++) {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);

		// Setup the last item as test subject. It will have the most comparisons during voting
		while (voting_round.value().numberOfSortedItems() < number_of_items - 1) {
			voting_round.value().vote(Option::A);
		}

		auto const matchup_after_voting_a = voting_round.value().currentMatchup();
		voting_round.value().vote(Option::A);
		auto const matchup_after_voting_aa = voting_round.value().currentMatchup();

		voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);
		voting_round.value().vote(Option::A);
		voting_round.value().vote(Option::B);
		auto const matchup_after_voting_ab = voting_round.value().currentMatchup();

		voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);
		voting_round.value().vote(Option::B);
		auto const matchup_after_voting_b = voting_round.value().currentMatchup();
		voting_round.value().vote(Option::A);
		auto const matchup_after_voting_ba = voting_round.value().currentMatchup();

		voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);
		voting_round.value().vote(Option::B);
		voting_round.value().vote(Option::B);
		auto const matchup_after_voting_bb = voting_round.value().currentMatchup();
	}
}
void itemsAreSortedCorrectlyForNumberOfSortedItems() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Ranked);

		while (voting_round.value().hasRemainingVotes()) {
			auto const number_of_sorted_items = voting_round.value().numberOfSortedItems();
			for (uint32_t sorted_item_index = 0; sorted_item_index < number_of_sorted_items; sorted_item_index++) {
				ASSERT_EQ(voting_round.value().items()[sorted_item_index], "item" + std::to_string(number_of_sorted_items - sorted_item_index));
			}
			voting_round.value().vote(Option::B);
		}
		auto const number_of_sorted_items = voting_round.value().numberOfSortedItems();
		for (uint32_t sorted_item_index = 0; sorted_item_index < number_of_sorted_items; sorted_item_index++) {
			ASSERT_EQ(voting_round.value().items()[sorted_item_index], "item" + std::to_string(number_of_sorted_items - sorted_item_index));
		}
	}
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

auto run_tests(char* argv[]) -> int {
	RUN_TEST_IF_ARGUMENT_EQUALS(newRoundFormatStringPresentsAvailableVotingFormatsAndNumberOfVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(createNewRankedVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(parseRankedVotingRoundWithNoVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(parseRankedVotingRoundWithSomeVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(parseRankedVotingRoundWithTooManyVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(undoWhenNoRankVote);
	RUN_TEST_IF_ARGUMENT_EQUALS(undoFirstRankVoteReducesNumberOfSortedItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(undoLastRankVoteReducesNumberOfSortedItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(undoAllRankVotesThenVoteTheSame);
	RUN_TEST_IF_ARGUMENT_EQUALS(undoRankVoteDisplaysPreviousOptions);
	RUN_TEST_IF_ARGUMENT_EQUALS(numberOfSortedItemsIncrementsWhenRightOptionChanges);
	RUN_TEST_IF_ARGUMENT_EQUALS(presentedOptionsDependOnVoting);
	RUN_TEST_IF_ARGUMENT_EQUALS(itemsAreSortedCorrectlyForNumberOfSortedItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(currentRankingListPresentsNumberOfSortedItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(convertingRankedVotingRoundToText);
	RUN_TEST_IF_ARGUMENT_EQUALS(rankItemsBySortingWhenInitiallyRanked);
	RUN_TEST_IF_ARGUMENT_EQUALS(rankItemsBySortingWhenInitiallyUnranked);
	RUN_TEST_IF_ARGUMENT_EQUALS(voteCounterAndApproximateTotalVotesIsPresented);
	return 1;
}

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	if (run_tests(argv) != 0) {
		return 1;
	}
	return 0;
}
