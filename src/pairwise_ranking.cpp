#include <clocale>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "calculate_scores.h"
#include "constants.h"
#include "functions.h"
#include "helpers.h"
#include "menus.h"
#include "print.h"
#include "score_helpers.h"

auto getLine() -> std::string {
	std::string input{};
	std::getline(std::cin, input);
	return input;
}

void programLoop() {
	std::optional<VotingRound> voting_round{};
	bool first_iteration = true;

	while (true) {
		print(activeMenuString(voting_round, first_iteration), false);
		first_iteration = false;

		char const ch = getKey();
		print(std::to_string(ch));
		if (ch == 'q') {
			if (continueWithoutSaving(voting_round, "quit")) {
				break;
			}
		}
		else if (ch == 'h') {
			print(helpString());
		}
		else if (ch == 'n') {
			if (!continueWithoutSaving(voting_round, "create new voting round")) {
				break;
			}
			newRound(voting_round);
		}
		else if (ch == 'l') {
			if (!continueWithoutSaving(voting_round, "load a voting round")) {
				continue;
			}
			std::vector<std::string> const lines = loadFile(kVotesFile);
			if (lines.empty()) {
				printError("No lines loaded");
				continue;
			}
			loadRound(voting_round, lines);
		}
		else if (ch == 's') {
			if (!voting_round.has_value()) {
				printError("No voting round to save from");
				continue;
			}
			if (!voting_round.value().save(kVotesFile)) {
				printError("Could not save votes to " + kVotesFile + ".");
			}
			else {
				print("Votes saved to " + kVotesFile + ".");
			}
			if (!saveScores(calculateScores(voting_round.value().items(), voting_round.value().votes()), kScoresFile)) {
				printError("Could not save scores to " + kScoresFile + ".");
			}
			else {
				print("Scores saved to " + kScoresFile + ".");
			}
		}
		else if (ch == 'p') {
			printScores(voting_round);
		}
		else if (ch == 'a') {
			if (!voting_round.has_value()) {
				printError("No voting round to vote in");
				continue;
			}
			if (!voting_round.value().vote(Option::A)) {
				printError("No votes pending");
			}
		}
		else if (ch == 'b') {
			if (!voting_round.has_value()) {
				printError("No voting round to vote in");
				continue;
			}
			if (!voting_round.value().vote(Option::B)) {
				printError("No votes pending");
			}
		}
		else if (ch == 'u') {
			if (!voting_round.has_value()) {
				printError("No voting round to undo from");
				continue;
			}
			voting_round.value().undoVote();
		}
		else if (ch == 'c') {
			print("Specify two or more files, e.g. \"file_1.txt file_2.txt\", without the quotation marks (\"): ", false);
			auto const input_line = getLine();
			if (input_line.empty()) {
				continue;
			}
			if (!combine(input_line, kCombinedScoresFile)) {
				printError("Failed to combine scores");
			}
		}
		else {
			printError("Invalid input");
		}
	}
}

int main() {
	setlocale(LC_ALL, "sv-SE");

	programLoop();

	return 0;
}
