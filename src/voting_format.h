#pragma once

#include <cstdint>
#include <string>

enum class VotingFormat : uint32_t {
	Invalid,
	Full,
	Ranked,
	Reduced,
};

auto characterToVotingFormat(char const c) noexcept -> VotingFormat;
auto stringToVotingFormat(std::string const& s) noexcept -> VotingFormat;
auto votingFormatToString(VotingFormat f) noexcept -> std::string;
