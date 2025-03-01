#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "voting_round.h"

auto activeMenuString(std::optional<VotingRound> const& voting_round, bool const show_intro_screen) -> std::string;
auto helpString() -> std::string;
auto newRoundFormatString(uint32_t const number_of_items) -> std::string;
