#pragma once

#include <optional>
#include <string>
#include <vector>

#include "vote.h"
#include "voting_format.h"

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

struct Matchup {
	Item item_a{};
	Item item_b{};
};

/* -------------- Voting round -------------- */
class VotingRound final {
	class ScoreBased final {
	public:
		static auto create(uint32_t const number_of_items, VotingFormat const voting_format) -> std::optional<ScoreBased>;

		auto shuffle(Seed const seed) -> bool;
		auto currentIndexPair(uint32_t const counter) const -> IndexPair;
		auto indexPairs() const -> IndexPairs const&;
		auto numberOfScheduledVotes() const -> uint32_t;
	private:
		IndexPairs index_pairs_{};
	};
	class RankBased final {
	public:
		static auto create() -> std::optional<RankBased>;

		void vote(Items& items, Option option);
		auto undoVote(Items& items, Votes const& votes) -> bool;
		auto currentIndexPair() const -> IndexPair;
		auto numberOfSortedItems() const -> uint32_t;
	private:
		uint32_t number_of_sorted_items_{ 1 };
		uint32_t start_index_{ 0 };
		uint32_t end_index_{ 1 };
	};
public:

	static auto create(Items const& items, VotingFormat voting_format, Seed seed = 0) -> std::optional<VotingRound>;
	static auto create(std::vector<std::string> const& lines) -> std::optional<VotingRound>;

	auto shuffle() -> bool;

	auto vote(Option option) -> bool;
	auto undoVote() -> bool;
	auto save(std::string const& file_name) -> bool;

	// Internal members access
	auto items() const -> Items const&;
	auto originalItemOrder() const -> Items const&;
	auto format() const -> VotingFormat;
	auto seed() const -> Seed;
	auto votes() const -> Votes const&;
	auto isSaved() const -> bool;

	// TODO: This is an implementation detail currently, only applicable for
	// VotingFormat::Ranking. Find a better way to deal with this.
	auto numberOfSortedItems() const -> uint32_t;

	// TODO: This is an implementation detail currently, only applicable for
	// VotingFormat::Full or Reduced. Find a better way to deal with this.
	auto numberOfScheduledVotes() const -> uint32_t;

	auto currentMatchup() const -> std::optional<Matchup>;
	auto currentVotingLine() const -> std::optional<std::string>;
	auto hasRemainingVotes() const -> bool;
	auto convertToText() const -> std::vector<std::string>;

private:
	auto shuffleImpl() -> bool;
	auto counterStringImpl() const -> std::string;
	auto voteImpl(Option option) -> bool;
	auto undoVoteImpl() -> bool;
	auto createFormatImpl() -> bool;
	auto currentIndexPairImpl() const -> IndexPair;
	auto hasRemainingVotesImpl() const -> bool;
	auto numberOfSortedItemsImpl() const -> uint32_t;
	auto numberOfScheduledVotesImpl() const -> uint32_t;

	std::optional<ScoreBased> score_based_{};
	std::optional<RankBased> rank_based_{};

	Items original_items_order_{};
	Items items_{};
	Seed seed_{ 0 };
	Votes votes_{};
	bool is_saved_{ false };
	VotingFormat voting_format_{ VotingFormat::Invalid };
};
