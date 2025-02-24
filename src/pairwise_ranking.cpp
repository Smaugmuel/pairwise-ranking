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
		printActiveMenu(voting_round, first_iteration);
		first_iteration = false;

		char const ch = getKey();
		std::cout << ch << std::endl;
		if (ch == 'q') {
			if (continueWithoutSaving(voting_round, "quit")) {
				break;
			}
		}
		else if (ch == 'h') {
			printHelp();
		}
		else if (ch == 'n') {
			if (!continueWithoutSaving(voting_round, "create new poll")) {
				break;
			}
			newRound(voting_round);
		}
		else if (ch == 'l') {
			if (!continueWithoutSaving(voting_round, "load a poll")) {
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
			if (!saveScores(voting_round.value().calculateScores(), kResultFile)) {
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
			auto const result = vote(voting_round, Option::A);
			if (!result.empty()) {
				printError(result);
			}
		}
		else if (ch == 'b') {
			auto const result = vote(voting_round, Option::B);
			if (!result.empty()) {
				printError(result);
			}
		}
		else if (ch == 'u') {
			auto const result = undo(voting_round);
			if (!result.empty()) {
				printError(result);
			}
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
