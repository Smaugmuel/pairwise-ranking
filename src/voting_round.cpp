#include "voting_round.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <unordered_set>

#include "constants.h"
#include "helpers.h"
#include "print.h"

namespace
{

auto sortIndexPair(IndexPair const& index_pair) -> IndexPair {
	if (index_pair.first < index_pair.second) {
		return index_pair;
	}
	return std::make_pair(index_pair.second, index_pair.first);
}
auto itemsAreUnique(Items const& items) -> bool {
	return items.size() == std::unordered_set<Item>{ items.begin(), items.end() }.size();
}
auto votingIndicesAreSmallerThanNumberOfItems(Votes const& votes, uint32_t const number_of_items) -> bool {
	for (auto const& vote : votes) {
		if (vote.a_idx >= number_of_items || vote.b_idx >= number_of_items) {
			return false;
		}
	}
	return true;
}
auto hasDuplicateMatchups(Votes const& votes) -> bool {
	std::unordered_set<IndexPair, IndexPairHash> index_pair_set{};
	for (auto const& vote : votes) {
		index_pair_set.insert(sortIndexPair({ vote.a_idx, vote.b_idx }));
	}
	return index_pair_set.size() != votes.size();
}
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
auto hasDuplicateItems(Items const& items) -> bool {
	std::unordered_set<Item> item_set{};
	for (auto const& item : items) {
		item_set.insert(item);
	}
	return item_set.size() != items.size();
}
auto hasInvalidScheduledVotes(IndexPairs const& index_pairs, uint32_t number_of_items) -> bool {
	for (auto const& [left, right] : index_pairs) {
		if (left >= number_of_items || right >= number_of_items || left == right) {
			printError("Invalid scheduled votes. Left: " + std::to_string(left) +
				", right: " + std::to_string(right) + ", number of items: " + std::to_string(number_of_items));
			return true;
		}
	}
	return false;
}
auto hasDuplicateScheduledVotes(IndexPairs const& index_pairs) -> bool {
	std::unordered_set<IndexPair, IndexPairHash> index_pair_set{};
	for (auto const& index_pair : index_pairs) {
		index_pair_set.insert(sortIndexPair(index_pair));
	}
	return index_pair_set.size() != index_pairs.size();
}
auto expectedIndexPairs(Items const& items, bool reduced_voting) -> uint32_t {
	if (items.size() < 2) {
		return 0;
	}
	size_t number_of_pairs = sumOfFirstIntegers(items.size() - 1);
	if (reduced_voting && items.size() >= kMinimumItemsForPruning) {
		number_of_pairs -= items.size() * pruningAmount(items.size());
	}
	return static_cast<uint32_t>(number_of_pairs);
}
auto hasVotesWithInvalidIndices(Votes const& votes, uint32_t number_of_items) -> bool {
	for (auto const& vote : votes) {
		if (vote.a_idx >= vote.b_idx || vote.b_idx >= number_of_items) {
			return true;
		}
	}
	return false;
}
auto hasVotesWithInvalidVoteOption(Votes const& votes) -> bool {
	for (auto const& vote : votes) {
		if (vote.winner != Option::A && vote.winner != Option::B) {
			return true;
		}
	}
	return false;
}
auto hasDuplicateVotes(Votes const& votes) -> bool {
	std::unordered_set<IndexPair, IndexPairHash> index_pair_set{};
	for (auto const& vote : votes) {
		index_pair_set.insert({ vote.a_idx, vote.b_idx });
	}
	return index_pair_set.size() != votes.size();
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
auto reduceVotes(IndexPairs const& index_pairs, uint32_t const number_of_items) -> IndexPairs {
	if (number_of_items < kMinimumItemsForPruning) {
		printError("Too few items (" + std::to_string(number_of_items) + ") to reduce voting. No reduction performed");
		return index_pairs;
	}
	return pruneVotes(index_pairs, number_of_items, pruningAmount(number_of_items));
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
		option_a.value(),
		option_b.value(),
		static_cast<Option>(winner.value()) };
}
auto findMaxLength(Items const& items) -> size_t {
	if (items.empty()) {
		return 0;
	}
	return std::max_element(items.begin(), items.end(),
		[](Item const& a, Item const& b) {
			return a.size() < b.size();
		}
	)->size();
}
auto counterString(size_t counter, size_t total) -> std::string {
	size_t const total_length = numberOfDigits(total);
	size_t const counter_length = numberOfDigits(counter);

	return
		"(" + std::string(total_length - counter_length, ' ') + std::to_string(counter) +
		"/" + std::to_string(total) + ")";
}

} // namespace


auto VotingRound::ScoreBased::create(uint32_t const number_of_items, VotingFormat const voting_format) -> std::optional<VotingRound::ScoreBased> {
	if (!(voting_format == VotingFormat::Full || voting_format == VotingFormat::Reduced)) {
		return std::nullopt;
	}

	VotingRound::ScoreBased score_based{};
	score_based.index_pairs_ = generateIndexPairs(number_of_items);
	if (voting_format == VotingFormat::Reduced) {
		score_based.index_pairs_ = reduceVotes(score_based.index_pairs_, number_of_items);
	}
	return score_based;
}
auto VotingRound::ScoreBased::shuffle(std::default_random_engine& engine) -> bool {
	//std::default_random_engine random_engine(seed);
	std::shuffle(index_pairs_.begin(), index_pairs_.end(), engine);
	return true;
}
auto VotingRound::ScoreBased::currentIndexPair(uint32_t const counter) const -> IndexPair {
	return index_pairs_[counter];
}
auto VotingRound::ScoreBased::indexPairs() const -> IndexPairs const& {
	return index_pairs_;
}
auto VotingRound::ScoreBased::numberOfScheduledVotes() const -> uint32_t {
	return static_cast<uint32_t>(index_pairs_.size());
}

auto VotingRound::create(Items const& items, VotingFormat voting_format, Seed seed) -> std::optional<VotingRound> {
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

	// Retain item order, to make seeded item shuffling deterministic
	voting_round.original_items_order_ = items;
	voting_round.items_ = items;
	voting_round.seed_ = (seed == 0 ? generateSeed() : seed);
	voting_round.voting_format_ = voting_format;
	voting_round.score_based_ = ScoreBased::create(static_cast<uint32_t>(items.size()), voting_format);

	return voting_round;
}
auto VotingRound::create(std::vector<std::string> const& lines) -> std::optional<VotingRound> {
	VotingRound voting_round{};
	size_t line_index = 0;

	// Load items
	for (; line_index < lines.size(); line_index++) {
		if (lines[line_index].empty()) {
			break;
		}
		voting_round.items_.emplace_back(lines[line_index]);
	}
	if (voting_round.items_.size() < 2) {
		printError("Parsed items are fewer than two");
		return std::nullopt;
	}
	if (!itemsAreUnique(voting_round.items_)) {
		printError("Parsed items are not unique");
		return std::nullopt;
	}

	// Retain item order, to make seeded item shuffling deterministic
	voting_round.original_items_order_ = voting_round.items_;

	// Skip empty line
	line_index++;

	// Load seed
	if (line_index >= lines.size()) {
		printError("Could not load seed from votes file");
		return std::nullopt;
	}
	std::optional<uint32_t> const seed = parseNumber(lines[line_index++]);
	if (!seed.has_value()) {
		printError("Invalid seed");
		return std::nullopt;
	}
	voting_round.seed_ = seed.value();

	// Load reduced voting round setting
	if (line_index >= lines.size()) {
		printError("Voting format missing");
		return std::nullopt;
	}

	voting_round.voting_format_ = stringToVotingFormat(lines[line_index]);
	if (voting_round.format() == VotingFormat::Invalid) {
		printError("Incorrect voting format: " + lines[line_index]);
		return std::nullopt;
	}
	line_index++;

	voting_round.score_based_ = ScoreBased::create(static_cast<uint32_t>(voting_round.items_.size()), voting_round.format());
	if (!voting_round.score_based_.has_value()) {
		return std::nullopt;
	}

	voting_round.shuffle();

	// Load votes
	for (; line_index < lines.size(); line_index++) {
		auto const vote = parseVote(lines[line_index]);
		if (vote.a_idx == 0 && vote.b_idx == 0) {
			printError("Failed to parse vote");
			return std::nullopt;
		}
		voting_round.votes_.emplace_back(vote);
	}
	if (!votingIndicesAreSmallerThanNumberOfItems(voting_round.votes_, voting_round.items_.size())) {
		printError("Some parsed indices in vote are larger than allowed");
		return std::nullopt;
	}

	if (hasDuplicateMatchups(voting_round.votes_)) {
		printError("Some vote matchups are duplicated");
		return std::nullopt;
	}

	if (voting_round.votes_.size() > voting_round.numberOfScheduledVotes()) {
		printError("Too many votes parsed. Voting round is invalidated");
		return std::nullopt;
	}

	voting_round.is_saved_ = true;
	return voting_round;
}
auto VotingRound::shuffle() -> bool {
	std::default_random_engine random_engine(seed_);
	if (!shuffleImpl(random_engine)) {
		return false;
	}
	std::shuffle(items_.begin(), items_.end(), random_engine);
	return true;
}
auto VotingRound::vote(Option option) -> bool {
	if (!hasRemainingVotes()) {
		return false;
	}
	auto const index_pair = currentIndexPairImpl();
	votes_.emplace_back(index_pair.first, index_pair.second, option);
	is_saved_ = false;
	return true;
}
auto VotingRound::undoVote() -> bool {
	// Need to check since pop_back() decrements vector size even when it's 0, causing a size_t underflow
	if (votes_.empty()) {
		return false;
	}
	votes_.pop_back();
	is_saved_ = false;
	return true;
}
auto VotingRound::save(std::string const& file_name) -> bool {
	if (!saveFile(file_name, convertToText())) {
		return false;
	}
	is_saved_ = true;
	return true;
}
auto VotingRound::items() const->Items const& {
	return items_;
}
auto VotingRound::originalItemOrder() const -> Items const& {
	return original_items_order_;
}
auto VotingRound::format() const->VotingFormat {
	return voting_format_;
}
auto VotingRound::seed() const->Seed {
	return seed_;
}
auto VotingRound::votes() const->Votes const& {
	return votes_;
}
auto VotingRound::isSaved() const -> bool {
	return is_saved_;
}
// TODO: Deprecated in favor of VotingRound::create().
// Either move some of these into VotingRound::create(), or call this from within it.
// auto VotingRound::verify() const -> bool {
// 	// Verify state of items, seed, index pairs, and votes
// 
// 	if (items_.size() < 2) {
// 		printError("At least two items are required");
// 		return false;
// 	}
// 	if (hasDuplicateItems(items_)) {
// 		printError("Duplicate items found");
// 		return false;
// 	}
// 	if (hasInvalidScheduledVotes(index_pairs_, items_.size())) {
// 		printError("Scheduled votes are invalid");
// 		return false;
// 	}
// 	if (hasDuplicateScheduledVotes(index_pairs_)) {
// 		printError("Scheduled votes have duplicates");
// 		return false;
// 	}
// 	if (seed_ == 0) {
// 		printError("Seed is 0");
// 		return false;
// 	}
// 	uint32_t const expected_pairs = expectedIndexPairs(items_, voting_format_ == VotingFormat::Reduced);
// 	if (numberOfScheduledVotes() != expected_pairs) {
// 		printError("Generated pairs: " + std::to_string(index_pairs_.size()) + ". Expected: " + std::to_string(expected_pairs));
// 		return false;
// 	}
// 	if (hasVotesWithInvalidIndices(votes_, items_.size())) {
// 		printError("Invalid indices in votes");
// 		return false;
// 	}
// 	if (hasVotesWithInvalidVoteOption(votes_)) {
// 		printError("Invalid option voted for");
// 		return false;
// 	}
// 	if (hasDuplicateVotes(votes_)) {
// 		printError("Votes are duplicated");
// 		return false;
// 	}
// 	if (votes_.size() > numberOfScheduledVotes()) {
// 		printError("Number of votes exceed maximum amount");
// 		return false;
// 	}
// 	return true;
// }
auto VotingRound::numberOfScheduledVotes() const -> uint32_t {
	return numberOfScheduledVotesImpl();
}
auto VotingRound::currentMatchup() const -> std::optional<Matchup> {
	if (!hasRemainingVotes()) {
		printError("No active matchup");
		return std::nullopt;
	}
	return currentMatchupImpl();
}
auto VotingRound::currentVotingLine() const -> std::optional<std::string> {
	if (!hasRemainingVotes()) {
		printError("Voting finished. No active votes to print");
		return std::nullopt;
	}
	auto const matchup = currentMatchup();
	auto const max_length = findMaxLength(items_);
	auto const length_a = matchup.value().item_a.size();
	auto const length_b = matchup.value().item_b.size();
	auto const padding_length_a = max_length - length_a;
	auto const padding_length_b = max_length - length_b;
	return counterString(votes_.size() + 1, numberOfScheduledVotes()) + " "
		"H: help. "
		"A: \'" + matchup.value().item_a + "\'." + std::string(padding_length_a, ' ') + " "
		"B: \'" + matchup.value().item_b + "\'." + std::string(padding_length_b, ' ') + " Your choice: ";
}
auto VotingRound::hasRemainingVotes() const -> bool {
	return hasRemainingVotesImpl();
}
auto VotingRound::convertToText() const -> std::vector<std::string> {
	std::vector<std::string> lines{};

	// Original item order, to make seeded item shuffling deterministic
	if (original_items_order_.empty() ||
		items_.empty()) {
		printError("Failed to generate voting file data: No items");
		return lines;
	}

	// Items
	for (Item const& item : original_items_order_) {
		lines.emplace_back(item);
	}
	lines.emplace_back("");

	// Seed
	lines.emplace_back(std::to_string(seed_));

	// Voting format
	lines.emplace_back(votingFormatToString(voting_format_));

	// Votes
	for (Vote const& vote : votes_) {
		lines.emplace_back(std::to_string(vote.a_idx) + " " + std::to_string(vote.b_idx) + " " + std::to_string(to_underlying(vote.winner)));
	}
	return lines;
}

auto VotingRound::currentIndexPairImpl() const -> IndexPair {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		return score_based_.value().indexPairs()[votes_.size()];
	case VotingFormat::Invalid:
	default:
		return {};
	}
}
auto VotingRound::shuffleImpl(std::default_random_engine& engine) -> bool {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		if (!score_based_.has_value()) {
			return false;
		}
		return score_based_.value().shuffle(engine);
	case VotingFormat::Invalid:
	default:
		return false;
	}
}
auto VotingRound::hasRemainingVotesImpl() const -> bool {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		if (!score_based_.has_value()) {
			return false;
		}
		return votes_.size() < score_based_.value().numberOfScheduledVotes();
	case VotingFormat::Invalid:
	default:
		return false;
	}
}
auto VotingRound::numberOfScheduledVotesImpl() const -> uint32_t {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		if (!score_based_.has_value()) {
			return 0;
		}
		return score_based_.value().numberOfScheduledVotes();
	case VotingFormat::Invalid:
	default:
		return 0;
	}
}
auto VotingRound::currentMatchupImpl() const -> std::optional<Matchup> {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced: {
		if (!score_based_.has_value()) {
			return std::nullopt;
		}
		auto const index_pair = currentIndexPairImpl();
		return Matchup{ items_[index_pair.first], items_[index_pair.second] };
	}
	case VotingFormat::Invalid:
	default:
		return std::nullopt;
	}
}
