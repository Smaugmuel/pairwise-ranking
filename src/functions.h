#pragma once

#include "voting_round.h"

/* -------------- Command line inputs -------------- */
auto continueWithoutSaving(std::optional<VotingRound> const& voting_round, std::string const& str) -> bool;

/* -------------- Menu alternatives -------------- */
auto newRound(VotingFormat format, Items const& items) -> std::optional<VotingRound>;
void loadRound(std::optional<VotingRound>& voting_round, std::vector<std::string> const& lines);
void printScores(std::optional<VotingRound> const& voting_round);
