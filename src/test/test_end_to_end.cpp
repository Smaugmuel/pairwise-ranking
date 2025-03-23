#include "testing.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <queue>

#include "constants.h"
#include "mocks/output_catcher.h"
#include "program_loop.h"

extern std::queue<char> g_keys;
extern std::queue<std::string> g_lines;

namespace
{

enum class KeyAction {
	Quit = 'q',
	NewRound = 'n',
	LoadRound = 'l',
	FullRound = 'f',
	RankedRound = 'r',
	Save = 's',
	Yes = 'y',
	No = 'n',
	VoteA = 'a',
	VoteB = 'b',
	Undo = 'u',
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
	std::ofstream items_file(kItemsFile);
	ASSERT_TRUE(items_file.is_open());
	auto const items = getNItems(number_of_items);
	for (auto const& item : items) {
		items_file << item << "\n";
	}
	items_file.close();
}
void createVotesFile(uint32_t number_of_items) {
	std::ofstream votes_file(kVotesFile);
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
	std::filesystem::remove(kItemsFile);
	std::filesystem::remove(kVotesFile);
	std::filesystem::remove(kScoresFile);
}

void endToEnd_quit() {
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Welcome! Press H if you need help"));
	ASSERT_TRUE(allActionsCompleted());
}
void endToEnd_continueWithoutSavingWhenCreatingNewRound() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::Yes);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("You have unsaved data. Do you still wish to create new voting round?"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_continueWithoutSavingCancelWhenCreatingNewRound() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("You have unsaved data. Do you still wish to create new voting round?"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_continueWithoutSavingWhenLoadingRound() {
	createItemsFile(2);
	createVotesFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::LoadRound);
	appendAction(KeyAction::Yes);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("You have unsaved data. Do you still wish to load a voting round?"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_continueWithoutSavingCancelWhenLoadingRound() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::LoadRound);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("You have unsaved data. Do you still wish to load a voting round?"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_continueWithoutSavingWhenQuitting() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("You have unsaved data. Do you still wish to quit?"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_continueWithoutSavingCancelWhenQuitting() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::No);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("You have unsaved data. Do you still wish to quit?", 2));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_loadEmptyRound() {
	appendAction(KeyAction::LoadRound);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No lines loaded"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_saveBeforeCreatingRound() {
	appendAction(KeyAction::Save);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No voting round to save from"));
	ASSERT_FALSE(std::filesystem::exists(kVotesFile));
	ASSERT_TRUE(allActionsCompleted());
}
void endToEnd_saveRoundWithNoVotes() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Save);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Votes saved to " + kVotesFile + "."));
	ASSERT_TRUE(catcher.contains("Could not save scores to " + kScoresFile + "."));
	ASSERT_TRUE(std::filesystem::exists(kVotesFile));
	ASSERT_FALSE(std::filesystem::exists(kScoresFile));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_saveRoundWithVotes() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Save);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Votes saved to " + kVotesFile + "."));
	ASSERT_TRUE(catcher.contains("Scores saved to " + kScoresFile + "."));
	ASSERT_TRUE(std::filesystem::exists(kVotesFile));
	ASSERT_TRUE(std::filesystem::exists(kScoresFile));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_voteWithoutRound() {
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::VoteB);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No voting round to vote in", 2));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_voteWhenVotesRemain() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Yes);
	appendAction(KeyAction::VoteB);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_FALSE(catcher.contains("No voting round to vote in"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_voteWhenNoVotesRemain() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::VoteB);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No votes pending", 2));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_undoWithoutRound() {
	appendAction(KeyAction::Undo);
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No voting round to undo from"));
	ASSERT_TRUE(allActionsCompleted());
}
void endToEnd_undoWithoutVotes() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::Undo);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("No votes to undo"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_undoScoreVote() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::FullRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Undo);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_FALSE(catcher.contains("No voting round to undo from"));
	ASSERT_FALSE(catcher.contains("No votes to undo"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_undoRankVote() {
	createItemsFile(2);

	appendAction(KeyAction::NewRound);
	appendAction(KeyAction::RankedRound);
	appendAction(KeyAction::VoteA);
	appendAction(KeyAction::Undo);
	appendAction(KeyAction::Quit);
	appendAction(KeyAction::Yes);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_FALSE(catcher.contains("No voting round to undo from"));
	ASSERT_FALSE(catcher.contains("No votes to undo"));
	ASSERT_TRUE(allActionsCompleted());
	cleanUpFiles();
}
void endToEnd_combine() {
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

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("Saved combined scores to '" + kCombinedScoresFile + "'"));
	ASSERT_TRUE(catcher.contains("Saved combined scores to '" + kCombinedScoresFile + "'"));
	ASSERT_TRUE(std::filesystem::exists(kCombinedScoresFile));
	ASSERT_TRUE(allActionsCompleted());

	std::filesystem::remove(file_name_1);
	std::filesystem::remove(file_name_2);
	std::filesystem::remove(kCombinedScoresFile);
}
void endToEnd_combineWithInvalidFiles() {
	appendAction(KeyAction::Combine);
	appendLine("file1 file2");
	appendAction(KeyAction::Quit);

	OutputCatcher catcher;
	programLoop();
	catcher.stop();

	ASSERT_TRUE(catcher.contains("File file1 doesn't exist"));
	ASSERT_TRUE(catcher.contains("File file2 doesn't exist"));
	ASSERT_TRUE(catcher.contains("Failed to combine scores"));
	ASSERT_TRUE(allActionsCompleted());
}

} // namespace

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_quit);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_continueWithoutSavingWhenCreatingNewRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_continueWithoutSavingCancelWhenCreatingNewRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_continueWithoutSavingWhenLoadingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_continueWithoutSavingCancelWhenLoadingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_continueWithoutSavingWhenQuitting);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_continueWithoutSavingCancelWhenQuitting);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_loadEmptyRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_saveBeforeCreatingRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_saveRoundWithNoVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_saveRoundWithVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_voteWithoutRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_voteWhenVotesRemain);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_voteWhenNoVotesRemain);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_undoWithoutRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_undoWithoutVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_undoScoreVote);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_undoRankVote);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_combine);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_combineWithInvalidFiles);
	return 1;
}
