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

} // namespace

auto run_tests(char* argv[]) -> int {
	RUN_TEST_IF_ARGUMENT_EQUALS(saveWhenSomeVotesRemain);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveWhenNoVotesRemain);
	return 1;
}

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	std::filesystem::remove(kTestFileName);
	if (run_tests(argv) != 0) {
		return 1;
	}
	std::filesystem::remove(kTestFileName);
	return 0;
}
