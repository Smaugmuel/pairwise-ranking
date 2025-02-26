#include "menus.h"

#include <cmath>

#include "helpers.h"

auto activeMenuString(std::optional<VotingRound> const& voting_round, bool const show_intro_screen) -> std::string {
	std::string menu{};
	if (show_intro_screen) {
		menu += "Welcome! ";
	}

	if (!voting_round.has_value()) {
		menu += "Press H if you need help: ";
		return menu;
	}

	if (!voting_round.value().hasRemainingVotes()) {
		menu += "Voting round finished. [H]elp. [Q]uit. Your choice: ";
		return menu;
	}

	auto const current_voting_line = voting_round.value().currentVotingLine();
	if (current_voting_line.has_value()) {
		menu += current_voting_line.value();
	}
	return menu;
}
auto helpString() -> std::string {
	return
		"Display this [H]elp menu\n"
		"[Q]uit program\n"
		"Start a [N]ew voting round\n"
		"[L]oad started voting round\n"
		"[S]ave votes and scores\n"
		"Vote for option [A]\n"
		"Vote for option [B]\n"
		"[U]ndo last vote\n"
		"[P]rint scores\n"
		"[C]ombine previously saved scores\n";
}
auto newRoundFormatString(uint32_t const number_of_items) -> std::string {
	auto const full_voting_size{ sumOfFirstIntegers(number_of_items - 1) };
	auto const reduced_voting_size{ full_voting_size - number_of_items * pruningAmount(number_of_items) };
	auto const ranked_voting_size{ static_cast<uint32_t>(number_of_items * std::log2f(number_of_items)) };
	return
		"Please choose voting format:\n"
		"[F]ull     score-based voting - " + std::to_string(full_voting_size) + " votes\n"
		"[R]educed  score-based voting - " + std::to_string(reduced_voting_size) + " votes\n"
		"[I]nsertion rank-based voting - Around " + std::to_string(ranked_voting_size) + " votes\n"
		"[C]ancel";
}
