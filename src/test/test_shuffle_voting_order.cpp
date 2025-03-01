#include "testing.h"
#include "voting_round.h"

namespace
{

void shufflingWithSeedIsDeterministic() {
	auto voting_round = VotingRound::create(getNItems(8), VotingFormat::Full, 123456);
	voting_round.value().shuffle();

	while (voting_round.value().vote(Option::A));

	ASSERT_EQ(voting_round.value().votes(), Votes{
		Vote{0, 6, Option::A},
		Vote{0, 3, Option::A},
		Vote{6, 7, Option::A},
		Vote{2, 7, Option::A},
		Vote{3, 7, Option::A},
		Vote{0, 1, Option::A},
		Vote{0, 7, Option::A},
		Vote{3, 5, Option::A},
		Vote{3, 4, Option::A},
		Vote{1, 2, Option::A},
		Vote{2, 5, Option::A},
		Vote{4, 5, Option::A},
		Vote{0, 4, Option::A},
		Vote{0, 2, Option::A},
		Vote{4, 6, Option::A},
		Vote{5, 7, Option::A},
		Vote{0, 5, Option::A},
		Vote{2, 6, Option::A},
		Vote{5, 6, Option::A},
		Vote{4, 7, Option::A},
		Vote{2, 4, Option::A},
		Vote{2, 3, Option::A},
		Vote{1, 6, Option::A},
		Vote{1, 4, Option::A},
		Vote{3, 6, Option::A},
		Vote{1, 3, Option::A},
		Vote{1, 5, Option::A},
		Vote{1, 7, Option::A} });
}

} // namespace

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	RUN_TEST_IF_ARGUMENT_EQUALS(shufflingWithSeedIsDeterministic);
	return 0;
}