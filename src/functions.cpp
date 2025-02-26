#include "functions.h"

#include <conio.h>			// _getch()
#include <fstream>			// ifstream
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
		if (ch == 'f' || ch == 'r' || ch == 'c' || ch == 'i') {
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

	switch (voting_round.value().format())
	{
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		print(createScoreTable(calculateScores(voting_round.value().items(), voting_round.value().votes())), false);
		break;
	case VotingFormat::Ranked: {
		auto const& items = voting_round.value().items();
		std::string str{};
		for (size_t i = 0; i < items.size(); i++) {
			str += items[i];
			if (i + 1 == voting_round.value().numberOfSortedItems()) {
				str += " <-- sorted until here";
			}
			str += '\n';
		}
		print(str, false);
		break;
	}
	case VotingFormat::Invalid:
	default:
		break;
	}
}
