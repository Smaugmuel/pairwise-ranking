#include "menus.h"
#include "testing.h"

namespace
{

void showIntro() {
	ASSERT_EQ(activeMenuString(std::nullopt, true),
			  std::string{ "Welcome! Press H if you need help: " });
}
void noVotingRoundCreated() {
	ASSERT_EQ(activeMenuString(std::nullopt, false),
			  std::string{ "Press H if you need help: " });
}
void votingRoundStarted() {
	auto const voting_round = VotingRound::create(getNItems(2), VotingFormat::Full);
	ASSERT_EQ(activeMenuString(voting_round, false),
			  std::string{ "(1/1) H: help. A: \'item1\'. B: \'item2\'. Your choice: " });
}
void votingRoundCompleted() {
	auto voting_round = VotingRound::create(getNItems(2), VotingFormat::Full);
	voting_round.value().vote(Option::A);
	ASSERT_EQ(activeMenuString(voting_round, false),
			  std::string{ "Voting round finished. [H]elp. [Q]uit. Your choice: " });
}

} // namespace

int main() {
	showIntro();
	noVotingRoundCreated();
	votingRoundStarted();
	votingRoundCompleted();
	return 0;
}