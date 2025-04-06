#include "testing.h"
#include "voting_format.h"

namespace
{

void charToFormatWithValidOptions() {
	ASSERT_EQ(characterToVotingFormat('f'), VotingFormat::Full);
	ASSERT_EQ(characterToVotingFormat('r'), VotingFormat::Reduced);
	ASSERT_EQ(characterToVotingFormat('i'), VotingFormat::Ranked);
}
void charToFormatWithInvalidOptions() {
	for (char letter = 'a'; letter <= 'z'; letter++) {
		if (letter == 'f' || letter == 'r' || letter == 'i') {
			continue;
		}
		ASSERT_EQ(characterToVotingFormat(letter), VotingFormat::Invalid);
	}
}
void stringToFormatWithValidOptions() {
	ASSERT_EQ(stringToVotingFormat("full"), VotingFormat::Full);
	ASSERT_EQ(stringToVotingFormat("reduced"), VotingFormat::Reduced);
	ASSERT_EQ(stringToVotingFormat("ranked"), VotingFormat::Ranked);
}
void stringToFormatWithInalidOptions() {
	ASSERT_EQ(stringToVotingFormat("ful"), VotingFormat::Invalid);
	ASSERT_EQ(stringToVotingFormat("reduuced"), VotingFormat::Invalid);
	ASSERT_EQ(stringToVotingFormat("rank3d"), VotingFormat::Invalid);
}
void formatToString() {
	ASSERT_EQ(votingFormatToString(VotingFormat::Full), std::string{ "full" });
	ASSERT_EQ(votingFormatToString(VotingFormat::Reduced), std::string{ "reduced" });
	ASSERT_EQ(votingFormatToString(VotingFormat::Ranked), std::string{ "ranked" });
	ASSERT_EQ(votingFormatToString(VotingFormat::Invalid), std::string{ "" });
}

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(charToFormatWithValidOptions);
	RUN_TEST_IF_ARGUMENT_EQUALS(charToFormatWithInvalidOptions);
	RUN_TEST_IF_ARGUMENT_EQUALS(stringToFormatWithValidOptions);
	RUN_TEST_IF_ARGUMENT_EQUALS(stringToFormatWithInalidOptions);
	RUN_TEST_IF_ARGUMENT_EQUALS(formatToString);
	return true;
}

} // namespace

auto test_voting_format(std::string const& test_case) -> int {
	if (run_tests(test_case)) {
		return 1;
	}
	return 0;
}
