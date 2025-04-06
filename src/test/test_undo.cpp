#include "testing.h"
#include "voting_round.h"

namespace
{

void undoWhenVotesExist() {
	auto voting_round = VotingRound::create(getNItems(3), VotingFormat::Full);
	voting_round.value().vote(Option::A);
	voting_round.value().vote(Option::B);
	ASSERT_TRUE(voting_round.value().undoVote());
	ASSERT_EQ(voting_round.value().votes().size(), 1ui64);
	ASSERT_EQ(voting_round.value().votes()[0].winner, Option::A);
}
void undoWhenNoVotesExist() {
	auto voting_round = VotingRound::create(getNItems(3), VotingFormat::Full);
	ASSERT_FALSE(voting_round.value().undoVote());
}

} // namespace

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(undoWhenVotesExist);
	RUN_TEST_IF_ARGUMENT_EQUALS(undoWhenNoVotesExist);
	return true;
}

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	if (run_tests(argv[1])) {
		return 1;
	}
	return 0;
}
