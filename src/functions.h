#pragma once

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

/* -------------- Constants -------------- */
std::string const kItemsFile = "items.txt";
std::string const kVotesFile = "votes.txt";
std::string const kSeedFile = "seed.txt";
std::string const kResultFile = "results.txt";
std::string const kCombinedResultFile = "combined_results.txt";

constexpr uint32_t kMinimumItemsForPruning = 6;

/* -------------- Type definitions -------------- */
using Item = std::string;
using Items = std::vector<Item>;
using Seed = uint32_t;
using Index = uint32_t;
using IndexPair = std::pair<Index, Index>;
struct IndexPairHash {
	uint64_t operator()(IndexPair const& index_pair) const {
		return (static_cast<uint64_t>(index_pair.first) << 32) + static_cast<uint64_t>(index_pair.second);
	}
};

using IndexPairs = std::vector<IndexPair>;

enum class Option : uint32_t {
	A,
	B
};

struct Score {
	Item item{};
	uint32_t wins = 0;
	uint32_t losses = 0;
};
auto operator==(Score const& a, Score const& b) -> bool;

struct Vote {
	IndexPair index_pair{};
	Option winner{};
};
auto operator==(Vote const& a, Vote const& b) -> bool;

struct ScoreMaxLengths {
	size_t max_length_str{ 0 };
	size_t max_length_wins{ 0 };
	size_t max_length_losses{ 0 };
};

using Scores = std::vector<Score>;
using Votes = std::vector<Vote>;

struct VotingRound {
	Items original_items_order{};
	Items items{};
	Votes votes{};
	IndexPairs index_pairs{};
	Seed seed{ 0 };
	bool is_saved{ false };
	bool reduced_voting{ false };
};

/* -------------- General helpers -------------- */
auto isNumber(std::string const& str) -> bool;
void print(std::string const& str, bool add_newline = true);

template<typename T>
constexpr auto to_underlying(T t) -> std::underlying_type_t<T> {
	return static_cast<std::underlying_type_t<T>>(t);
}

/* -------------- Voting round generation -------------- */
auto generateSeed() -> Seed;
auto generateIndexPairs(uint32_t const number_of_items) -> IndexPairs;
void shuffleVotingOrder(VotingRound& voting_round);
auto generateNewVotingRound(Items const& items, bool reduce_voting) -> std::optional<VotingRound>;
auto parseVotingRoundFromText(std::vector<std::string> const& lines) -> std::optional<VotingRound>;

/* -------------- Voting round verification -------------- */
auto hasDuplicateItems(Items const& items) -> bool;
auto hasInvalidScheduledVotes(IndexPairs const& index_pairs, uint32_t number_of_items) -> bool;
auto hasDuplicateScheduledVotes(IndexPairs const& index_pairs) -> bool;
auto numberOfScheduledVotes(VotingRound const& voting_round) -> uint32_t;
auto sumOfFirstIntegers(size_t n) -> size_t;
auto expectedIndexPairs(Items const& items, bool reduced_voting) -> uint32_t;
auto hasVotesWithInvalidIndices(Votes const& votes, uint32_t number_of_items) -> bool;
auto hasVotesWithInvalidVoteOption(Votes const& votes) -> bool;
auto hasDuplicateVotes(Votes const& votes) -> bool;
auto verifyVotingRound(VotingRound const& voting_round) -> bool;

/* -------------- Vote pruning -------------- */
auto sortIndexPair(IndexPair const& index_pair)->IndexPair;
auto generateIndexPairWithOffset(uint32_t const i, uint32_t const offset, uint32_t const number_of_items)->IndexPair;
void removeNSpacedPairs(IndexPairs& index_pairs, uint32_t const offset, uint32_t const number_of_items);
auto pruneVotes(IndexPairs const& index_pairs, uint32_t const number_of_items, uint32_t const pruning_iterations)->IndexPairs;
auto pruningAmount(uint32_t const number_of_items)->uint32_t;
auto reduceVotes(IndexPairs const& index_pairs, uint32_t const number_of_items)->IndexPairs;
void pruneVotes(VotingRound& voting_round);

/* -------------- Voting round score calculation -------------- */
void incrementWinner(Scores& scores, Item const& item);
void incrementLoser(Scores& scores, Item const& item);
auto calculateScores(VotingRound const& voting_round) -> Scores;

/* -------------- Voting round miscellaneous -------------- */
auto hasRemainingVotes(VotingRound const& voting_round) -> bool;
auto convertVotingRoundToText(VotingRound const& voting_round) -> std::vector<std::string>;

/* -------------- Printing votable options -------------- */
auto currentOptionItems(VotingRound const& voting_round) -> std::pair<std::string, std::string>;
auto findMaxLength(Items const& items) -> size_t;
auto numberOfDigits(size_t n) -> size_t;
auto counterString(size_t counter, size_t total) -> std::string;
auto currentVotingLine(VotingRound const& voting_round) -> std::optional<std::string>;

/* -------------- Printing active screen -------------- */
auto getActiveMenuString(std::optional<VotingRound> const& voting_round, bool const show_intro_screen)->std::string;
void printActiveMenu(std::optional<VotingRound> const& voting_round, bool const show_intro_screen);

/* -------------- Printing miscellaneous -------------- */
void printError(std::string const& err);
auto getHelpString()->std::string;
void printHelp();

/* -------------- Command line inputs -------------- */
auto getKey() -> char;
auto getLine() -> std::string;
auto getConfirmation() -> bool;
auto getMultipleFileNames() -> std::vector<std::string>;
auto continueWithoutSaving(std::optional<VotingRound> const& voting_round, std::string const& str) -> bool;

/* -------------- Localization -------------- */
auto fixSwedish(std::string const& str) -> std::string;

/* -------------- String-to-object parsing -------------- */
auto readNextWord(std::stringstream& stream) -> std::string;
auto parseWords(std::string const& str) -> std::vector<std::string>;
auto parseNumber(std::string const& str) -> std::optional<uint32_t>;
auto parseNextNumber(std::stringstream& stream) -> std::optional<uint32_t>;
auto parseVote(std::string const& str) -> Vote;
auto parseScore(std::string const& str) -> Score;
auto parseItems(std::vector<std::string> const& lines) -> Items;
auto parseScores(std::vector<std::string> const& lines) -> Scores;

/* -------------- Object-to-string conversion -------------- */
auto generateScoreFileData(Scores const& scores) -> std::vector<std::string>;

/* -------------- File system management -------------- */
auto saveFile(std::string const& file_name, std::vector<std::string> const& lines) -> bool;
auto loadFile(std::string const& file_name) -> std::vector<std::string>;

/* -------------- Verifications -------------- */
auto verifyFilesExist(std::vector<std::string> const& file_names) -> bool;

/* -------------- Print scores -------------- */
auto sortScores(Scores const& scores)->Scores;
auto findMaxLengthItem(Scores const& scores)->size_t;
auto findMaxLengthWins(Scores const& scores)->size_t;
auto findMaxLengthLosses(Scores const& scores)->size_t;
auto createScoreTable(Scores const& scores)->std::string;

/* -------------- Combine scores -------------- */
void addScore(Scores& scores, Score const& new_score);
auto addScores(Scores const& a, Scores const& b) -> Scores;
auto combineScores(std::vector<Scores> const& score_sets) -> Scores;

/* -------------- Menu alternatives -------------- */
auto vote(std::optional<VotingRound>& voting_round, Option option)->std::string;
auto undo(std::optional<VotingRound>& voting_round)->std::string;
auto save(std::optional<VotingRound>& voting_round, std::string const votes_file_name, std::string const results_file_name)->std::pair<bool, std::string>;
void newRound(std::optional<VotingRound>& voting_round);
void loadRound(std::optional<VotingRound>& voting_round, std::vector<std::string> const& lines);
void printScores(std::optional<VotingRound> const& voting_round);
void combine();