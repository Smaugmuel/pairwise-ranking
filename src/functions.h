#pragma once

#include "voting_round.h"

/* -------------- Menu strings -------------- */
auto getActiveMenuString(std::optional<VotingRound> const& voting_round, bool const show_intro_screen)->std::string;
auto getHelpString()->std::string;

/* -------------- Command line inputs -------------- */
auto getKey() -> char;
auto getConfirmation() -> bool;
auto continueWithoutSaving(std::optional<VotingRound> const& voting_round, std::string const& str) -> bool;

/* -------------- Menu alternatives -------------- */
auto vote(std::optional<VotingRound>& voting_round, Option option)->std::string;
auto undo(std::optional<VotingRound>& voting_round)->std::string;
void newRound(std::optional<VotingRound>& voting_round);
void loadRound(std::optional<VotingRound>& voting_round, std::vector<std::string> const& lines);
void printScores(std::optional<VotingRound> const& voting_round);
void combine();
