#pragma once

#include "voting_round.h"

/* -------------- Menu strings -------------- */
auto activeMenuString(std::optional<VotingRound> const& voting_round, bool const show_intro_screen) -> std::string;
auto helpString() -> std::string;
auto newRoundFormatString(uint32_t const number_of_items) -> std::string;

/* -------------- Command line inputs -------------- */
auto getKey() -> char;
auto getConfirmation() -> bool;
auto continueWithoutSaving(std::optional<VotingRound> const& voting_round, std::string const& str) -> bool;

/* -------------- Menu alternatives -------------- */
void newRound(std::optional<VotingRound>& voting_round);
void loadRound(std::optional<VotingRound>& voting_round, std::vector<std::string> const& lines);
void printScores(std::optional<VotingRound> const& voting_round);
void combine();
