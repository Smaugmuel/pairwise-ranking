#pragma once

#include <optional>
#include <string>
#include <vector>

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

/* -------------- Votes -------------- */
enum class Option : uint32_t {
	A,
	B
};

struct Vote {
	IndexPair index_pair{};
	Option winner{};
};
auto operator==(Vote const& a, Vote const& b) -> bool;
using Votes = std::vector<Vote>;

/* -------------- Score -------------- */
struct Score {
	Item item{};
	uint32_t wins = 0;
	uint32_t losses = 0;
};
auto operator==(Score const& a, Score const& b) -> bool;
using Scores = std::vector<Score>;

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
	auto verify() const -> bool;
	auto currentVotingLine() const -> std::optional<std::string>;
	auto hasRemainingVotes() const -> bool;
	auto numberOfScheduledVotes() const -> uint32_t;
	auto calculateScores() const -> Scores;
	auto convertToText() const -> std::vector<std::string>;

	Items original_items_order{};
	Items items{};
	Seed seed{ 0 };
	IndexPairs index_pairs{};
	Votes votes{};
	bool is_saved{ false };
	bool reduced_voting{ false };
};
