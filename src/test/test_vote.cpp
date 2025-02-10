#include "functions.h"
#include "testing.h"

namespace
{

void votingRoundNotCreated() {
	std::optional<VotingRound> voting_round;
	ASSERT_EQ(vote(voting_round, Option::A), std::string{ "No poll to vote in" });
	ASSERT_FALSE(voting_round.has_value());
	ASSERT_EQ(vote(voting_round, Option::B), std::string{ "No poll to vote in" });
	ASSERT_FALSE(voting_round.has_value());
}
void firstAndFinalVotes() {
	auto voting_round = generateNewVotingRound({ "item1", "item2" ,"item3" }, false);
	ASSERT_TRUE(voting_round.value().votes.empty());
	
	vote(voting_round, Option::A);
	ASSERT_EQ(voting_round.value().votes.size(), 1ui64);
	ASSERT_EQ(voting_round.value().votes[0], Vote{ { 0, 1 }, Option::A });
	
	vote(voting_round, Option::B);
	ASSERT_EQ(voting_round.value().votes.size(), 2ui64);
	ASSERT_EQ(voting_round.value().votes[1], Vote{ { 0, 2 }, Option::B });

	vote(voting_round, Option::A);
	ASSERT_EQ(voting_round.value().votes.size(), 3ui64);
	ASSERT_EQ(voting_round.value().votes[2], Vote{ { 1, 2 }, Option::A });
}
void votingAfterCompleted() {
	auto voting_round = generateNewVotingRound({ "item1", "item2" ,"item3" }, false);

	vote(voting_round, Option::B);
	vote(voting_round, Option::B);
	vote(voting_round, Option::B);

	ASSERT_EQ(vote(voting_round, Option::A), std::string{ "No ongoing poll with pending votes" });
	ASSERT_EQ(vote(voting_round, Option::B), std::string{ "No ongoing poll with pending votes" });
	ASSERT_EQ(voting_round.value().votes.size(), 3ui64);
}

}

int main() {
	votingRoundNotCreated();
	firstAndFinalVotes();
	votingAfterCompleted();
	return 0;
}