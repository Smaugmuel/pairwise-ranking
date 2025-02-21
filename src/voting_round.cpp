#include "voting_round.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <unordered_set>

#include "constants.h"
#include "print.h"

namespace
{

auto itemsAreUnique(Items const& items) -> bool {
	return items.size() == std::unordered_set<Item>{ items.begin(), items.end() }.size();
}
auto votingIndicesAreSmallerThanNumberOfItems(Votes const& votes, uint32_t const number_of_items) -> bool {
	for (auto const& vote : votes) {
		if (vote.index_pair.first >= number_of_items ||
			vote.index_pair.second >= number_of_items) {
			return false;
		}
	}
	return true;
}
auto hasDuplicateMatchups(Votes const& votes) -> bool {
	std::unordered_set<IndexPair, IndexPairHash> index_pair_set{};
	for (auto const& vote : votes) {
		index_pair_set.insert(sortIndexPair(vote.index_pair));
	}
	return index_pair_set.size() != votes.size();
}

} // namespace


/* -------------- Voting round generation -------------- */
auto generateSeed() -> Seed {
	return static_cast<Seed>(std::chrono::system_clock::now().time_since_epoch().count());
}
auto generateIndexPairs(uint32_t const number_of_items) -> IndexPairs {
	if (number_of_items < 2) {
		printError("Number of items (" + std::to_string(number_of_items) + ") is less than two");
		return {};
	}

	IndexPairs index_pairs;
	for (uint32_t item_a = 0; item_a < number_of_items; item_a++) {
		for (uint32_t item_b = item_a + 1; item_b < number_of_items; item_b++) {
			index_pairs.emplace_back(item_a, item_b);
		}
	}

	return index_pairs;
}
void shuffleVotingOrder(VotingRound& voting_round) {
	std::default_random_engine random_engine(voting_round.seed);
	std::shuffle(voting_round.index_pairs.begin(), voting_round.index_pairs.end(), random_engine);
	std::shuffle(voting_round.items.begin(), voting_round.items.end(), random_engine);
}
auto generateNewVotingRound(Items const& items, bool reduce_voting) -> std::optional<VotingRound> {
	if (items.size() < 2) {
		printError("Can't generate voting round: Fewer than two items");
		return std::nullopt;
	}
	for (auto const& item : items) {
		if (item.empty() || item == "") {
			printError("Can't generate voting round: Items are empty");
			return std::nullopt;
		}
	}
	VotingRound voting_round{};
	voting_round.items = items;
	voting_round.seed = generateSeed();
	voting_round.reduced_voting = false;
	voting_round.index_pairs = generateIndexPairs(static_cast<uint32_t>(voting_round.items.size()));

	// Retain item order, to make seeded item shuffling deterministic
	voting_round.original_items_order = items;

	if (reduce_voting) {
		pruneVotes(voting_round);
	}

	return voting_round;
}
auto parseVotingRoundFromText(std::vector<std::string> const& lines) -> std::optional<VotingRound> {
	VotingRound voting_round{};
	size_t line_index = 0;

	// Load items
	for (; line_index < lines.size(); line_index++) {
		if (lines[line_index].empty()) {
			break;
		}
		voting_round.items.emplace_back(lines[line_index]);
	}
	if (voting_round.items.size() < 2) {
		printError("Parsed items are fewer than two");
		return std::nullopt;
	}
	if (!itemsAreUnique(voting_round.items)) {
		printError("Parsed items are not unique");
		return std::nullopt;
	}

	// Retain item order, to make seeded item shuffling deterministic
	voting_round.original_items_order = voting_round.items;

	// Skip empty line
	line_index++;

	// Load seed
	if (line_index >= lines.size()) {
		printError("Could not load seed from poll file");
		return std::nullopt;
	}
	std::optional<uint32_t> const seed = parseNumber(lines[line_index++]);
	if (!seed.has_value()) {
		printError("Invalid seed");
		return std::nullopt;
	}
	voting_round.seed = seed.value();

	// Load reduced voting round setting
	if (line_index >= lines.size()) {
		printError("Voting format missing");
		return std::nullopt;
	}

	bool reduced_voting = false;
	if (lines[line_index] == "reduced") {
		reduced_voting = true;
	}
	else if (lines[line_index] == "full") {
		reduced_voting = false;
	}
	else {
		printError("Incorrect voting format: " + lines[line_index]);
		return std::nullopt;
	}
	line_index++;

	voting_round.index_pairs = generateIndexPairs(static_cast<uint32_t>(voting_round.items.size()));
	if (reduced_voting) {
		pruneVotes(voting_round);
	}

	shuffleVotingOrder(voting_round);

	// Load votes
	for (; line_index < lines.size(); line_index++) {
		auto const vote = parseVote(lines[line_index]);
		if (vote.index_pair == IndexPair{ 0, 0 }) {
			printError("Failed to parse vote");
			return std::nullopt;
		}
		voting_round.votes.emplace_back(vote);
	}
	if (!votingIndicesAreSmallerThanNumberOfItems(voting_round.votes, voting_round.items.size())) {
		printError("Some parsed indices in vote are larger than allowed");
		return std::nullopt;
	}

	if (hasDuplicateMatchups(voting_round.votes)) {
		printError("Some vote matchups are duplicated");
		return std::nullopt;
	}

	if (voting_round.votes.size() > numberOfScheduledVotes(voting_round)) {
		printError("Too many votes parsed. Voting round is invalidated");
		return std::nullopt;
	}

	voting_round.is_saved = true;
	return voting_round;
}

/* -------------- Voting round verification -------------- */
auto numberOfScheduledVotes(VotingRound const& voting_round) -> uint32_t {
	return static_cast<uint32_t>(voting_round.index_pairs.size());
}
auto sumOfFirstIntegers(size_t n) -> size_t {
	return (n * (n + 1)) / 2;
}

/* -------------- Vote pruning -------------- */
auto sortIndexPair(IndexPair const& index_pair) -> IndexPair {
	if (index_pair.first < index_pair.second) {
		return index_pair;
	}
	return std::make_pair(index_pair.second, index_pair.first);
}
auto generateIndexPairWithOffset(uint32_t const i, uint32_t const offset, uint32_t const number_of_items) -> IndexPair {
	return std::make_pair(i, (i + offset) % number_of_items);
}
void removeNSpacedPairs(IndexPairs& index_pairs, uint32_t const offset, uint32_t const number_of_items) {
	for (uint32_t i = 0; i < number_of_items; i++) {
		// Since pairs are generated with the first element always being smaller than the second,
		// the order must be ensured here.
		auto const pair = sortIndexPair(generateIndexPairWithOffset(i, offset, number_of_items));
		index_pairs.erase(std::remove(index_pairs.begin(), index_pairs.end(), pair), index_pairs.end());
	}
}
auto pruneVotes(IndexPairs const& index_pairs, uint32_t const number_of_items, uint32_t const pruning_iterations) -> IndexPairs {
	IndexPairs reduced_pairs = index_pairs;
	for (uint32_t index_offset = 0; index_offset < pruning_iterations; index_offset++) {
		removeNSpacedPairs(reduced_pairs, index_offset + 1, number_of_items);
	}
	return reduced_pairs;
}
auto pruningAmount(uint32_t const number_of_items) -> uint32_t {
	if (number_of_items < kMinimumItemsForPruning) {
		return 0;
	}
	return 1 + (number_of_items - kMinimumItemsForPruning) / 2;
}
auto reduceVotes(IndexPairs const& index_pairs, uint32_t const number_of_items) -> IndexPairs {
	if (number_of_items < kMinimumItemsForPruning) {
		printError("Too few items (" + std::to_string(number_of_items) + ") to reduce voting. No reduction performed");
		return index_pairs;
	}
	return pruneVotes(index_pairs, number_of_items, pruningAmount(number_of_items));
}
void pruneVotes(VotingRound& voting_round) {
	if (voting_round.reduced_voting) {
		printError("Already pruned. Can't prune again");
		return;
	}
	if (!voting_round.votes.empty()) {
		printError("Can't prune when votes exist");
		return;
	}
	auto const number_of_items = static_cast<uint32_t>(voting_round.items.size());
	voting_round.index_pairs = reduceVotes(voting_round.index_pairs, number_of_items);
	voting_round.reduced_voting = voting_round.index_pairs.size() < sumOfFirstIntegers(number_of_items - 1);
}

/* -------------- General helpers -------------- */
auto isNumber(std::string const& str) -> bool {
	if (str.empty()) {
		return false;
	}
	for (auto c : str) {
		if (!std::isdigit(c)) {
			return false;
		}
	}
	return true;
}

/* -------------- String-to-object parsing -------------- */
auto parseWords(std::string const& str) -> std::vector<std::string> {
	std::stringstream stream(str);
	std::vector<std::string> words{};
	std::string word{};
	while (stream >> word) {
		words.push_back(word);
	}
	return words;
}

auto parseNumber(std::string const& str) -> std::optional<uint32_t> {
	if (!isNumber(str)) {
		printError("Unable to parse '" + str + "'. Not a number");
		return std::nullopt;
	}
	return static_cast<uint32_t>(std::stoul(str));
}

auto parseVote(std::string const& str) -> Vote {
	std::vector<std::string> words = parseWords(str);
	if (words.size() != 3) {
		printError("Invalid vote format. Number of words (" + std::to_string(words.size()) + ") doesn't equal 3");
		return {};
	}
	std::optional<uint32_t> option_a = parseNumber(words[0]);
	std::optional<uint32_t> option_b = parseNumber(words[1]);
	std::optional<uint32_t> winner = parseNumber(words[2]);
	if (!(option_a.has_value() && option_b.has_value() && winner.has_value())) {
		printError("Unable to parse vote");
		return {};
	}
	if (!(winner.value() == 0 || winner.value() == 1)) {
		printError("Voted option is invalid");
		return {};
	}

	return Vote{
		std::make_pair(option_a.value(), option_b.value()),
		static_cast<Option>(winner.value()) };
}
