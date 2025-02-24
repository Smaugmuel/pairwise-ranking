#include <filesystem>
#include <fstream>

#include "helpers.h"
#include "testing.h"
#include "voting_round.h"

namespace
{

constexpr auto kTestFileName = "placeholder_file_name.txt";

void saveWhenSaveFileFails() {
	std::filesystem::remove(kTestFileName);

	auto voting_round = VotingRound::create(getNItems(2), false);
	voting_round.value().items.clear();
	ASSERT_FALSE(voting_round.value().save(kTestFileName));
	ASSERT_FALSE(voting_round.value().is_saved);
	ASSERT_FALSE(std::filesystem::exists(kTestFileName));
}
void saveWhenSomeVotesRemain() {
	std::filesystem::remove(kTestFileName);

	auto voting_round = VotingRound::create(getNItems(3), false);
	ASSERT_TRUE(voting_round.value().save(kTestFileName));
	ASSERT_TRUE(voting_round.value().is_saved);
	ASSERT_TRUE(std::filesystem::exists(kTestFileName));

	std::filesystem::remove(kTestFileName);
}
void saveWhenNoVotesRemain() {
	std::filesystem::remove(kTestFileName);

	auto voting_round = VotingRound::create(getNItems(3), false);
	voting_round.value().vote(Option::A);
	voting_round.value().vote(Option::A);
	voting_round.value().vote(Option::A);
	ASSERT_TRUE(voting_round.value().save(kTestFileName));
	ASSERT_TRUE(voting_round.value().is_saved);
	ASSERT_TRUE(std::filesystem::exists(kTestFileName));

	std::filesystem::remove(kTestFileName);
}

} // namespace

int main() {
	std::filesystem::remove(kTestFileName);

	saveWhenSaveFileFails();
	saveWhenSomeVotesRemain();
	saveWhenNoVotesRemain();

	std::filesystem::remove(kTestFileName);
	return 0;
}
