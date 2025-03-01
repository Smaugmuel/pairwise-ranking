#pragma once

#include "voting_round.h"

/* -------------- Command line inputs -------------- */
auto getKey() -> char;
auto getConfirmation() -> bool;
auto continueWithoutSaving(std::optional<VotingRound> const& voting_round, std::string const& str) -> bool;

/* -------------- Menu alternatives -------------- */
void newRound(std::optional<VotingRound>& voting_round);
void loadRound(std::optional<VotingRound>& voting_round, std::vector<std::string> const& lines);
void printScores(std::optional<VotingRound> const& voting_round);
void combine();
