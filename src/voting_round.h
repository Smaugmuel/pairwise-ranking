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

/* -------------- Voting round -------------- */

struct VotingRound final {
	Items original_items_order{};
	Items items{};
	Seed seed{ 0 };
	IndexPairs index_pairs{};
	Votes votes{};
	bool is_saved{ false };
	bool reduced_voting{ false };
};

/* -------------- Voting round generation -------------- */
auto generateSeed() -> Seed;
auto generateIndexPairs(uint32_t const number_of_items) -> IndexPairs;
void shuffleVotingOrder(VotingRound& voting_round);
auto generateNewVotingRound(Items const& items, bool reduce_voting) -> std::optional<VotingRound>;
auto parseVotingRoundFromText(std::vector<std::string> const& lines) -> std::optional<VotingRound>;

/* -------------- Voting round verification -------------- */
auto numberOfScheduledVotes(VotingRound const& voting_round) -> uint32_t;
auto sumOfFirstIntegers(size_t n) -> size_t;

/* -------------- Vote pruning -------------- */
auto sortIndexPair(IndexPair const& index_pair) -> IndexPair;
auto generateIndexPairWithOffset(uint32_t const i, uint32_t const offset, uint32_t const number_of_items) -> IndexPair;
void removeNSpacedPairs(IndexPairs& index_pairs, uint32_t const offset, uint32_t const number_of_items);
auto pruneVotes(IndexPairs const& index_pairs, uint32_t const number_of_items, uint32_t const pruning_iterations) -> IndexPairs;
auto pruningAmount(uint32_t const number_of_items) -> uint32_t;
auto reduceVotes(IndexPairs const& index_pairs, uint32_t const number_of_items) -> IndexPairs;
void pruneVotes(VotingRound& voting_round);

/* -------------- General helpers -------------- */
auto isNumber(std::string const& str) -> bool;

/* -------------- String-to-object parsing -------------- */
auto parseWords(std::string const& str) -> std::vector<std::string>;
auto parseNumber(std::string const& str) -> std::optional<uint32_t>;
auto parseVote(std::string const& str) -> Vote;
