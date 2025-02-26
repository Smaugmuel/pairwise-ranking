#pragma once

#include <cstdint>
#include <string>

std::string const kItemsFile = "items.txt";
std::string const kVotesFile = "votes.txt";
std::string const kScoresFile = "scores.txt";
std::string const kRankingFile = "ranking.txt";
std::string const kCombinedScoresFile = "combined_scores.txt";

constexpr uint32_t kMinimumItemsForPruning = 6;
