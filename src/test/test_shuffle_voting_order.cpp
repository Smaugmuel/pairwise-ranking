#include "testing.h"
#include "voting_round.h"

int main() {
	auto voting_round = VotingRound::create(getNItems(8), false);
	voting_round.value().seed = 123456;
	voting_round.value().shuffle();

	ASSERT_EQ(voting_round.value().index_pairs, IndexPairs{
		IndexPair{0, 6},
		IndexPair{0, 3},
		IndexPair{6, 7},
		IndexPair{2, 7},
		IndexPair{3, 7},
		IndexPair{0, 1},
		IndexPair{0, 7},
		IndexPair{3, 5},
		IndexPair{3, 4},
		IndexPair{1, 2},
		IndexPair{2, 5},
		IndexPair{4, 5},
		IndexPair{0, 4},
		IndexPair{0, 2},
		IndexPair{4, 6},
		IndexPair{5, 7},
		IndexPair{0, 5},
		IndexPair{2, 6},
		IndexPair{5, 6},
		IndexPair{4, 7},
		IndexPair{2, 4},
		IndexPair{2, 3},
		IndexPair{1, 6},
		IndexPair{1, 4},
		IndexPair{3, 6},
		IndexPair{1, 3},
		IndexPair{1, 5},
		IndexPair{1, 7} });

	return 0;
}