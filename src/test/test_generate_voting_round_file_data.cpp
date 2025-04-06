#include "helpers.h"
#include "testing.h"
#include "voting_round.h"

namespace
{

void emptyVotingRound() {
	ASSERT_EQ(VotingRound{}.convertToText(), std::vector<std::string>{});
}
void votingRoundWithFullVoting() {
	auto const voting_round = VotingRound::create(getNItems(2), VotingFormat::Full);
	ASSERT_EQ(voting_round.value().convertToText(), std::vector<std::string>{
		"item1",
		"item2",
		"",
		std::to_string(voting_round.value().seed()),
		"full"
	});
}
void votingRoundWithReducedVoting() {
	auto const voting_round = VotingRound::create(getNItems(6), VotingFormat::Reduced);
	ASSERT_EQ(voting_round.value().convertToText(), std::vector<std::string>{
		"item1",
		"item2",
		"item3",
		"item4",
		"item5",
		"item6",
		"",
		std::to_string(voting_round.value().seed()),
		"reduced"
	});
}
void votingRoundWithOneVote() {
	auto voting_round = VotingRound::create(getNItems(3), VotingFormat::Full);
	voting_round.value().shuffle();
	voting_round.value().vote(Option::A);

	auto const vote_string =
		std::to_string(voting_round.value().votes()[0].a_idx) + " " +
		std::to_string(voting_round.value().votes()[0].b_idx) + " " +
		std::to_string(to_underlying(voting_round.value().votes()[0].winner));
	ASSERT_EQ(voting_round.value().convertToText(), std::vector<std::string>{
		"item1",
		"item2",
		"item3",
		"",
		std::to_string(voting_round.value().seed()),
		"full",
		vote_string
	});
}

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(emptyVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingRoundWithFullVoting);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingRoundWithReducedVoting);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingRoundWithOneVote);
	return true;
}

} // namespace

auto test_generate_voting_round_file_data(std::string const& test_case) -> int {
	if (run_tests(test_case)) {
		return 1;
	}
	return 0;
}
