#include "testing.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <queue>

#include "helpers.h"
#include "mocks/log_catcher.h"
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
	// General
	Quit = 'q',
	Cancel = 'c',
	Help = 'h',
	// Main menu options
	NewRound = 'n',
	LoadRound = 'l',
	Combine = 'c',
	// Voting round format
	FullRound = 'f',
	RankedRound = 'i',
	// Voting ongoing
	Save = 's',
	VoteA = 'a',
	VoteB = 'b',
	Undo = 'u',
	// Voting misc
	Yes = 'y',
	No = 'n',
	Print = 'p',
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
auto createScoresFiles(uint32_t number_of_files) -> std::vector<std::string> {
	std::vector<std::string> file_names{};
	for (uint32_t i = 0; i < number_of_files; i++) {
		auto file_name = "temp_scores_" + std::to_string(i) + ".txt";
		std::ofstream score_file(file_name);
		ASSERT_TRUE(score_file.is_open());
		score_file << "1 1 item1\n";
		score_file.close();
		file_names.emplace_back(std::move(file_name));
	}
	return file_names;
}
void cleanUpFiles(std::vector<std::string> extra_files = {}) {
	std::filesystem::remove(kTestItemsFile);
	std::filesystem::remove(kTestVotesFile);
	std::filesystem::remove(kTestScoresFile);
	std::filesystem::remove(kTestRankingFile);
	std::filesystem::remove(kTestCombinedScoresFile);
	for (auto file_name : extra_files) {
		std::filesystem::remove(file_name);
	}
}

auto runProgramLoopAndCatchLogs() -> LogCatcher {
	LogCatcher log_catcher;
	programLoop();
	log_catcher.stop();
	return log_catcher;
}

// NOTE: How to trigger these failing?
//void saveVotingRoundFails( ) {
//}
//void saveVotingRoundScoresFails( ) {
//}
//void saveVotingRoundRankingFails( ) {
//}
//void combineSaveScoresFails( ) {
//}

void mainMenuLegendPrinted() {
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Pairwise ranking"));
	ASSERT_TRUE(logs.contains("[N]ew voting round"));
	ASSERT_TRUE(logs.contains("[L]oad voting round"));
	ASSERT_TRUE(logs.contains("[C]ombine scores"));
	ASSERT_TRUE(logs.contains("[Q]uit"));
}
void mainMenuLegendNotReprinted() {
	// Repeat an invalid key
	for (uint32_t i = 0; i < 10; i++) {
		appendAction(KeyAction::VoteA);
	}
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Pairwise ranking"), 1);
}
void mainMenuNewVotingRound() {
	appendAction(KeyAction::NewRound);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Select file to load items from"));
}
void mainMenuLoadVotingRound() {
	appendAction(KeyAction::LoadRound);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Select file name to load voting round from"));
}
void mainMenuCombine() {
	appendAction(KeyAction::Combine);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Select two or more score files to combine"));
}
void mainMenuQuit() {
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_FALSE(logs.contains("Select file to load items from"));
	ASSERT_FALSE(logs.contains("Select file name to load voting round from"));
	ASSERT_FALSE(logs.contains("Select two or more score files to combine"));
}
void newVotingRoundSelectItemsPromptPrintedEachTime() {
	appendAction(KeyAction::NewRound);
	for (uint32_t i = 0; i < 10; i++) {
		appendLine("");
	}
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Select file to load items from"), 11);
}
void newVotingRoundSelectItemsEmptyFileName() {
	appendAction(KeyAction::NewRound);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("No file name selected"));
}
void newVotingRoundSelectItemsCancel() {
	appendAction(KeyAction::NewRound);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	programLoop();
}
void newVotingRoundSelectItemsNonExistingFile() {
	appendAction(KeyAction::NewRound);
	appendLine("non_existing_file.txt");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("File 'non_existing_file.txt' does not exist"));
}
void newVotingRoundSelectItemsTooFewItems() {
	createItemsFile(1);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Too few items in '" + std::string{ kTestItemsFile } + "'. At least two expected."));
	cleanUpFiles();
}
void newVotingRoundSelectItemsSuccessful() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Select voting format"));
	cleanUpFiles();
}
void newVotingRoundSelectFormatLegendPrinted() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Select voting format"));
	ASSERT_TRUE(logs.contains("[F]ull score-based voting"));
	ASSERT_TRUE(logs.contains("[R]educed score-based voting"));
	ASSERT_TRUE(logs.contains("[I]nsertion rank-based voting"));
	ASSERT_TRUE(logs.contains("[H]elp"));
	ASSERT_TRUE(logs.contains("[C]ancel"));
	cleanUpFiles();
}
void newVotingRoundSelectFormatEstimatedVotesPrinted() {
	createItemsFile(10);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Estimated votes"));
	ASSERT_TRUE(logs.contains("45"));  // Full
	ASSERT_TRUE(logs.contains("15"));  // Reduced
	ASSERT_TRUE(logs.contains("33"));  // Ranked
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Select voting format"), 1);
	cleanUpFiles();
}
void newVotingRoundSelectFormatCancel() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	cleanUpFiles();
}
void newVotingRoundSelectFormatPrintHelp() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendLine(kTestItemsFile);
	appendAction(KeyAction::Help);
	appendAction(KeyAction::Cancel);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Score-based voting will"));
	ASSERT_TRUE(logs.contains("Rank-based voting will"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Voting round"));
	cleanUpFiles();
}
void loadVotingRoundPromptPrintedEachTime() {
	appendAction(KeyAction::LoadRound);
	for (uint32_t i = 0; i < 10; i++) {
		appendLine("");
	}
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Select file name to load voting round from"), 11);
}
void loadVotingRoundEmptyFileName() {
	appendAction(KeyAction::LoadRound);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("No file name selected"));
}
void loadVotingRoundCancel() {
	appendAction(KeyAction::LoadRound);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	programLoop();
}
void loadVotingRoundNonExistingFile() {
	cleanUpFiles();
	appendAction(KeyAction::LoadRound);
	appendLine(kTestVotesFile);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("File '" + std::string{ kTestVotesFile } + "' does not exist"));
}
void loadVotingRoundEmptyFile() {
	std::ofstream votes_file(kTestVotesFile);
	ASSERT_TRUE(votes_file.is_open());
	votes_file.close();

	appendAction(KeyAction::LoadRound);
	appendLine(kTestVotesFile);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("No lines found in '" + std::string{ kTestVotesFile } + "'"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Failed to create voting round from '" + std::string{ kTestVotesFile } + "'"));
	cleanUpFiles();
}
void loadVotingRoundSuccessful() {
	createVotesFile(2);

	appendAction(KeyAction::LoadRound);
	appendLine(kTestVotesFile);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Select file name to load voting round from"));
	ASSERT_TRUE(logs.contains("Voting round loaded from '" + std::string{ kTestVotesFile } + "'"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Select file name to save voting round to"), 11);
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("No file name selected"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Voting round saved to '" + std::string{ kTestVotesFile } + "'"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_FALSE(logs.contains("Select file name to save scores to"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Select file name to save scores to"), 11);
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("No file name selected"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Scores saved to '" + std::string{ kTestScoresFile } + "'"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_FALSE(logs.contains("Select file name to save ranking to"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Select file name to save ranking to"), 11);
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("No file name selected"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Ranking saved to '" + std::string{ kTestRankingFile } + "'"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Voting round"));
	ASSERT_TRUE(logs.contains("Vote option [A]"));
	ASSERT_TRUE(logs.contains("Vote option [B]"));
	ASSERT_TRUE(logs.contains("[U]ndo vote"));
	ASSERT_TRUE(logs.contains("[P]rint current score"));
	ASSERT_TRUE(logs.contains("[S]ave votes"));
	ASSERT_TRUE(logs.contains("[Q]uit to main menu"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Voting round"), 1);
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("A:"), 11);
	ASSERT_EQ(logs.occurrences("B:"), 11);
	ASSERT_EQ(logs.occurrences("Your choice"), 11);
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Voting round completed"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_FALSE(logs.contains("Can't vote. Voting round is completed."));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_FALSE(logs.contains("Can't vote. Voting round is completed."));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Can't vote. Voting round is completed."));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Can't vote. Voting round is completed."));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_FALSE(logs.contains("No votes to undo"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_FALSE(logs.contains("No votes to undo"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("No votes to undo"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Item"));
	ASSERT_TRUE(logs.contains("Wins"));
	ASSERT_TRUE(logs.contains("Losses"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("<-- sorted until here"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_FALSE(logs.contains("You have unsaved progress"));
	ASSERT_TRUE(std::filesystem::exists(kTestVotesFile));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("You have unsaved progress"));
	ASSERT_TRUE(logs.contains("Save before quitting?"));
	ASSERT_TRUE(logs.contains("[Y]es"));
	ASSERT_TRUE(logs.contains("[N]o (lose progress)"));
	ASSERT_TRUE(logs.contains("[C]ancel"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("You have unsaved progress"), 1);
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Select file name to save voting round to"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_FALSE(logs.contains("Select file name to save voting round to"));
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("You have unsaved progress"), 2);
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

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Invalid key"));
	cleanUpFiles();
}
void combinePromptPrintedEachTime() {
	appendAction(KeyAction::Combine);
	for (uint32_t i = 0; i < 10; i++) {
		appendLine("");
	}
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Select two or more score files to combine"), 11);
}
void combineEmptyFileNames() {
	appendAction(KeyAction::Combine);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("No file names selected"));
}
void combineCancel() {
	appendAction(KeyAction::Combine);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	programLoop();
}
void combineTooFewFiles() {
	appendAction(KeyAction::Combine);
	appendLine("file1");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Too few files. No scores combined"));
}
void combineNonExistingFiles() {
	appendAction(KeyAction::Combine);
	appendLine("file1 file2");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("File 'file1' doesn't exist"));
	ASSERT_TRUE(logs.contains("File 'file2' doesn't exist"));
	ASSERT_TRUE(logs.contains("No scores combined"));
}
void combineInvalidFiles() {
	auto file_names = createScoresFiles(1);
	std::string const invalid_file_name{ "temp_scores_2.txt" };
	std::ofstream invalid_score_file(invalid_file_name);
	ASSERT_TRUE(invalid_score_file.is_open());
	invalid_score_file << "0 item1\n";
	invalid_score_file.close();

	appendAction(KeyAction::Combine);
	appendLine(file_names[0] + " " + invalid_file_name);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("File '" + invalid_file_name + "' is invalid"));
	ASSERT_TRUE(logs.contains("No scores combined"));
	ASSERT_FALSE(std::filesystem::exists(kTestCombinedScoresFile));

	cleanUpFiles(file_names + std::vector<std::string>{ invalid_file_name });
}
void combineSuccessful() {
	auto const file_names = createScoresFiles(2);

	appendAction(KeyAction::Combine);
	appendLine(file_names[0] + " " + file_names[1]);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Reading " + file_names[0]));
	ASSERT_TRUE(logs.contains("Reading " + file_names[1]));
	ASSERT_TRUE(logs.contains("Scores were combined"));

	cleanUpFiles(file_names);
}
void combineViewScoresLegendPrinted() {
	auto const file_names = createScoresFiles(2);

	appendAction(KeyAction::Combine);
	appendLine(file_names[0] + " " + file_names[1]);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Scores combined"));
	ASSERT_TRUE(logs.contains("[P]rint combined scores"));
	ASSERT_TRUE(logs.contains("[S]ave combined scores"));
	ASSERT_TRUE(logs.contains("[Q]uit to main menu"));

	cleanUpFiles(file_names);
}
void combineViewScoresLegendNotReprinted() {
	auto const file_names = createScoresFiles(2);

	appendAction(KeyAction::Combine);
	appendLine(file_names[0] + " " + file_names[1]);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Scores combined"), 1);

	cleanUpFiles(file_names);
}
void combineViewScoresDoesNotAutomaticallySave() {
	auto const file_names = createScoresFiles(2);

	appendAction(KeyAction::Combine);
	appendLine(file_names[0] + " " + file_names[1]);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_FALSE(std::filesystem::exists(kTestCombinedScoresFile));

	cleanUpFiles(file_names);
}
void combineViewScoresPrint() {
	auto const file_names = createScoresFiles(2);

	appendAction(KeyAction::Combine);
	appendLine(file_names[0] + " " + file_names[1]);
	appendAction(KeyAction::Print);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Item"));
	ASSERT_TRUE(logs.contains("Wins"));
	ASSERT_TRUE(logs.contains("Losses"));

	cleanUpFiles(file_names);
}
void combineSaveScoresPromptPrintedEachTime() {
	auto const file_names = createScoresFiles(2);

	appendAction(KeyAction::Combine);
	appendLine(file_names[0] + " " + file_names[1]);
	appendAction(KeyAction::Save);
	// Repeat invalid action
	for (uint32_t i = 0; i < 10; i++) {
		appendLine("");
	}
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_EQ(logs.occurrences("Select file name to save combined scores to"), 11);

	cleanUpFiles(file_names);
}
void combineSaveScoresEmptyFileName() {
	auto const file_names = createScoresFiles(2);

	appendAction(KeyAction::Combine);
	appendLine(file_names[0] + " " + file_names[1]);
	appendAction(KeyAction::Save);
	appendLine("");
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("No file name selected"));

	cleanUpFiles(file_names);
}
void combineSaveScoresCancel() {
	auto const file_names = createScoresFiles(2);

	appendAction(KeyAction::Combine);
	appendLine(file_names[0] + " " + file_names[1]);
	appendAction(KeyAction::Save);
	appendLine(std::string{ static_cast<char>(KeyAction::Cancel) });
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Quit);

	programLoop();

	cleanUpFiles(file_names);
}
void combineSaveScoresSuccessful() {
	auto const file_names = createScoresFiles(2);

	appendAction(KeyAction::Combine);
	appendLine(file_names[0] + " " + file_names[1]);
	appendAction(KeyAction::Save);
	appendLine(kTestCombinedScoresFile);
	appendAction(KeyAction::Quit);

	auto const logs = runProgramLoopAndCatchLogs();

	ASSERT_TRUE(logs.contains("Saved combined scores to '" + std::string{ kTestCombinedScoresFile } + "'"));
	ASSERT_TRUE(std::filesystem::exists(kTestCombinedScoresFile));

	cleanUpFiles(file_names);
}

auto run_tests(std::string const& test) -> bool {
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
	RUN_TEST_IF_ARGUMENT_EQUALS(combinePromptPrintedEachTime);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineEmptyFileNames);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineTooFewFiles);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineNonExistingFiles);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineInvalidFiles);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineSuccessful);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineViewScoresLegendPrinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineViewScoresLegendNotReprinted);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineViewScoresDoesNotAutomaticallySave);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineViewScoresPrint);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineSaveScoresPromptPrintedEachTime);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineSaveScoresEmptyFileName);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineSaveScoresCancel);
	RUN_TEST_IF_ARGUMENT_EQUALS(combineSaveScoresSuccessful);
	return true;
}

} // namespace

auto test_end_to_end(std::string const& test_case) -> int {
	cleanUpFiles();
	if (run_tests(test_case)) {
		return 1;
	}
	ASSERT_TRUE(allActionsCompleted());
	return 0;
}
