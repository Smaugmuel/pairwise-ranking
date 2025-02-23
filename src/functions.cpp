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

/* -------------- Type definitions -------------- */
auto operator==(Score const& a, Score const& b) -> bool {
	return a.item == b.item && a.wins == b.wins && a.losses == b.losses;
}
auto operator==(Vote const& a, Vote const& b) -> bool {
	return a.index_pair == b.index_pair && a.winner == b.winner;
}

/* -------------- Voting round score calculation -------------- */
void incrementWinner(Scores& scores, Item const& item) {
	auto it = std::find_if(scores.begin(), scores.end(), [&](Score const& score) {
		return score.item == item;
		});
	if (it == scores.end()) {
		Score score{};
		score.item = item;
		score.wins = 1;
		scores.emplace_back(score);
	}
	else {
		it->wins++;
	}
}
void incrementLoser(Scores& scores, Item const& item) {
	auto it = std::find_if(scores.begin(), scores.end(), [&](Score const& score) {
		return score.item == item;
		});
	if (it == scores.end()) {
		Score score{};
		score.item = item;
		score.losses = 1;
		scores.emplace_back(score);
	}
	else {
		it->losses++;
	}
}
auto calculateScores(VotingRound const& voting_round) -> Scores {
	Scores scores{};
	for (Vote const& vote : voting_round.votes) {
		if (vote.winner == Option::A) {
			incrementWinner(scores, voting_round.items[vote.index_pair.first]);
			incrementLoser(scores, voting_round.items[vote.index_pair.second]);
		}
		else {
			incrementWinner(scores, voting_round.items[vote.index_pair.second]);
			incrementLoser(scores, voting_round.items[vote.index_pair.first]);
		}
	}
	return scores;
}

/* -------------- Voting round miscellaneous -------------- */
auto hasRemainingVotes(VotingRound const& voting_round) -> bool {
	return voting_round.votes.size() < voting_round.index_pairs.size();
}
auto convertVotingRoundToText(VotingRound const& voting_round) -> std::vector<std::string> {
	std::vector<std::string> lines{};

	// Save original item order, to make seeded item shuffling deterministic
	if (voting_round.original_items_order.empty() ||
		voting_round.items.empty()) {
		printError("Failed to generate voting file data: No items");
		return lines;
	}

	// Items
	for (Item const& item : voting_round.original_items_order) {
		lines.emplace_back(item);
	}
	lines.emplace_back("");

	// Seed
	lines.emplace_back(std::to_string(voting_round.seed));

	// Voting format
	if (voting_round.reduced_voting) {
		lines.emplace_back("reduced");
	}
	else {
		lines.emplace_back("full");
	}

	// Save votes
	for (Vote const& vote : voting_round.votes) {
		lines.emplace_back(std::to_string(vote.index_pair.first) + " " + std::to_string(vote.index_pair.second) + " " + std::to_string(to_underlying(vote.winner)));
	}
	return lines;
}

/* -------------- Printing votable options -------------- */
auto currentOptionItems(VotingRound const& voting_round) -> std::pair<std::string, std::string> {
	auto const index_pair = voting_round.index_pairs[voting_round.votes.size()];
	return { voting_round.items[index_pair.first], voting_round.items[index_pair.second] };
}
auto findMaxLength(Items const& items) -> size_t {
	if (items.empty()) {
		return 0;
	}
	return std::max_element(items.begin(), items.end(),
		[](Item const& a, Item const& b) {
			return a.size() < b.size();
		}
	)->size();
}
auto numberOfDigits(size_t n) -> size_t {
	if (n == 0) {
		return 1;
	}
	return static_cast<size_t>(std::log10(n)) + 1;
}
auto counterString(size_t counter, size_t total) -> std::string {
	size_t const total_length = numberOfDigits(total);
	size_t const counter_length = numberOfDigits(counter);

	return
		"(" + std::string(total_length - counter_length, ' ') + std::to_string(counter) +
		"/" + std::to_string(total) + ")";
}
auto currentVotingLine(VotingRound const& voting_round) -> std::optional<std::string> {
	if (!hasRemainingVotes(voting_round)) {
		printError("Voting finished. No active votes to print");
		return std::nullopt;
	}
	Items     const& items = voting_round.items;
	size_t    const  counter = voting_round.votes.size();
	IndexPair const& indices = voting_round.index_pairs[counter];

	size_t const max_length = findMaxLength(items);
	size_t const length_a = items[indices.first].size();
	size_t const length_b = items[indices.second].size();

	size_t const padding_length_a = max_length - length_a;
	size_t const padding_length_b = max_length - length_b;
	return counterString(counter + 1, voting_round.index_pairs.size()) + " "
		"H: help. "
		"A: \'" + items[indices.first] + "\'." + std::string(padding_length_a, ' ') + " "
		"B: \'" + items[indices.second] + "\'." + std::string(padding_length_b, ' ') + " Your choice: ";
}

/* -------------- Printing active screen -------------- */
auto getActiveMenuString(std::optional<VotingRound> const& voting_round, bool const show_intro_screen) -> std::string {
	std::string menu{};
	if (show_intro_screen) {
		menu += "Welcome! ";
	}

	if (!voting_round.has_value()) {
		menu += "Press H for help: ";
		return menu;
	}

	if (!hasRemainingVotes(voting_round.value())) {
		menu += "Poll finished. H: help. Q: quit. Your choice: ";
		return menu;
	}

	auto const current_voting_line = currentVotingLine(voting_round.value());
	if (current_voting_line.has_value()) {
		menu += current_voting_line.value();
	}
	return menu;
}
void printActiveMenu(std::optional<VotingRound> const& voting_round, bool const show_intro_screen) {
	print(getActiveMenuString(voting_round, show_intro_screen), false);
}

/* -------------- Printing miscellaneous -------------- */
auto getHelpString() -> std::string {
	return
		"Q: quit program. "
		"N: start new poll. "
		"L: load started poll. "
		"S: save votes/scores to file.\n"
		"A: vote for left option. "
		"B: vote for right option. "
		"U: undo last vote.\n"
		"P: print scores. "
		"C: combine scores. ";
}
void printHelp() {
	print(getHelpString());
}

/* -------------- Command line inputs -------------- */
auto getKey() -> char {
	return _getch();
}
auto getLine() -> std::string {
	std::string input{};
	std::getline(std::cin, input);
	return input;
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

/* -------------- Localization -------------- */
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

/* -------------- String-to-object parsing -------------- */
auto readNextWord(std::stringstream& stream) -> std::string {
	if (stream.rdbuf()->in_avail() == 0) {
		printError("No word to read from stream");
		return {};
	}
	std::string str{};
	stream >> str;
	return str;
}

auto parseNextNumber(std::stringstream& stream) -> std::optional<uint32_t> {
	std::string str = readNextWord(stream);
	if (str.empty()) {
		printError("Unable to parse word");
		return {};
	}
	return parseNumber(str);
}

auto parseScore(std::string const& str) -> Score {
	std::stringstream stream(str);

	std::optional<uint32_t> const wins = parseNextNumber(stream);
	if (!wins.has_value()) {
		printError("Unable to parse wins number");
		return {};
	}

	std::optional<uint32_t> const losses = parseNextNumber(stream);
	if (!losses.has_value()) {
		printError("Unable to parse losses number");
		return {};
	}

	// Character length until item name, including the spaces between values
	size_t const name_offset = numberOfDigits(wins.value()) + 1 + numberOfDigits(losses.value()) + 1;

	if (name_offset >= str.size()) {
		printError("Invalid score item format: no item name");
		return {};
	}
	std::string const name = str.substr(name_offset);
	if (name.empty()) {
		printError("Invalid score item format: empty item name");
		return {};
	}

	return Score{ name, wins.value(), losses.value() };
}
auto parseItems(std::vector<std::string> const& lines) -> Items {
	Items items{};
	items.reserve(lines.size());
	for (auto const& line : lines) {
		items.emplace_back(fixSwedish(line));
	}
	return items;
}
auto parseScores(std::vector<std::string> const& lines) -> Scores {
	Scores scores{};
	scores.reserve(lines.size());
	for (auto const& line : lines) {
		Score score = parseScore(line);
		if (score == Score{}) {
			printError("Invalid score format: " + line);
			continue;
		}
		scores.emplace_back(score);
	}
	return scores;
}

/* -------------- Object-to-string conversion -------------- */
auto generateScoreFileData(Scores const& scores) -> std::vector<std::string> {
	std::vector<std::string> lines{};
	for (auto const& score : scores) {
		lines.emplace_back(std::to_string(score.wins) + " " + std::to_string(score.losses) + " " + score.item);
	}
	return lines;
}

/* -------------- File system management -------------- */
auto loadFile(std::string const& file_name) -> std::vector<std::string> {
	std::ifstream file(file_name);
	if (!file.is_open()) {
		printError("Could not open file \'" + file_name + "\' in " + std::filesystem::current_path().string());
		return {};
	}

	std::vector<std::string> lines{};
	std::string line;
	while (std::getline(file, line)) {
		lines.emplace_back(line);
	}
	file.close();
	return lines;
}
auto saveFile(std::string const& file_name, std::vector<std::string> const& lines) -> bool {
	if (lines.empty()) {
		printError("No lines to save");
		return false;
	}
	std::ofstream file(file_name);
	if (!file.is_open()) {
		printError("Could not create file " + file_name);
		return false;
	}
	for (auto const& line : lines) {
		file << line << '\n';
	}
	file.close();
	return true;
}

/* -------------- Verifications -------------- */
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

/* -------------- Print scores -------------- */
auto sortScores(Scores const& scores) -> Scores {
	Scores sorted_scores = scores;
	std::sort(sorted_scores.begin(), sorted_scores.end(),
		[](Score const& a, Score const& b) {
			uint32_t const total_votes_a = a.wins + a.losses;
			uint32_t const total_votes_b = b.wins + b.losses;
			int32_t const net_wins_a = static_cast<int32_t>(a.wins) - static_cast<int32_t>(a.losses);
			int32_t const net_wins_b = static_cast<int32_t>(b.wins) - static_cast<int32_t>(b.losses);


			// Maybe change algorithm to win ratio-based instead, since it's easier to understand.
			// float win_ratio_a = static_cast<float>(a.wins) / static_cast<float>(a.losses);
			// float win_ratio_b = static_cast<float>(b.wins) / static_cast<float>(b.losses);
			// 
			// // First, simply compare win ratios. Greater ratio is better.
			// if (win_ratio_a > win_ratio_b) {
			// 	return true;
			// }
			// if (win_ratio_a < win_ratio_b) {
			// 	return false;
			// }
			// 
			// // If win ratio is the same, compare total votes to determine confidence.
			// // For a positive win ratio, more votes is better.
			// // For a negative win ratio, fewer votes is better.
			// if (win_ratio_a >= 1.0f) {
			// 	if (total_votes_a > total_votes_b) {
			// 		return true;
			// 	}
			// 	if (total_votes_a < total_votes_b) {
			// 		return false;
			// 	}
			// }
			// if (win_ratio_a < 1.0f) {
			// 	if (total_votes_a > total_votes_b) {
			// 		return true;
			// 	}
			// 	if (total_votes_a < total_votes_b) {
			// 		return false;
			// 	}
			// }

			if (net_wins_a > net_wins_b) {
				return true;
			}
			if (net_wins_a < net_wins_b) {
				return false;
			}

			// If score is positive, 
			if (net_wins_a > 0) {
				if (total_votes_a < total_votes_b) {
					return true;
				}
				if (total_votes_a > total_votes_b) {
					return false;
				}
			}
			if (net_wins_a < 0) {
				if (total_votes_a > total_votes_b) {
					return true;
				}
				if (total_votes_a < total_votes_b) {
					return false;
				}
			}
			return a.item < b.item; });
	return sorted_scores;
}
auto findMaxLengthItem(Scores const& scores) -> size_t {
	if (scores.empty()) {
		return 0;
	}
	return std::max_element(scores.begin(), scores.end(),
		[](Score const& a, Score const& b) {
			return a.item.size() < b.item.size();
		}
	)->item.size();
}
auto findMaxLengthWins(Scores const& scores) -> size_t {
	if (scores.empty()) {
		return 0;
	}
	return numberOfDigits(std::max_element(scores.begin(), scores.end(),
		[](Score const& a, Score const& b) {
			return numberOfDigits(a.wins) < numberOfDigits(b.wins);
		}
	)->wins);
}
auto findMaxLengthLosses(Scores const& scores) -> size_t {
	if (scores.empty()) {
		return 0;
	}
	return numberOfDigits(std::max_element(scores.begin(), scores.end(),
		[](Score const& a, Score const& b) {
			return numberOfDigits(a.losses) < numberOfDigits(b.losses);
		}
	)->losses);
}
auto createScoreTable(Scores const& scores) -> std::string {
	Scores const sorted_scores = sortScores(scores);

	std::string const item_header = "Item";
	std::string const wins_header = "Wins";
	std::string const losses_header = "Losses";

	size_t const item_min_length = item_header.size();
	size_t const wins_min_length = wins_header.size();
	size_t const losses_min_length = losses_header.size();

	size_t const item_max_length = std::max(findMaxLengthItem(sorted_scores), item_min_length);
	size_t const wins_max_length = std::max(findMaxLengthWins(sorted_scores), wins_min_length);
	size_t const losses_max_length = std::max(findMaxLengthLosses(sorted_scores), losses_min_length);

	size_t const total_length = 2 + item_max_length + 3 + wins_max_length + 3 + losses_max_length + 2;

	std::string score_string{};

	score_string += std::string(total_length, '-') + '\n';
	score_string += "| " +
		std::string(item_max_length - item_min_length, ' ') + item_header + " | " +
		std::string(wins_max_length - wins_min_length, ' ') + wins_header + " | " +
		std::string(losses_max_length - losses_min_length, ' ') + losses_header + " |" + '\n';

	score_string += "|-" +
		std::string(item_max_length, '-') + "-|-" +
		std::string(wins_max_length, '-') + "-|-" +
		std::string(losses_max_length, '-') + "-|" + '\n';

	for (Score const& score : sorted_scores) {
		score_string += "| " +
			std::string(item_max_length - score.item.size(), ' ') + score.item + " | " +
			std::string(wins_max_length - numberOfDigits(score.wins), ' ') + std::to_string(score.wins) + " | " +
			std::string(losses_max_length - numberOfDigits(score.losses), ' ') + std::to_string(score.losses) + " |\n";
	}
	score_string += std::string(total_length, '-') + '\n';
	return score_string;
}

/* -------------- Combine scores -------------- */
void addScore(Scores& scores, Score const& new_score) {
	auto it = std::find_if(scores.begin(), scores.end(), [&](Score const& score) {
		return score.item == new_score.item;
		});
	if (it == scores.end()) {
		scores.emplace_back(new_score);
	}
	else {
		it->wins += new_score.wins;
		it->losses += new_score.losses;
	}
}
auto addScores(Scores const& scores_a, Scores const& scores_b) -> Scores {
	Scores combined_scores = scores_a;
	for (auto const& score : scores_b) {
		addScore(combined_scores, score);
	}
	return combined_scores;
}
auto combineScores(std::vector<Scores> const& score_sets) -> Scores {
	Scores combined_scores{};
	for (auto const& scores : score_sets) {
		combined_scores = addScores(combined_scores, scores);
	}
	return combined_scores;
}

/* -------------- Menu alternatives -------------- */
auto vote(std::optional<VotingRound>& voting_round, Option option) -> std::string {
	if (!voting_round.has_value()) {
		return "No poll to vote in";
	}
	if (!hasRemainingVotes(voting_round.value())) {
		return "No ongoing poll with pending votes";
	}
	voting_round.value().votes.emplace_back(voting_round.value().index_pairs[voting_round.value().votes.size()], option);
	voting_round.value().is_saved = false;
	return {};
}
auto undo(std::optional<VotingRound>& voting_round) -> std::string {
	if (!voting_round.has_value()) {
		return "No poll to undo from";
	}
	// Need to check since pop_back() decrements vector size even when it's 0, causing an underflow
	if (!voting_round.value().votes.empty()) {
		voting_round.value().votes.pop_back();
		voting_round.value().is_saved = false;
	}
	return {};
}
auto save(std::optional<VotingRound>& voting_round, std::string const votes_file_name, std::string const results_file_name) -> std::pair<bool, std::string> {
	if (!voting_round.has_value()) {
		return { false, "No poll to save" };
	}
	if (!saveFile(votes_file_name, convertVotingRoundToText(voting_round.value()))) {
		return { false, "Could not save votes to " + votes_file_name + "." };
	}

	std::string result{ "Votes saved to " + votes_file_name + "." };
	if (hasRemainingVotes(voting_round.value())) {
		voting_round.value().is_saved = true;
		return { true, result };
	}

	if (!saveFile(results_file_name, generateScoreFileData(sortScores(calculateScores(voting_round.value()))))) {
		result += " Could not save results to " + results_file_name + ".";
	}
	else {
		result += " Results saved to " + results_file_name + ".";
	}
	voting_round.value().is_saved = true;
	return { true, result };
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
	print(createScoreTable(calculateScores(voting_round.value())), false);
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
