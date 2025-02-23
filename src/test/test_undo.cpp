#include "functions.h"
#include "testing.h"

namespace
{

void undoWhenNoVotingRound() {
	std::optional<VotingRound> voting_round;
	ASSERT_EQ(undo(voting_round), std::string{ "No poll to undo from" });
}
void undoWhenVotesExist() {
	auto voting_round = VotingRound::create({ "item1", "item2", "item3" }, false);
	vote(voting_round, Option::A);
	vote(voting_round, Option::B);
	ASSERT_EQ(undo(voting_round), std::string{});
	ASSERT_EQ(voting_round.value().votes.size(), 1ui64);
	ASSERT_EQ(voting_round.value().votes[0].winner, Option::A);
}
void undoWhenNoVotesExist() {
	auto voting_round = VotingRound::create({ "item1", "item2", "item3" }, false);
	ASSERT_EQ(undo(voting_round), std::string{});
}

} // namespace

int main() {
	undoWhenNoVotingRound();
	undoWhenVotesExist();
	undoWhenNoVotesExist();
	return 0;
}