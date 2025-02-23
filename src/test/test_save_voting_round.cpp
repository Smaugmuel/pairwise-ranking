#include <filesystem>
#include <fstream>

#include "functions.h"
#include "helpers.h"
#include "testing.h"
#include "voting_round.h"

// TODO: This still uses save(), which will save the results too if the voting is completed.
// That should be tested in a separate file, and the logic should probably even be changed
// to always save the results regardless.
// When removing the use of save() and just using VotingRound::save(), the functions.h dependency can be removed.

namespace
{
constexpr auto kTestFileName = "placeholder_file_name.txt";
constexpr auto kTestFileName2 = "placeholder_file_name_2.txt";

void saveWhenNoVotingRound() {
	std::filesystem::remove(kTestFileName);
	std::filesystem::remove(kTestFileName2);

	std::optional<VotingRound> voting_round;
	auto const results = save(voting_round, kTestFileName, kTestFileName2);
	ASSERT_EQ(results.first, false);
	ASSERT_EQ(results.second, std::string{ "No poll to save" });
	ASSERT_FALSE(std::filesystem::exists(kTestFileName));
	ASSERT_FALSE(std::filesystem::exists(kTestFileName2));
}
void saveWhenSaveFileFails() {
	std::filesystem::remove(kTestFileName);
	std::filesystem::remove(kTestFileName2);

	auto voting_round = VotingRound::create({ "item1", "item2" }, false);
	voting_round.value().items.clear();
	auto const results = save(voting_round, kTestFileName, kTestFileName2);
	ASSERT_EQ(results.first, false);
	ASSERT_EQ(results.second, std::string{ "Could not save votes to " } + kTestFileName + ".");
	ASSERT_FALSE(voting_round.value().is_saved);
	ASSERT_FALSE(std::filesystem::exists(kTestFileName));
	ASSERT_FALSE(std::filesystem::exists(kTestFileName2));
}
void saveWhenSomeVotesRemain() {
	std::filesystem::remove(kTestFileName);
	std::filesystem::remove(kTestFileName2);

	auto voting_round = VotingRound::create({ "item1", "item2", "item3" }, false);
	auto const results = save(voting_round, kTestFileName, kTestFileName2);
	ASSERT_EQ(results.first, true);
	ASSERT_EQ(results.second, std::string{ "Votes saved to " } + kTestFileName +  ".");
	ASSERT_TRUE(voting_round.value().is_saved);
	ASSERT_TRUE(std::filesystem::exists(kTestFileName));
	ASSERT_FALSE(std::filesystem::exists(kTestFileName2));

	std::filesystem::remove(kTestFileName);
}
void saveWhenNoVotesRemain() {
	std::filesystem::remove(kTestFileName);
	std::filesystem::remove(kTestFileName2);

	auto voting_round = VotingRound::create({ "item1", "item2", "item3" }, false);
	voting_round.value().vote(Option::A);
	voting_round.value().vote(Option::A);
	voting_round.value().vote(Option::A);
	auto const results = save(voting_round, kTestFileName, kTestFileName2);
	ASSERT_EQ(results.first, true);
	ASSERT_EQ(results.second, std::string{ "Votes saved to " } + kTestFileName + ". Results saved to " + kTestFileName2 + ".");
	ASSERT_TRUE(voting_round.value().is_saved);
	ASSERT_TRUE(std::filesystem::exists(kTestFileName));
	ASSERT_TRUE(std::filesystem::exists(kTestFileName2));

	std::filesystem::remove(kTestFileName);
	std::filesystem::remove(kTestFileName2);
}

} // namespace

int main() {
	std::filesystem::remove(kTestFileName);
	std::filesystem::remove(kTestFileName2);

	saveWhenNoVotingRound();
	saveWhenSaveFileFails();
	saveWhenSomeVotesRemain();
	saveWhenNoVotesRemain();

	std::filesystem::remove(kTestFileName);
	std::filesystem::remove(kTestFileName2);
	return 0;
}