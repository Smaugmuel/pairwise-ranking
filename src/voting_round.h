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

struct Matchup {
	Item item_a{};
	Item item_b{};
};

/* -------------- Voting round -------------- */
class VotingRound final {
public:

	static auto create(Items const& items, bool reduce_voting, Seed seed = 0) -> std::optional<VotingRound>;
	static auto create(std::vector<std::string> const& lines) -> std::optional<VotingRound>;

	auto prune() -> bool;
	auto shuffle() -> bool;

	auto vote(Option option) -> bool;
	auto undoVote() -> bool;
	auto save(std::string const& file_name) -> bool;

	// Internal members access
	auto items() const -> Items const&;
	auto originalItemOrder() const -> Items const&;
	auto format() const -> VotingFormat;
	auto seed() const -> Seed;
	auto indexPairs() const -> IndexPairs const&;
	auto votes() const -> Votes const&;
	auto isSaved() const -> bool;

	auto currentMatchup() const -> std::optional<Matchup>;
	auto currentVotingLine() const -> std::optional<std::string>;
	auto hasRemainingVotes() const -> bool;
	auto convertToText() const -> std::vector<std::string>;
	auto numberOfScheduledVotes() const -> uint32_t;

private:
	Items original_items_order_{};
	Items items_{};
	Seed seed_{ 0 };
	IndexPairs index_pairs_{};
	Votes votes_{};
	bool is_saved_{ false };
	VotingFormat voting_format_{ VotingFormat::Invalid };
};
