#include "testing.h"
#include "voting_round.h"

namespace
{

void firstVoteForA() {
	auto voting_round = VotingRound::create(getNItems(3), VotingFormat::Full);
	ASSERT_TRUE(voting_round.value().votes().empty());
	ASSERT_TRUE(voting_round.value().vote(Option::A));
	ASSERT_EQ(voting_round.value().votes().size(), 1ui64);
	ASSERT_EQ(voting_round.value().votes()[0], Vote{ 0, 1, Option::A });
}
void firstVoteForB() {
	auto voting_round = VotingRound::create(getNItems(3), VotingFormat::Full);
	ASSERT_TRUE(voting_round.value().votes().empty());
	ASSERT_TRUE(voting_round.value().vote(Option::B));
	ASSERT_EQ(voting_round.value().votes().size(), 1ui64);
	ASSERT_EQ(voting_round.value().votes()[0], Vote{ 0, 1, Option::B });
}
void votingForAForEachScheduledVote() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Full);
		while (voting_round.value().hasRemainingVotes()) {
			voting_round.value().vote(Option::A);
		}

		auto const& votes = voting_round.value().votes();
		ASSERT_EQ(static_cast<uint32_t>(votes.size()), voting_round.value().numberOfScheduledVotes());
		for (auto const& vote : votes) {
			ASSERT_EQ(vote.winner, Option::A);
		}
	}
}
void votingForBForEachScheduledVote() {
	for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		auto voting_round = VotingRound::create(getNItems(number_of_items), VotingFormat::Full);
		while (voting_round.value().hasRemainingVotes()) {
			voting_round.value().vote(Option::B);
		}

		auto const& votes = voting_round.value().votes();
		ASSERT_EQ(static_cast<uint32_t>(votes.size()), voting_round.value().numberOfScheduledVotes());
		for (auto const& vote : votes) {
			ASSERT_EQ(vote.winner, Option::B);
		}
	}
}
void votingAfterRoundCompleted() {
	auto voting_round = VotingRound::create(getNItems(3), VotingFormat::Full);

	voting_round.value().vote(Option::B);
	voting_round.value().vote(Option::B);
	voting_round.value().vote(Option::B);

	ASSERT_FALSE(voting_round.value().vote(Option::A));
	ASSERT_FALSE(voting_round.value().vote(Option::B));
	ASSERT_EQ(voting_round.value().votes().size(), 3ui64);
}

} // namespace

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(firstVoteForA);
	RUN_TEST_IF_ARGUMENT_EQUALS(firstVoteForB);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForAForEachScheduledVote);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForBForEachScheduledVote);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingAfterRoundCompleted);
	return true;
}

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	if (run_tests(argv[1])) {
		return 1;
	}
	return 0;
}
