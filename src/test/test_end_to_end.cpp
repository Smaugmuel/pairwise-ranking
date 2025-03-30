#include "testing.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <queue>

#include "helpers.h"
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

// NOTE: How to trigger this failing?
//void saveVotingRoundFails() {
//}
//void saveVotingRoundScoresFails() {
//}
//void saveVotingRoundRankingFails() {
//}

void mainMenuLegendPrinted() {
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
void mainMenuLegendNotReprinted() {
	// Repeat an invalid key
	for (uint32_t i = 0; i < 10; i++) {
		appendAction(KeyAction::VoteA);
	}
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_EQ(catcher.occurrences("Pairwise ranking"), 1);
	ASSERT_TRUE(allActionsCompleted());
}
void mainMenuNewVotingRound() {
	appendAction(KeyAction::NewRound);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select file to load items from"));
	ASSERT_TRUE(allActionsCompleted());
}
void mainMenuLoadVotingRound() {
	appendAction(KeyAction::LoadRound);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select file name to load voting round from"));
	ASSERT_TRUE(allActionsCompleted());
}
void mainMenuCombine() {
	appendAction(KeyAction::Combine);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select two or more score files to combine"));
	ASSERT_TRUE(allActionsCompleted());
}
void mainMenuQuit() {
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_FALSE(catcher.contains("Select file to load items from"));
	ASSERT_FALSE(catcher.contains("Select file name to load voting round from"));
	ASSERT_FALSE(catcher.contains("Select two or more score files to combine"));
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

	ASSERT_EQ(catcher.occurrences("Select file to load items from"), 11);
	ASSERT_TRUE(allActionsCompleted());
}
void newVotingRoundSelectItemsEmptyFileName() {
	appendAction(KeyAction::NewRound);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No file name selected"));
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
void newVotingRoundSelectItemsSuccessful() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select voting format"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void newVotingRoundSelectFormatLegendPrinted() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select voting format"));
	ASSERT_TRUE(catcher.contains("[F]ull score-based voting"));
	ASSERT_TRUE(catcher.contains("[R]educed score-based voting"));
	ASSERT_TRUE(catcher.contains("[I]nsertion rank-based voting"));
	ASSERT_TRUE(catcher.contains("[H]elp"));
	ASSERT_TRUE(catcher.contains("[C]ancel"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void newVotingRoundSelectFormatEstimatedVotesPrinted() {
	createItemsFile(10);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Estimated votes"));
	ASSERT_TRUE(catcher.contains("45"));  // Full
	ASSERT_TRUE(catcher.contains("15"));  // Reduced
	ASSERT_TRUE(catcher.contains("33"));  // Ranked
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void newVotingRoundSelectFormatLegendNotReprinted() {
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

	ASSERT_EQ(catcher.occurrences("Select voting format"), 1);
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
void newVotingRoundSelectFormatSuccessful() {
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

	ASSERT_TRUE(catcher.contains("Voting round"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void loadVotingRoundPromptPrintedEachTime() {
	appendAction(KeyAction::LoadRound);
	for (uint32_t i = 0; i < 10; i++) {
		appendLine("");
	}
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_EQ(catcher.occurrences("Select file name to load voting round from"), 11);
	ASSERT_TRUE(allActionsCompleted());
}
void loadVotingRoundEmptyFileName() {
	appendAction(KeyAction::LoadRound);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No file name selected"));
	ASSERT_TRUE(allActionsCompleted());
}
void loadVotingRoundCancel() {
	appendAction(KeyAction::LoadRound);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	programLoop();

	ASSERT_TRUE(allActionsCompleted());
}
void loadVotingRoundNonExistingFile() {
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
void loadVotingRoundEmptyFile() {
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
void loadVotingRoundInvalidFile() {
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
void loadVotingRoundSuccessful() {
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
void saveVotingRoundPromptPrintedEachTime() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Save);
	for (uint32_t i = 0; i < 10; i++) {
		appendLine("");
	}
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_EQ(catcher.occurrences("Select file name to save voting round to"), 11);
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundEmptyFileName() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Save);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No file name selected"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundCancel() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Save);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	programLoop();

	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundSuccessful() {
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

	ASSERT_TRUE(catcher.contains("Voting round saved to '" + std::string{ kTestVotesFile } + "'"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundScoresNoVotes() {
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

	ASSERT_FALSE(catcher.contains("Select file name to save scores to"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundScoresPromptPrintedEachTime() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Save);
	appendLine(kTestVotesFile);
	for (uint32_t i = 0; i < 10; i++) {
		appendLine("");
	}
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_EQ(catcher.occurrences("Select file name to save scores to"), 11);
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundScoresEmptyFileName() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Save);
	appendLine(kTestVotesFile);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No file name selected"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundScoresCancel() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Save);
	appendLine(kTestVotesFile);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	programLoop();

	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundScoresSuccessful() {
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

	ASSERT_TRUE(catcher.contains("Scores saved to '" + std::string{ kTestScoresFile } + "'"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundRankingNoVotes() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::RankedRound);
	appendAction(KeyAction::Save);
	appendLine(kTestVotesFile);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_FALSE(catcher.contains("Select file name to save ranking to"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundRankingPromptPrintedEachTime() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::RankedRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Save);
	appendLine(kTestVotesFile);
	for (uint32_t i = 0; i < 10; i++) {
		appendLine("");
	}
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_EQ(catcher.occurrences("Select file name to save ranking to"), 11);
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundRankingEmptyFileName() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::RankedRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Save);
	appendLine(kTestVotesFile);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No file name selected"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundRankingCancel() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::RankedRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Save);
	appendLine(kTestVotesFile);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	programLoop();

	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundRankingSuccessful() {
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

	ASSERT_TRUE(catcher.contains("Ranking saved to '" + std::string{ kTestRankingFile } + "'"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void saveVotingRoundRankingSortedMarked() {
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

	programLoop();

	ASSERT_TRUE(allActionsCompleted());

	auto const ranking_lines = loadFile(kTestRankingFile);
	auto has_sorted_marker = false;
	for (auto const& line : ranking_lines) {
		if (line.find("<-- sorted until here") != std::string::npos) {
			has_sorted_marker = true;
			break;
		}
	}
	ASSERT_TRUE(has_sorted_marker);

	cleanUpFiles();
}
void votingLegendPrinted() {
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

	ASSERT_TRUE(catcher.contains("Voting round"));
	ASSERT_TRUE(catcher.contains("Vote option [A]"));
	ASSERT_TRUE(catcher.contains("Vote option [B]"));
	ASSERT_TRUE(catcher.contains("[U]ndo vote"));
	ASSERT_TRUE(catcher.contains("[P]rint current score"));
	ASSERT_TRUE(catcher.contains("[S]ave votes"));
	ASSERT_TRUE(catcher.contains("[Q]uit to main menu"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingLegendNotReprinted() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::FullRound); // Invalid option during voting
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_EQ(catcher.occurrences("Voting round"), 1);
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingMatchupPrintedEachTime() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	// Repeat an invalid key
	for (uint32_t i = 0; i < 10; i++) {
		appendAction(KeyAction::FullRound);
	}
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_EQ(catcher.occurrences("A:"), 11);
	ASSERT_EQ(catcher.occurrences("B:"), 11);
	ASSERT_EQ(catcher.occurrences("Your choice"), 11);
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingCompletedPrinted() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Voting round completed"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingOptionA() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_FALSE(catcher.contains("Can't vote. Voting round is completed."));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingOptionB() {
	createItemsFile(2);

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
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingCompletedVoteOptionA() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Can't vote. Voting round is completed."));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingCompletedVoteOptionB() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::VoteB);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Can't vote. Voting round is completed."));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingUndoScoreVote() {
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
void votingUndoRankVote() {
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
void votingUndoWithNoVotes() {
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
void votingPrintScore() {
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
void votingPrintRank() {
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
void votingQuitWhenSaved() {
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

	ASSERT_FALSE(catcher.contains("You have unsaved progress"));
	ASSERT_TRUE(std::filesystem::exists(kTestVotesFile));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingQuitWhenUnsavedLegendPrinted() {
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
void votingQuitWhenUnsavedLegendNotReprinted() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::FullRound); // Invalid input
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_EQ(catcher.occurrences("You have unsaved progress"), 1);
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingQuitWhenUnsavedThenSave() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Select file name to save voting round to"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingQuitWhenUnsavedThenDontSave() {
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

	ASSERT_FALSE(catcher.contains("Select file name to save voting round to"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingQuitWhenUnsavedThenCancel() {
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

	ASSERT_EQ(catcher.occurrences("You have unsaved progress"), 2);
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void votingInvalidKey() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Invalid key"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void combineEmptyFileNames() {
	appendAction(KeyAction::Combine);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No file names selected"));
	ASSERT_TRUE(allActionsCompleted());
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

	ASSERT_EQ(catcher.occurrences("Scores combined"), 2);
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

	RUN_TEST_IF_ARGUMENT_EQUALS(mainMenuLegendPrinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(mainMenuLegendNotReprinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(mainMenuNewVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(mainMenuLoadVotingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(mainMenuCombine);
	RUN_TEST_IF_ARGUMENT_EQUALS(mainMenuQuit);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectItemsPromptPrintedEachTime);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectItemsEmptyFileName);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectItemsCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectItemsNonExistingFile);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectItemsTooFewItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectItemsSuccessful);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectFormatLegendPrinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectFormatEstimatedVotesPrinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectFormatLegendNotReprinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectFormatCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectFormatPrintHelp);
	RUN_TEST_IF_ARGUMENT_EQUALS(newVotingRoundSelectFormatSuccessful);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundPromptPrintedEachTime);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundEmptyFileName);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundNonExistingFile);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundEmptyFile);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundInvalidFile);
	RUN_TEST_IF_ARGUMENT_EQUALS(loadVotingRoundSuccessful);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundPromptPrintedEachTime);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundEmptyFileName);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundSuccessful);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundScoresNoVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundScoresPromptPrintedEachTime);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundScoresEmptyFileName);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundScoresCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundScoresSuccessful);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundRankingNoVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundRankingPromptPrintedEachTime);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundRankingEmptyFileName);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundRankingCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundRankingSuccessful);
	RUN_TEST_IF_ARGUMENT_EQUALS(saveVotingRoundRankingSortedMarked);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingLegendPrinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingLegendNotReprinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingMatchupPrintedEachTime);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingCompletedPrinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingOptionA);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingOptionB);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingCompletedVoteOptionA);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingCompletedVoteOptionB);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingUndoScoreVote);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingUndoRankVote);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingUndoWithNoVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingPrintScore);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingPrintRank);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingQuitWhenSaved);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingQuitWhenUnsavedLegendPrinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingQuitWhenUnsavedLegendNotReprinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingQuitWhenUnsavedThenSave);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingQuitWhenUnsavedThenDontSave);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingQuitWhenUnsavedThenCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingInvalidKey);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineEmptyFileNames);
	RUN_TEST_IF_ARGUMENT_EQUALS(combine);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineThenPrintScores);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineThenSaveScores);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineThenSaveScoresCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineWithTooFewFiles);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineWithNonExistingFiles);
	return 1;
}
