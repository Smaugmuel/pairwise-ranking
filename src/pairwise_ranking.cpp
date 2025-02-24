#include <algorithm>
#include <chrono>
#include <clocale>
#include <conio.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <random>
#include <regex>
#include <source_location>
#include <string>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <vector>

#include "constants.h"
#include "functions.h"
#include "helpers.h"
#include "print.h"
#include "score_helpers.h"

void programLoop() {
	std::optional<VotingRound> voting_round{};
	bool first_iteration = true;

	while (true) {
		print(activeMenuString(voting_round, first_iteration), false);
		first_iteration = false;

		char const ch = getKey();
		std::cout << ch << std::endl;
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
			if (!saveScores(calculateScores(voting_round.value().getItems(), voting_round.value().getVotes()), kResultFile)) {
				printError("Could not save scores to " + kResultFile + ".");
			}
			else {
				print("Scores saved to " + kResultFile + ".");
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
			combine();
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
