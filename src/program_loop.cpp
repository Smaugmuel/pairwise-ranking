#include "program_loop.h"

#include <filesystem>
//#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "calculate_scores.h"
#include "constants.h"
#include "functions.h"
#include "helpers.h"
#include "keyboard_input.h"
#include "menus.h"
#include "print.h"
#include "score_helpers.h"

enum class ProgramState {
	MainMenu,
	SelectItemsFile,
	SelectFormatAndCreateVotingRound,
	LoadVotingRound,
	SaveVotingRound,
	Voting,
	VotingEnded,
	CombineScores,
	ViewCombinedScores,
	SaveCombinedScores,
	Quit,
};

void mainMenu(ProgramState& state, bool show_menu) {
	if (show_menu) {
		system("cls");
		print(
			"-----------------------\n"
			"|  Pairwise ranking   |\n"
			"|---------------------|\n"
			"| [N]ew voting round  |\n"
			"| [L]oad voting round |\n"
			"|  [C]ombine scores   |\n"
			"|       [Q]uit        |\n"
			"-----------------------"
		);
	}

	auto const ch = getKey();
	switch (ch) {
	case 'n':
		state = ProgramState::SelectItemsFile;
		break;
	case 'l':
		state = ProgramState::LoadVotingRound;
		break;
	case 'c':
		state = ProgramState::CombineScores;
		break;
	case 'q':
		state = ProgramState::Quit;
		break;
	default:
		break;
	}
}
void selectItemsFile(ProgramState& state, Items& items) {
	print("Select file to load items from, or just \'c\' to cancel: ", false);

	auto const file_name = getLine();
	if (file_name == "c" || file_name == "C") {
		state = ProgramState::MainMenu;
		return;
	}
	if (!std::filesystem::exists(file_name)) {
		printError("File \'" + file_name + "\' does not exist");
		return;
	}
	auto loaded_items = loadFile(file_name);
	if (loaded_items.empty()) {
		printError("No items found in \'" + file_name + "\'");
		return;
	}

	state = ProgramState::SelectFormatAndCreateVotingRound;
	items = std::move(loaded_items);
}
void selectFormatAndCreateVotingRound(ProgramState& state, bool show_menu, Items const& items, std::optional<VotingRound>& voting_round) {
	if (show_menu) {
		system("cls");
		auto const padded_number = [](uint32_t n) -> std::string {
			auto const padding = 15ui32 - static_cast<uint32_t>(numberOfDigits(n));
			auto const padding_left = padding / 2;
			auto const padding_right = padding - padding_left;
			return std::string(padding_left, ' ') + std::to_string(n) + std::string(padding_right, ' ');
			};

		auto const full_votes = static_cast<uint32_t>(items.size() * (items.size() - 1) / 2);
		auto const reduced_votes = static_cast<uint32_t>(items.size() * (3 + (items.size() % 2)) / 2);
		auto const ranking_votes = static_cast<uint32_t>(items.size() * std::log2(items.size()));
		print(
			"---------------------------------------------------\n"
			"|     Select voting format      | Estimated votes |\n"
			"|-------------------------------|-----------------|\n"
			"|   [F]ull score-based voting   | " + padded_number(full_votes) + " |\n"
			"|  [R]educed score-based voting | " + padded_number(reduced_votes) + " |\n"
			"| [I]nsertion rank-based voting | " + padded_number(ranking_votes) + " |\n"
			"|            [H]elp             |------------------\n"
			"|           [C]ancel            |\n"
			"---------------------------------"
		);
	}

	auto const ch = getKey();
	switch (ch) {
	case 'f':
	case 'r':
	case 'i':
		voting_round = newRound(characterToVotingFormat(ch));
		if (voting_round.has_value()) {
			state = ProgramState::Voting;
		}
		break;
	case 'c':
		state = ProgramState::MainMenu;
		break;
	case 'h':
		print(
			"Score-based voting will collect the number of wins and losses each item receives, "
			"and calculate the order based on them\n"
			"Rank-based voting will sort the items in relation to each other, and item matchups "
			"will be affected by the previous votes"
		);
		break;
	default:
		break;
	}
}
void loadVotingRound(ProgramState& state, std::optional<VotingRound>& voting_round) {
	print("Select file name to load voting round from, or just \'c\' to cancel: ", false);

	auto const file_name = getLine();
	if (file_name.empty()) {
		return;
	}
	if (file_name == "c" || file_name == "C") {
		state = ProgramState::MainMenu;
		return;
	}
	if (!std::filesystem::exists(file_name)) {
		printError("File \'" + file_name + "\' does not exist");
		return;
	}
	auto loaded_lines = loadFile(file_name);
	if (loaded_lines.empty()) {
		printError("No lines found in \'" + file_name + "\'");
		return;
	}

	voting_round = VotingRound::create(loaded_lines);
	if (!voting_round.has_value()) {
		printError("Failed to create voting round from \'" + file_name + "\'");
		return;
	}
	print("Voting round loaded from \'" + file_name + "\'");
	state = ProgramState::Voting;
}
void saveVotingRound(ProgramState& state, VotingRound& voting_round) {
	print("Select file name to save voting round to, or just \'c\' to cancel: ", false);

	auto const file_name = getLine();
	if (file_name.empty()) {
		return;
	}
	if (file_name == "c" || file_name == "C") {
		state = ProgramState::Voting;
		return;
	}
	if (!voting_round.save(file_name)) {
		printError("Failed to save voting round to \'" + file_name + "\'");
		return;
	}
	print("Voting round saved to \'" + file_name + "\'");
	state = ProgramState::Voting;
}
void voting(ProgramState& state, bool show_menu, VotingRound& voting_round) {
	if (show_menu) {
		system("cls");
		print(
			"-------------------------\n"
			"|     Voting round      |\n"
			"|-----------------------|\n"
			"|    Vote option [A]    |\n"
			"|    Vote option [B]    |\n"
			"|      [U]ndo vote      |\n"
			"| [P]rint current score |\n"
			"|     [S]ave votes      |\n"
			"|  [Q]uit to main menu  |\n"
			"-------------------------"
		);
	}

	auto const voting_line{ voting_round.currentVotingLine() };
	if (!voting_line.has_value()) {
		state = ProgramState::VotingEnded;
		return;
	}

	print(voting_line.value());

	auto const ch = getKey();
	switch (ch) {
	case 'a':
		voting_round.vote(Option::A);
		break;
	case 'b':
		voting_round.vote(Option::B);
		break;
	case 'u':
		voting_round.undoVote();
		break;
	case 'p':
		print(createScoreTable(sortScores(calculateScores(voting_round.items(), voting_round.votes()))));
		break;
	case 's':
		state = ProgramState::SaveVotingRound;
		break;
	case 'q':
		state = ProgramState::MainMenu;
		break;
	default:
		break;
	}
}
void votingEnded(ProgramState& state, bool show_menu, VotingRound& voting_round) {
	if (show_menu) {
		system("cls");
		print(
			"-------------------------\n"
			"|    Voting completed   |\n"
			"|-----------------------|\n"
			"|      [U]ndo vote      |\n"
			"| [P]rint current score |\n"
			"|  [S]ave current score |\n"
			"|  [Q]uit to main menu  |\n"
			"-------------------------"
		);
	}

	auto const ch = getKey();
	switch (ch) {
	case 'u':
		voting_round.undoVote();
		break;
	case 'p':
		print(createScoreTable(sortScores(calculateScores(voting_round.items(), voting_round.votes()))));
		break;
	case 's':
		state = ProgramState::SaveVotingRound;
		break;
	case 'q':
		state = ProgramState::MainMenu;
		break;
	default:
		break;
	}
}
void combineScores(ProgramState& state, std::optional<Scores>& combined_scores) {
	print("Select two or more score files to combine, or just \'c\' to cancel: ", false);

	auto const input = getLine();
	if (input == "c" || input == "C") {
		state = ProgramState::MainMenu;
		return;
	}

	auto file_names = parseWords(input);
	if (file_names.size() < 2) {
		printError("Too few files. No scores combined");
		return;
	}
	bool all_files_exist = true;
	for (auto const& name : file_names) {
		if (!std::filesystem::exists(name)) {
			printError("File \'" + name + "\' doesn't exist");
			all_files_exist = false;
		}
	}
	if (!all_files_exist) {
		printError("No scores combined");
		return;
	}

	// Load files' contents
	std::vector<Scores> scores_sets{};
	bool all_scores_valid = true;
	for (auto const& file_name : file_names) {
		print("Reading " + file_name);
		auto const lines = loadFile(file_name);
		auto const scores = parseScores(lines);
		if (scores.size() != lines.size()) {
			all_scores_valid = false;
		}
		scores_sets.emplace_back(scores);
	}
	if (!all_scores_valid) {
		return;
	}

	combined_scores = combineScores(scores_sets);
	state = ProgramState::ViewCombinedScores;
}
void viewCombinedScores(ProgramState& state, bool show_menu, Scores const& combined_scores) {
	if (show_menu) {
		system("cls");
		print(
			"---------------------------\n"
			"|     Scores combined     |\n"
			"|-------------------------|\n"
			"| [P]rint combined scores |\n"
			"|  [S]ave combined scores |\n"
			"|   [Q]uit to main menu   |\n"
			"---------------------------"
		);
	}

	auto const ch = getKey();
	switch (ch) {
	case 'p':
		print(createScoreTable(sortScores(combined_scores)));
		break;
	case 's':
		state = ProgramState::SaveCombinedScores;
		break;
	case 'q':
		state = ProgramState::MainMenu;
		break;
	default:
		break;
	}
}
void saveCombinedScores(ProgramState& state, bool show_menu, Scores const& combined_scores) {
	print("Select file name to save combined scores to, or just \'c\' to cancel: ", false);

	auto const input = getLine();
	if (input.empty()) {
		return;
	}
	if (input == "c" || input == "C") {
		state = ProgramState::ViewCombinedScores;
		return;
	}

	if (!saveFile(input, generateScoreFileData(sortScores(combined_scores)))) {
		printError("Failed to save combined scores to \'" + input + "\'");
		return;
	}
	print("Saved combined scores to \'" + input + "\'");
	state = ProgramState::MainMenu;
}

void programLoop() {
	std::optional<VotingRound> voting_round{};
	std::optional<Scores> combined_scores{};
	ProgramState state{ ProgramState::MainMenu };
	bool show_menu{ true };
	bool program_running{ true };
	Items items{};

	while (program_running) {
		switch (state) {
		case ProgramState::MainMenu:
			mainMenu(state, show_menu);
			show_menu = (state != ProgramState::MainMenu);
			break;
		case ProgramState::SelectItemsFile:
			selectItemsFile(state, items);
			show_menu = (state != ProgramState::SelectItemsFile);
			break;
		case ProgramState::SelectFormatAndCreateVotingRound:
			selectFormatAndCreateVotingRound(state, show_menu, items, voting_round);
			show_menu = (state != ProgramState::SelectFormatAndCreateVotingRound);
			break;
		case ProgramState::LoadVotingRound:
			loadVotingRound(state, voting_round);
			show_menu = (state != ProgramState::LoadVotingRound);
			break;
		case ProgramState::SaveVotingRound:
			saveVotingRound(state, voting_round.value());
			show_menu = (state != ProgramState::SaveVotingRound);
			break;
		case ProgramState::Voting:
			voting(state, show_menu, voting_round.value());
			show_menu = (state != ProgramState::Voting);
			break;
		case ProgramState::VotingEnded:
			votingEnded(state, show_menu, voting_round.value());
			show_menu = (state != ProgramState::VotingEnded);
			break;
		case ProgramState::CombineScores:
			combineScores(state, combined_scores);
			show_menu = (state != ProgramState::CombineScores);
			break;
		case ProgramState::ViewCombinedScores:
			viewCombinedScores(state, show_menu, combined_scores.value());
			show_menu = (state != ProgramState::ViewCombinedScores);
			break;
		case ProgramState::SaveCombinedScores:
			saveCombinedScores(state, show_menu, combined_scores.value());
			show_menu = (state != ProgramState::ViewCombinedScores);
			break;
		case ProgramState::Quit:
			program_running = false;
			break;
		default:
			break;
		}
	}
}
