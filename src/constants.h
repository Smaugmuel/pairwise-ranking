#pragma once

#include <cstdint>
#include <string>

std::string const kItemsFile = "items.txt";
std::string const kVotesFile = "votes.txt";
std::string const kSeedFile = "seed.txt";
std::string const kResultFile = "results.txt";
std::string const kCombinedResultFile = "combined_results.txt";

constexpr uint32_t kMinimumItemsForPruning = 6;
