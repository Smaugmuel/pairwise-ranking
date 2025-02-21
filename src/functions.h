#pragma once

#include "voting_round.h"

struct Score {
	Item item{};
	uint32_t wins = 0;
	uint32_t losses = 0;
};
auto operator==(Score const& a, Score const& b) -> bool;

struct ScoreMaxLengths {
	size_t max_length_str{ 0 };
	size_t max_length_wins{ 0 };
	size_t max_length_losses{ 0 };
};

using Scores = std::vector<Score>;

/* -------------- General helpers -------------- */
template<typename T>
constexpr auto to_underlying(T t) -> std::underlying_type_t<T> {
	return static_cast<std::underlying_type_t<T>>(t);
}

/* -------------- Voting round verification -------------- */
auto hasDuplicateItems(Items const& items) -> bool;
auto hasInvalidScheduledVotes(IndexPairs const& index_pairs, uint32_t number_of_items) -> bool;
auto hasDuplicateScheduledVotes(IndexPairs const& index_pairs) -> bool;
auto expectedIndexPairs(Items const& items, bool reduced_voting) -> uint32_t;
auto hasVotesWithInvalidIndices(Votes const& votes, uint32_t number_of_items) -> bool;
auto hasVotesWithInvalidVoteOption(Votes const& votes) -> bool;
auto hasDuplicateVotes(Votes const& votes) -> bool;
auto verifyVotingRound(VotingRound const& voting_round) -> bool;

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
auto parseNextNumber(std::stringstream& stream) -> std::optional<uint32_t>;
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