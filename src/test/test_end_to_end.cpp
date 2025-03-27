#include "testing.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <queue>

#include "mocks/output_catcher.h"
#include "program_loop.h"

extern std::queue<char> g_keys;
extern std::queue<std::string> g_lines;

namespace
{

constexpr char const* kTestItemsFile = "test_items.txt";
constexpr char const* kTestVotesFile = "test_votes.txt";
constexpr char const* kTestScoresFile = "test_scores.txt";
constexpr char const* kTestRankingFile = "test_ranking.txt";
constexpr char const* kTestCombinedScoresFile = "test_combined_scores.txt";

enum class KeyAction {
	Quit = 'q',
	NewRound = 'n',
	LoadRound = 'l',
	FullRound = 'f',
	RankedRound = 'i',
	Save = 's',
	Yes = 'y',
	No = 'n',
	Cancel = 'c',
	VoteA = 'a',
	VoteB = 'b',
	Undo = 'u',
	Print = 'p',
	Combine = 'c',
	Help = 'h'
};
void appendAction(KeyAction action) {
	g_keys.push(static_cast<char>(action));
}
void appendLine(std::string const& line) {
	g_lines.push(line);
}
auto allActionsCompleted() -> bool {
	return g_keys.empty() && g_lines.empty();
}
void createItemsFile(uint32_t number_of_items) {
	std::ofstream items_file(kTestItemsFile);
	ASSERT_TRUE(items_file.is_open());
	auto const items = getNItems(number_of_items);
	for (auto const& item : items) {
		items_file << item << "\n";
	}
	items_file.close();
}
void createVotesFile(uint32_t number_of_items) {
	std::ofstream votes_file(kTestVotesFile);
	ASSERT_TRUE(votes_file.is_open());
	auto const items = getNItems(number_of_items);
	for (auto const& item : items) {
		votes_file << item + "\n";
	}
	votes_file <<
		"\n"
		"12345\n"
		"full\n";
	votes_file.close();
}
void cleanUpFiles() {
	std::filesystem::remove(kTestItemsFile);
	std::filesystem::remove(kTestVotesFile);
	std::filesystem::remove(kTestScoresFile);
	std::filesystem::remove(kTestRankingFile);
	std::filesystem::remove(kTestCombinedScoresFile);
}

void mainMenuLegendPrintedOnce() {
	// Repeat an invalid key
	for (uint32_t i = 0; i < 10; i++) {
		appendAction(KeyAction::VoteA);
	}
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Pairwise ranking"));
	ASSERT_FALSE(catcher.contains("Pairwise ranking", 2));
	ASSERT_TRUE(allActionsCompleted());
}
void newVotingRoundSelectItemsPromptPrintedEachTime() {
	appendAction(KeyAction::NewRound);
	for (uint32_t i = 0; i < 10; i++) {
		appendLine("");
	}
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select file to load items from", 11));
	ASSERT_TRUE(allActionsCompleted());
}
void newVotingRoundSelectItemsCancel() {
	appendAction(KeyAction::NewRound);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	programLoop();

	ASSERT_TRUE(allActionsCompleted());
}
void newVotingRoundSelectItemsNonExistingFile() {
	appendAction(KeyAction::NewRound);
	appendLine("non_existing_file.txt");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("File 'non_existing_file.txt' does not exist"));
	ASSERT_TRUE(allActionsCompleted());
}
void newVotingRoundSelectItemsTooFewItems() {
	createItemsFile(1);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Too few items in '" + std::string{ kTestItemsFile } + "'. At least two expected."));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void newVotingRoundSelectFormatLegendPrintedOnce() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	// Repeat an invalid key
	for (uint32_t i = 0; i < 10; i++) {
		appendAction(KeyAction::VoteA);
	}
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select voting format"));
	ASSERT_FALSE(catcher.contains("Select voting format", 2));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void newVotingRoundSelectFormatCancel() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void newVotingRoundSelectFormatPrintHelp() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::Help);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Score-based voting will"));
	ASSERT_TRUE(catcher.contains("Rank-based voting will"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}

void quit() {
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Pairwise ranking"));
	ASSERT_TRUE(catcher.contains("[N]ew voting round"));
	ASSERT_TRUE(catcher.contains("[L]oad voting round"));
	ASSERT_TRUE(catcher.contains("[C]ombine scores"));
	ASSERT_TRUE(catcher.contains("[Q]uit"));
	ASSERT_TRUE(allActionsCompleted());
}
void quitWhenUnsaved() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("You have unsaved progress"));
	ASSERT_TRUE(catcher.contains("Save before quitting?"));
	ASSERT_TRUE(catcher.contains("[Y]es"));
	ASSERT_TRUE(catcher.contains("[N]o (lose progress)"));
	ASSERT_TRUE(catcher.contains("[C]ancel"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void quitWhenUnsavedThenCancel() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("You have unsaved progress", 2));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void loadVotingRound() {
	createVotesFile(2);

	appendAction(KeyAction::LoadRound);
	appendLine(kTestVotesFile);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select file name to load voting round from"));
	ASSERT_TRUE(catcher.contains("Voting round loaded from '" + std::string{ kTestVotesFile } + "'"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void loadVotingRoundThenCancel() {
	appendAction(KeyAction::LoadRound);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select file name to load voting round from"));
	ASSERT_FALSE(catcher.contains("Voting round loaded"));
	ASSERT_TRUE(allActionsCompleted());
}
void loadVotingRoundFromBlankFileName() {
	appendAction(KeyAction::LoadRound);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select file name to load voting round from", 2));
	ASSERT_FALSE(catcher.contains("Voting round loaded"));
	ASSERT_TRUE(allActionsCompleted());
}
void loadNonExistingVotingRound() {
	cleanUpFiles();
	appendAction(KeyAction::LoadRound);
	appendLine(kTestVotesFile);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("File '" + std::string{ kTestVotesFile } + "' does not exist"));
	ASSERT_TRUE(allActionsCompleted());
}
void loadVotingRoundFromEmptyFile() {
	std::ofstream votes_file(kTestVotesFile);
	ASSERT_TRUE(votes_file.is_open());
	votes_file.close();

	appendAction(KeyAction::LoadRound);
	appendLine(kTestVotesFile);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No lines found in '" + std::string{ kTestVotesFile } + "'"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void loadVotingRoundFromInvalidFile() {
	std::ofstream votes_file(kTestVotesFile);
	ASSERT_TRUE(votes_file.is_open());
	votes_file << "item1\n";
	votes_file.close();

	appendAction(KeyAction::LoadRound);
	appendLine(kTestVotesFile);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Failed to create voting round from '" + std::string{ kTestVotesFile } + "'"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRound() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Save);
	appendLine(kTestVotesFile);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select file name to save voting round to"));
	ASSERT_TRUE(catcher.contains("Voting round saved to '" + std::string{ kTestVotesFile } + "'"));
	ASSERT_FALSE(catcher.contains("Select file name to save scores"));
	ASSERT_TRUE(std::filesystem::exists(kTestVotesFile));
	ASSERT_FALSE(std::filesystem::exists(kTestScoresFile));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveScores() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Save);
	appendLine(kTestVotesFile);
	appendLine(kTestScoresFile);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select file name to save scores"));
	ASSERT_TRUE(catcher.contains("Scores saved to '" + std::string{ kTestScoresFile } + "'"));
	ASSERT_TRUE(std::filesystem::exists(kTestScoresFile));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveRanking() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::RankedRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Save);
	appendLine(kTestVotesFile);
	appendLine(kTestRankingFile);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select file name to save ranking"));
	ASSERT_TRUE(catcher.contains("Ranking saved to '" + std::string{ kTestRankingFile } + "'"));
	ASSERT_TRUE(std::filesystem::exists(kTestRankingFile));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void vote() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteB);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_FALSE(catcher.contains("Can't vote. Voting round is completed."));
	ASSERT_TRUE(catcher.contains("Voting round completed"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void voteWhenVotingRoundCompleted() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::VoteB);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Can't vote. Voting round is completed.", 2));
	ASSERT_TRUE(catcher.contains("Voting round completed", 3));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void undoWithNoVotes() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Undo);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No votes to undo"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void undoScoreVote() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Undo);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_FALSE(catcher.contains("No votes to undo"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void undoRankVote() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::RankedRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Undo);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_FALSE(catcher.contains("No votes to undo"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void printScore() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Print);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Item"));
	ASSERT_TRUE(catcher.contains("Wins"));
	ASSERT_TRUE(catcher.contains("Losses"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void printRank() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::RankedRound);
	appendAction(KeyAction::Print);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("<-- sorted until here"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void combine() {
	std::string const file_name_1{ "temp_scores1.txt" };
	std::string const file_name_2{ "temp_scores2.txt" };
	std::ofstream score_file_1(file_name_1);
	std::ofstream score_file_2(file_name_2);
	ASSERT_TRUE(score_file_1.is_open() && score_file_2.is_open());
	score_file_1 << "1 0 item1\n";
	score_file_2 << "1 1 item1\n";
	score_file_1.close();
	score_file_2.close();

	appendAction(KeyAction::Combine);
	appendLine(file_name_1 + " " + file_name_2);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select two or more score files to combine"));
	ASSERT_TRUE(catcher.contains("Reading " + file_name_1));
	ASSERT_TRUE(catcher.contains("Reading " + file_name_2));
	ASSERT_TRUE(catcher.contains("Scores combined"));
	ASSERT_TRUE(catcher.contains("[P]rint combined scores"));
	ASSERT_TRUE(catcher.contains("[S]ave combined scores"));
	ASSERT_FALSE(std::filesystem::exists(kTestCombinedScoresFile));
	ASSERT_TRUE(allActionsCompleted());

	std::filesystem::remove(file_name_1);
	std::filesystem::remove(file_name_2);
	cleanUpFiles();
}
void combineThenPrintScores() {
	std::string const file_name_1{ "temp_scores1.txt" };
	std::string const file_name_2{ "temp_scores2.txt" };
	std::ofstream score_file_1(file_name_1);
	std::ofstream score_file_2(file_name_2);
	ASSERT_TRUE(score_file_1.is_open() && score_file_2.is_open());
	score_file_1 << "1 0 item1\n";
	score_file_2 << "1 1 item1\n";
	score_file_1.close();
	score_file_2.close();

	appendAction(KeyAction::Combine);
	appendLine(file_name_1 + " " + file_name_2);
	appendAction(KeyAction::Print);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Item"));
	ASSERT_TRUE(catcher.contains("Wins"));
	ASSERT_TRUE(catcher.contains("Losses"));
	ASSERT_TRUE(catcher.contains("2"));
	ASSERT_FALSE(catcher.contains("0"));
	ASSERT_FALSE(std::filesystem::exists(kTestCombinedScoresFile));
	ASSERT_TRUE(allActionsCompleted());

	std::filesystem::remove(file_name_1);
	std::filesystem::remove(file_name_2);
	cleanUpFiles();
}
void combineThenSaveScores() {
	std::string const file_name_1{ "temp_scores1.txt" };
	std::string const file_name_2{ "temp_scores2.txt" };
	std::ofstream score_file_1(file_name_1);
	std::ofstream score_file_2(file_name_2);
	ASSERT_TRUE(score_file_1.is_open() && score_file_2.is_open());
	score_file_1 << "1 0 item1\n";
	score_file_2 << "1 1 item1\n";
	score_file_1.close();
	score_file_2.close();

	appendAction(KeyAction::Combine);
	appendLine(file_name_1 + " " + file_name_2);
	appendAction(KeyAction::Save);
	appendLine(kTestCombinedScoresFile);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Saved combined scores to '" + std::string{ kTestCombinedScoresFile } + "'"));
	ASSERT_TRUE(std::filesystem::exists(kTestCombinedScoresFile));
	ASSERT_TRUE(allActionsCompleted());

	std::filesystem::remove(file_name_1);
	std::filesystem::remove(file_name_2);
	cleanUpFiles();
}
void combineThenSaveScoresCancel() {
	std::string const file_name_1{ "temp_scores1.txt" };
	std::string const file_name_2{ "temp_scores2.txt" };
	std::ofstream score_file_1(file_name_1);
	std::ofstream score_file_2(file_name_2);
	ASSERT_TRUE(score_file_1.is_open() && score_file_2.is_open());
	score_file_1 << "1 0 item1\n";
	score_file_2 << "1 1 item1\n";
	score_file_1.close();
	score_file_2.close();

	appendAction(KeyAction::Combine);
	appendLine(file_name_1 + " " + file_name_2);
	appendAction(KeyAction::Save);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Scores combined", 2));
	ASSERT_TRUE(catcher.contains("Select file name to save combined scores to"));
	ASSERT_FALSE(catcher.contains("Saved combined scores"));
	ASSERT_TRUE(allActionsCompleted());

	std::filesystem::remove(file_name_1);
	std::filesystem::remove(file_name_2);
	cleanUpFiles();
}
void combineCancel() {
	appendAction(KeyAction::Combine);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select two or more score files to combine"));
	ASSERT_TRUE(allActionsCompleted());
}
void combineWithTooFewFiles() {
	appendAction(KeyAction::Combine);
	appendLine("file1");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Too few files. No scores combined"));
	ASSERT_TRUE(allActionsCompleted());
}
void combineWithNonExistingFiles() {
	appendAction(KeyAction::Combine);
	appendLine("file1 file2");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("File 'file1' doesn't exist"));
	ASSERT_TRUE(catcher.contains("File 'file2' doesn't exist"));
	ASSERT_TRUE(catcher.contains("No scores combined"));
	ASSERT_TRUE(allActionsCompleted());
}

} // namespace

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	cleanUpFiles();

	RUN_TEST_IF_ARGUMENT_EQUALS(mainMenuLegendPrintedOnce);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectItemsPromptPrintedEachTime);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectItemsCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectItemsNonExistingFile);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectItemsTooFewItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectFormatLegendPrintedOnce);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectFormatCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectFormatPrintHelp);
	RUN_TEST_IF_ARGUMENT_EQUALS(quit);
	RUN_TEST_IF_ARGUMENT_EQUALS(quitWhenUnsaved);
	RUN_TEST_IF_ARGUMENT_EQUALS(quitWhenUnsavedThenCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundThenCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundFromBlankFileName);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadNonExistingVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundFromEmptyFile);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundFromInvalidFile);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveScores);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveRanking);
	RUN_TEST_IF_ARGUMENT_EQUALS(vote);
	RUN_TEST_IF_ARGUMENT_EQUALS(voteWhenVotingRoundCompleted);
	RUN_TEST_IF_ARGUMENT_EQUALS(undoWithNoVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(undoScoreVote);
	RUN_TEST_IF_ARGUMENT_EQUALS(undoRankVote);
	RUN_TEST_IF_ARGUMENT_EQUALS(printScore);
	RUN_TEST_IF_ARGUMENT_EQUALS(printRank);
	RUN_TEST_IF_ARGUMENT_EQUALS(combine);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineThenPrintScores);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineThenSaveScores);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineThenSaveScoresCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineWithTooFewFiles);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineWithNonExistingFiles);
	return 1;
}
