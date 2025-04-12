#include "voting_format.h"

auto characterToVotingFormat(char const c) noexcept -> VotingFormat {
	switch (c) {
	case 'f':
		return VotingFormat::Full;
	case 'r':
		return VotingFormat::Reduced;
	case 'i':
		return VotingFormat::Ranked;
	default:
		return VotingFormat::Invalid;
	}
}
auto stringToVotingFormat(std::string const& s) noexcept -> VotingFormat {
	if (s == "reduced") {
		return VotingFormat::Reduced;
	}
	if (s == "full") {
		return VotingFormat::Full;
	}
	if (s == "ranked") {
		return VotingFormat::Ranked;
	}
	return VotingFormat::Invalid;
}
auto votingFormatToString(VotingFormat f) noexcept -> std::string {
	switch (f) {
	case VotingFormat::Full:
		return "full";
	case VotingFormat::Reduced:
		return "reduced";
	case VotingFormat::Ranked:
		return "ranked";
	case VotingFormat::Invalid:
	default:
		return "";
	}
}
