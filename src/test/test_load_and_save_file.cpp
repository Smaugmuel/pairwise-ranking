#include <filesystem>
#include <fstream>

#include "functions.h"
#include "helpers.h"
#include "testing.h"

namespace
{
constexpr auto kTestFileName = "placeholder_file_name.txt";
constexpr auto kTestFileName2 = "placeholder_file_name_2.txt";

/* -------------- Testing line saving -------------- */
void savingAndLoadingLines() {
	std::vector<std::string> const original_lines{
		"List",
		"of",
		"lines"
	};
	ASSERT_TRUE(saveFile(kTestFileName, original_lines));
	auto const loaded_lines = loadFile(kTestFileName);
	ASSERT_EQ(loaded_lines, original_lines);
	std::filesystem::remove(kTestFileName);
}
void savingAndLoadingLinesWithEmptyLines() {
	std::vector<std::string> const original_lines{
		"",
		"List",
		"of",
		"lines",
		"",
		"with",
		"some",
		"",
		"",
		"empty",
		"lines",
		""
	};
	saveFile(kTestFileName, original_lines);
	auto const loaded_lines = loadFile(kTestFileName);
	ASSERT_EQ(loaded_lines, original_lines);
	std::filesystem::remove(kTestFileName);
}
void savingAndLoadingOnlyEmptyLines() {
	std::vector<std::string> const original_lines{
		"",
		"",
		"",
		"",
		""
	};
	saveFile(kTestFileName, original_lines);
	auto const loaded_lines = loadFile(kTestFileName);
	ASSERT_EQ(loaded_lines, original_lines);
	std::filesystem::remove(kTestFileName);
}
void savingNoLines() {
	std::vector<std::string> const lines{};
	ASSERT_FALSE(saveFile(kTestFileName, lines));
	ASSERT_FALSE(std::filesystem::exists(kTestFileName));
}
void loadingNoLines() {
	std::ofstream file(kTestFileName);
	file.close();
	ASSERT_TRUE(loadFile(kTestFileName).empty());
	std::filesystem::remove(kTestFileName);
}
void savingToExistingFile() {
	std::vector<std::string> const original_lines{ "Content" };
	saveFile(kTestFileName, original_lines);
	ASSERT_TRUE(saveFile(kTestFileName, { "More", "Content" }));
	auto const loaded_lines = loadFile(kTestFileName);
	ASSERT_EQ(loaded_lines, std::vector<std::string>{ "More", "Content" });
	std::filesystem::remove(kTestFileName);
}
void loadingNonExistingFile() {
	ASSERT_FALSE(std::filesystem::exists(kTestFileName));
	ASSERT_TRUE(loadFile(kTestFileName).empty());
}

/* -------------- Testing voting round saving -------------- */
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
	vote(voting_round, Option::A);
	vote(voting_round, Option::A);
	vote(voting_round, Option::A);
	auto const results = save(voting_round, kTestFileName, kTestFileName2);
	ASSERT_EQ(results.first, true);
	ASSERT_EQ(results.second, std::string{ "Votes saved to " } + kTestFileName + ". Results saved to " + kTestFileName2 + ".");
	ASSERT_TRUE(voting_round.value().is_saved);
	ASSERT_TRUE(std::filesystem::exists(kTestFileName));
	ASSERT_TRUE(std::filesystem::exists(kTestFileName2));

	std::filesystem::remove(kTestFileName);
	std::filesystem::remove(kTestFileName2);
}

}

int main() {
	std::filesystem::remove(kTestFileName);

	savingAndLoadingLines();
	savingAndLoadingLinesWithEmptyLines();
	savingAndLoadingOnlyEmptyLines();
	savingNoLines();
	loadingNoLines();
	savingToExistingFile();
	loadingNonExistingFile();
	saveWhenNoVotingRound();
	saveWhenSaveFileFails();
	saveWhenSomeVotesRemain();
	saveWhenNoVotesRemain();

	std::filesystem::remove(kTestFileName);
	return 0;
}