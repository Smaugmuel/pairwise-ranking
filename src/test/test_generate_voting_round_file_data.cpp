#include "functions.h"
#include "testing.h"

namespace
{

void emptyVotingRound() {
	ASSERT_EQ(convertVotingRoundToText(VotingRound{}), std::vector<std::string>{});
}
void votingRoundWithFullVoting() {
	auto const voting_round = generateNewVotingRound({ "item1", "item2" }, false);
	ASSERT_EQ(convertVotingRoundToText(voting_round.value()), std::vector<std::string>{
		"item1",
		"item2",
		"",
		std::to_string(voting_round.value().seed),
		"full"
	});
}
void votingRoundWithReducedVoting() {
	auto const voting_round = generateNewVotingRound({ "i1", "i2", "i3", "i4", "i5", "i6" }, true);
	ASSERT_EQ(convertVotingRoundToText(voting_round.value()), std::vector<std::string>{
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
	auto voting_round = generateNewVotingRound({ "item1", "item2", "item3" }, false);
	shuffleVotingOrder(voting_round.value());
	vote(voting_round, Option::A);

	auto const vote_string =
		std::to_string(voting_round.value().votes[0].index_pair.first) + " " +
		std::to_string(voting_round.value().votes[0].index_pair.second) + " " +
		std::to_string(to_underlying(voting_round.value().votes[0].winner));
	ASSERT_EQ(convertVotingRoundToText(voting_round.value()), std::vector<std::string>{
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