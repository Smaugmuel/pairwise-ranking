#pragma once

#include <optional>
#include <string>
#include <vector>

#include "vote.h"

/* -------------- Items -------------- */
using Item = std::string;
using Items = std::vector<Item>;

/* -------------- Seed -------------- */
using Seed = uint32_t;

/* -------------- Index pairs -------------- */
using Index = uint32_t;
using IndexPair = std::pair<Index, Index>;
struct IndexPairHash {
	uint64_t operator()(IndexPair const& index_pair) const {
		return (static_cast<uint64_t>(index_pair.first) << 32) + static_cast<uint64_t>(index_pair.second);
	}
};
using IndexPairs = std::vector<IndexPair>;

/* -------------- Format -------------- */
enum class VotingFormat : uint32_t {
	Invalid,
	Full,
	Reduced,
};

/* -------------- Voting round -------------- */
class VotingRound final {
public:

	static auto create(Items const& items, bool reduce_voting) -> std::optional<VotingRound>;
	static auto create(std::vector<std::string> const& lines) -> std::optional<VotingRound>;

	auto prune() -> bool;
	auto shuffle() -> bool;
	auto vote(Option option) -> bool;
	auto undoVote() -> bool;
	auto save(std::string const& file_name) -> bool;
	auto getItems() const -> Items const&;
	auto getVotes() const -> Votes const&;
	auto verify() const -> bool;
	auto currentVotingLine() const -> std::optional<std::string>;
	auto hasRemainingVotes() const -> bool;
	auto numberOfScheduledVotes() const -> uint32_t;
	auto convertToText() const -> std::vector<std::string>;

	Items original_items_order{};
	Items items{};
	Seed seed{ 0 };

	// index_pairs is used for score-based voting. ranked_items is used for rank-based voting.
	IndexPairs index_pairs{};
	Items ranked_items;

	Votes votes{};
	bool is_saved{ false };
	VotingFormat voting_format{ VotingFormat::Invalid };
};
