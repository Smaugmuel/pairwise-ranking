#pragma once

#include "voting_round.h"

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
