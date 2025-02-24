#include "helpers.h"
#include "testing.h"
#include "voting_round.h"

namespace
{

void emptyVotingRound() {
	ASSERT_EQ(VotingRound{}.convertToText(), std::vector<std::string>{});
}
void votingRoundWithFullVoting() {
	auto const voting_round = VotingRound::create({ "item1", "item2" }, false);
	ASSERT_EQ(voting_round.value().convertToText(), std::vector<std::string>{
		"item1",
		"item2",
		"",
		std::to_string(voting_round.value().seed),
		"full"
	});
}
void votingRoundWithReducedVoting() {
	auto const voting_round = VotingRound::create({ "i1", "i2", "i3", "i4", "i5", "i6" }, true);
	ASSERT_EQ(voting_round.value().convertToText(), std::vector<std::string>{
		"i1",
		"i2",
		"i3",
		"i4",
		"i5",
		"i6",
		"",
		std::to_string(voting_round.value().seed),
		"reduced"
	});
}
void votingRoundWithOneVote() {
	auto voting_round = VotingRound::create({ "item1", "item2", "item3" }, false);
	voting_round.value().shuffle();
	voting_round.value().vote(Option::A);

	auto const vote_string =
		std::to_string(voting_round.value().votes[0].a_idx) + " " +
		std::to_string(voting_round.value().votes[0].b_idx) + " " +
		std::to_string(to_underlying(voting_round.value().votes[0].winner));
	ASSERT_EQ(voting_round.value().convertToText(), std::vector<std::string>{
		"item1",
		"item2",
		"item3",
		"",
		std::to_string(voting_round.value().seed),
		"full",
		vote_string
	});
}

} // namespace

int main() {
	emptyVotingRound();
	votingRoundWithFullVoting();
	votingRoundWithReducedVoting();
	votingRoundWithOneVote();
	return 0;
}