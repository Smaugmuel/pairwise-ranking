#pragma once

#include "score_helpers.h"

#include <algorithm>
#include <sstream>

#include "helpers.h"
#include "print.h"

namespace
{

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

} // namespace

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

/* -------------- Score conversion -------------- */
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
auto generateScoreFileData(Scores const& scores) -> std::vector<std::string> {
	std::vector<std::string> lines{};
	for (auto const& score : scores) {
		lines.emplace_back(std::to_string(score.wins) + " " + std::to_string(score.losses) + " " + score.item);
	}
	return lines;
}