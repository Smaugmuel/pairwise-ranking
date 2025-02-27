#pragma once

#include <cstdint>
#include <string>

enum class VotingFormat : uint32_t {
	Invalid,
	Full,
	Ranked,
	Reduced,
};

auto characterToVotingFormat(char const c) -> VotingFormat;
auto stringToVotingFormat(std::string const& s) -> VotingFormat;
auto votingFormatToString(VotingFormat f) -> std::string;
