#include <filesystem>
#include <fstream>

#include "helpers.h"
#include "testing.h"
#include "voting_round.h"

namespace
{

constexpr auto kTestFileName = "placeholder_file_name.txt";

void saveWhenSomeVotesRemain() {
	std::filesystem::remove(kTestFileName);

	auto voting_round = VotingRound::create(getNItems(3), VotingFormat::Full);
	ASSERT_TRUE(voting_round.value().save(kTestFileName));
	ASSERT_TRUE(voting_round.value().isSaved());
	ASSERT_TRUE(std::filesystem::exists(kTestFileName));

	std::filesystem::remove(kTestFileName);
}
void saveWhenNoVotesRemain() {
	std::filesystem::remove(kTestFileName);

	auto voting_round = VotingRound::create(getNItems(3), VotingFormat::Full);
	voting_round.value().vote(Option::A);
	voting_round.value().vote(Option::A);
	voting_round.value().vote(Option::A);
	ASSERT_TRUE(voting_round.value().save(kTestFileName));
	ASSERT_TRUE(voting_round.value().isSaved());
	ASSERT_TRUE(std::filesystem::exists(kTestFileName));

	std::filesystem::remove(kTestFileName);
}

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(saveWhenSomeVotesRemain);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveWhenNoVotesRemain);
	return true;
}

} // namespace

auto test_save_votes(std::string const& test_case) -> int {
	std::filesystem::remove(kTestFileName);
	if (run_tests(test_case)) {
		return 1;
	}
	std::filesystem::remove(kTestFileName);
	return 0;
}
