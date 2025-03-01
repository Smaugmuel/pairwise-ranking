#include "functions.h"

#include <conio.h>			// _getch()
#include <filesystem>		// filesystem::current_path(), filesystem::exists()
#include <fstream>			// ifstream
#include <iostream>			// getline(), cin
#include <unordered_set>	// unordered_set
#include <sstream>			// stringstream

#include "calculate_scores.h"
#include "constants.h"
#include "helpers.h"
#include "menus.h"
#include "print.h"
#include "score_helpers.h"
#include "voting_format.h"

namespace
{

auto fixSwedish(std::string const& str) -> std::string {
	std::string new_str = str;
	for (size_t i = 0; i < new_str.size(); i++) {
		if (static_cast<int>(new_str[i]) != -61) {
			continue;
		}
		switch (static_cast<int>(new_str[i + 1])) {
		case -91:	// -61 + this : å
			new_str.replace(i, 2, 1, 'å');
			break;
		case -92:	// -61 + this : ä
			new_str.replace(i, 2, 1, 'ä');
			break;
		case -74:	// -61 + this : ö
			new_str.replace(i, 2, 1, 'ö');
			break;
		case -123:	// -61 + this : Å
			new_str.replace(i, 2, 1, 'Å');
			break;
		case -124:	// -61 + this : Ä
			new_str.replace(i, 2, 1, 'Ä');
			break;
		case -106:	// -61 + this : Ö
			new_str.replace(i, 2, 1, 'Ö');
			break;
		}
	}
	return new_str;
}
auto parseItems(std::vector<std::string> const& lines) -> Items {
	Items items{};
	items.reserve(lines.size());
	for (auto const& line : lines) {
		items.emplace_back(fixSwedish(line));
	}
	return items;
}
auto getLine() -> std::string {
	std::string input{};
	std::getline(std::cin, input);
	return input;
}
auto verifyFilesExist(std::vector<std::string> const& file_names) -> bool {
	bool all_files_exist = true;
	for (auto const& name : file_names) {
		if (!std::filesystem::exists(name)) {
			printError("File " + name + " doesn't exist");
			all_files_exist = false;
		}
	}
	return all_files_exist;
}
auto getMultipleFileNames() -> std::vector<std::string> {
	print("Specify two or more files, e.g. \"file_1.txt file_2.txt\", without the quotation marks (\"): ", false);

	std::vector<std::string> file_names = parseWords(getLine());
	if (!verifyFilesExist(file_names)) {
		printError("One or more files don't exist");
		return {};
	}

	if (file_names.size() < 2) {
		printError("Two or more files are required to combine scores");
		return {};
	}
	return file_names;
}

} // namespace

/* -------------- Command line inputs -------------- */
auto getKey() -> char {
	return _getch();
}
auto getConfirmation() -> bool {
	while (true) {
		char const ch = getKey();
		if (ch == 'n') {
			return false;
		}
		else if (ch == 'y') {
			return true;
		}
	}
	return false;
}
auto continueWithoutSaving(std::optional<VotingRound> const& voting_round, std::string const& str) -> bool {
	bool response = true;
	if (voting_round.has_value() && !voting_round.value().isSaved()) {
		print("You have unsaved data. Do you still wish to " + str + "? (Y / N) : ", false);
		if (!getConfirmation()) {
			response = false;
		}
		print("");
	}
	return response;
}

/* -------------- Menu alternatives -------------- */
void newRound(std::optional<VotingRound>& voting_round) {
	std::vector<std::string> const lines = loadFile(kItemsFile);
	if (lines.size() < 2) {
		return;
	}

	// Print voting formats
	print(newRoundFormatString(static_cast<uint32_t>(lines.size())));

	// Choose voting format
	char ch{};
	while (true) {
		ch = getKey();
		if (ch == 'f' || ch == 'r' || ch == 'c') {
			break;
		}
	}
	print(std::string() + ch);
	VotingFormat const format{ characterToVotingFormat(ch) };
	if (format == VotingFormat::Invalid) {
		return;
	}

	// Create voting round
	Items items = parseItems(lines);
	voting_round = VotingRound::create(items, format);
	if (!voting_round.has_value()) {
		printError("Could not generate voting round");
		return;
	}
	voting_round.value().shuffle();
}
void loadRound(std::optional<VotingRound>& voting_round, std::vector<std::string> const& lines) {
	std::optional<VotingRound> loaded_voting_round = VotingRound::create(lines);
	if (!loaded_voting_round.has_value()) {
		printError("Could not load voting round");
		return;
	}
	voting_round = loaded_voting_round;
}
void printScores(std::optional<VotingRound> const& voting_round) {
	if (!voting_round.has_value()) {
		printError("No voting round to print");
		return;
	}
	print(createScoreTable(calculateScores(voting_round.value().items(), voting_round.value().votes())), false);
}
void combine() {
	// Input names of two or more files to combine
	//		Verify files exist
	//		Allow cancelling
	// Load lines for each file name
	// Parse scores from lines
	//		Identify the files which contain scores
	//			For those that don't, print an error
	// If there are two or more valid sets of scores, proceed
	// Combine scores
	// Return combined scores

	// Get valid file names
	std::vector<std::string> file_names = getMultipleFileNames();
	if (file_names.size() < 2) {
		printError("Too few files. No scores combined");
		return;
	}

	// Load files' contents
	std::vector<Scores> scores_sets{};

	for (auto const& file_name : file_names) {
		print("Reading " + file_name);
		auto const lines = loadFile(file_name);
		scores_sets.emplace_back(parseScores(lines));
	}

	Scores const combined_scores = combineScores(scores_sets);

	if (!saveFile(kCombinedScoresFile, generateScoreFileData(sortScores(combined_scores)))) {
		printError("Couldn't save file \'" + kCombinedScoresFile + "\'");
	}
	print("Saved combined scores to \'" + kCombinedScoresFile + "\'");
}
