#include "functions.h"

#include <conio.h>			// _getch()
#include <filesystem>		// filesystem::current_path(), filesystem::exists()
#include <fstream>			// ifstream
#include <iostream>			// getline(), cin
#include <unordered_set>	// unordered_set
#include <sstream>			// stringstream

#include "constants.h"
#include "helpers.h"
#include "print.h"
#include "score_helpers.h"

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

/* -------------- Menu strings -------------- */
auto getActiveMenuString(std::optional<VotingRound> const& voting_round, bool const show_intro_screen) -> std::string {
	std::string menu{};
	if (show_intro_screen) {
		menu += "Welcome! ";
	}

	if (!voting_round.has_value()) {
		menu += "Press H for help: ";
		return menu;
	}

	if (!voting_round.value().hasRemainingVotes()) {
		menu += "Poll finished. H: help. Q: quit. Your choice: ";
		return menu;
	}

	auto const current_voting_line = voting_round.value().currentVotingLine();
	if (current_voting_line.has_value()) {
		menu += current_voting_line.value();
	}
	return menu;
}
auto getHelpString() -> std::string {
	return
		"(Key to press is in brackets)\n"
		"Show this [H]elp menu\n"
		"[Q]uit program\n"
		"Start a [N]ew poll\n"
		"[L]oad poll\n"
		"[S]ave votes and scores\n"
		"Vote for option [A]\n"
		"Vote for option [B]\n"
		"[U]ndo last vote\n"
		"[P]rint scores\n"
		"[C]ombine previously saved scores\n";
}

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
	if (voting_round.has_value() && !voting_round.value().is_saved) {
		print("You have unsaved data. Do you still wish to " + str + "? (Y / N) : ", false);
		if (!getConfirmation()) {
			response = false;
		}
		print("");
	}
	return response;
}

/* -------------- Menu alternatives -------------- */
auto vote(std::optional<VotingRound>& voting_round, Option option) -> std::string {
	if (!voting_round.has_value()) {
		return "No poll to vote in";
	}
	if (!voting_round.value().vote(option)) {
		return "No ongoing poll with pending votes";
	}
	return {};
}
auto undo(std::optional<VotingRound>& voting_round) -> std::string {
	if (!voting_round.has_value()) {
		return "No poll to undo from";
	}
	voting_round.value().undoVote();
	return {};
}
void newRound(std::optional<VotingRound>& voting_round) {
	std::vector<std::string> const lines = loadFile(kItemsFile);
	if (lines.size() < 2) {
		return;
	}

	auto const full_voting_size = sumOfFirstIntegers(lines.size() - 1);
	print("Do you want reduced voting (y/n)? Full voting = " + std::to_string(full_voting_size) + " votes. Reduced voting = " + std::to_string(full_voting_size - lines.size() * pruningAmount(lines.size())) + " votes. Your choice: ", false);
	auto const reduce_voting = getConfirmation();
	print(reduce_voting ? "y" : "n");

	Items items = parseItems(lines);
	voting_round = VotingRound::create(items, reduce_voting);
	voting_round.value().shuffle();
	if (!voting_round.value().verify()) {
		printError("Could not generate poll");
		voting_round.reset();
		return;
	}
}
void loadRound(std::optional<VotingRound>& voting_round, std::vector<std::string> const& lines) {
	std::optional<VotingRound> temp = VotingRound::create(lines);
	if (!temp.has_value() || !temp.value().verify()) {
		printError("Could not load poll");
		return;
	}
	voting_round = temp;
}
void printScores(std::optional<VotingRound> const& voting_round) {
	if (!voting_round.has_value()) {
		printError("No poll to print");
		return;
	}
	print(createScoreTable(calculateScores(voting_round.value().getItems(), voting_round.value().getVotes())), false);
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

	if (!saveFile(kCombinedResultFile, generateScoreFileData(sortScores(combined_scores)))) {
		printError("Couldn't save file \'" + kCombinedResultFile + "\'");
	}
	print("Saved combined results to \'" + kCombinedResultFile + "\'");
}
