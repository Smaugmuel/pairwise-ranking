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
	size_t const max_length = std::max(total_length, counter_length);

	return
		"(" + std::string(max_length - counter_length, ' ') + std::to_string(counter) +
		"/" + std::string(max_length - total_length, ' ') + std::to_string(total) + ")";
}
auto counterString(size_t counter, std::string const& total_str) -> std::string {
	size_t const total_length = total_str.size();
	size_t const counter_length = numberOfDigits(counter);
	size_t const max_length = std::max(total_length, counter_length);

	return
		"(" + std::string(max_length - counter_length, ' ') + std::to_string(counter) +
		"/" + std::string(max_length - total_length, ' ') + total_str + ")";
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
auto VotingRound::ScoreBased::shuffle(Seed const seed) -> bool {
	std::default_random_engine engine(seed);
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

auto VotingRound::RankBased::create() -> std::optional<VotingRound::RankBased> {
	VotingRound::RankBased rank_based{};
	rank_based.number_of_sorted_items_ = 1;
	rank_based.end_index_ = rank_based.number_of_sorted_items_;
	return rank_based;
}
void VotingRound::RankBased::vote(Items& items, Option option) {
	uint32_t const mid_index = (start_index_ + end_index_) / 2;
	if (option == Option::A) {
		start_index_ = mid_index + 1;
	}
	else {
		end_index_ = mid_index;
	}

	// Check if the insertion index is known
	if (start_index_ < end_index_) {
		return;
	}
	uint32_t const insertion_index = start_index_;
	Item const item_to_insert = items[number_of_sorted_items_];

	// Move each item in [insertion_index, number_of_sorted_items_) one position down the vector
	for (size_t i = number_of_sorted_items_; i > insertion_index; i--) {
		items[i] = items[i - 1];
	}

	items[insertion_index] = item_to_insert;
	number_of_sorted_items_++;
	start_index_ = 0;
	end_index_ = number_of_sorted_items_;
}
auto VotingRound::RankBased::undoVote(Items& items, Votes const& votes) -> bool {
	if (number_of_sorted_items_ == 1 || votes.empty()) {
		return false;
	}
	// Check if the latest vote resulted in inserting an item
	if (start_index_ == 0 && end_index_ == number_of_sorted_items_) {
		// Check votes in reverse to find the first vote containing the previously inserted item.
		// From that first vote, the succeeding votes will be used to recreate start- and end index
		// up until right before the latest vote was performed.
		auto const previously_inserted_item_index = votes.back().b_idx;
		auto previously_inserted_item_first_vote_index = 0ui32;
		for (int32_t vote_index = votes.size() - 1; vote_index >= 0; vote_index--) {
			if (votes[vote_index].b_idx != previously_inserted_item_index) {
				previously_inserted_item_first_vote_index = vote_index + 1;
				break;
			}
		}

		// Reset internal state to what it was at the first vote containing the previously inserted item
		number_of_sorted_items_--;
		start_index_ = 0;
		end_index_ = number_of_sorted_items_;

		// Simulate voting to set the start- and end indices correctly
		for (uint32_t vote_index = previously_inserted_item_first_vote_index; vote_index < votes.size() - 1; vote_index++) {
			uint32_t const mid_index = (start_index_ + end_index_) / 2;
			if (votes[vote_index].winner == Option::A) {
				start_index_ = mid_index + 1;
			}
			else {
				end_index_ = mid_index;
			}
		}

		// Determine the index where the item was inserted.
		// Start index should now be equal to end index
		uint32_t const mid_index = (start_index_ + end_index_) / 2;
		// if (votes[votes.size() - 1].winner == Option::A) {
		// 	start_index_ = mid_index + 1;
		// }
		// else {
		// 	end_index_ = mid_index;
		// }
		uint32_t const insertion_index = mid_index + (votes.back().winner == Option::A ? 1 : 0);

		// Move each sorted item starting at the insertion index back one position,
		// and move the previously inserted item to right outside the sorted items
		Item const previously_inserted_item = items[insertion_index];
		for (uint32_t item_index = insertion_index; item_index < number_of_sorted_items_; item_index++) {
			items[item_index] = items[item_index + 1];
		}
		items[number_of_sorted_items_] = previously_inserted_item;

		// Start index and end index should now have the correct values

		return true;
	}

	// Check votes in reverse to find the first vote containing the previously inserted item.
	// From that first vote, the succeeding votes will be used to recreate start- and end index
	// up until right before the latest vote was performed.
	auto const previously_inserted_item_index = votes.back().b_idx;
	auto previously_inserted_item_first_vote_index = 0ui32;
	for (int32_t vote_index = votes.size() - 1; vote_index >= 0; vote_index--) {
		if (votes[vote_index].b_idx != previously_inserted_item_index) {
			previously_inserted_item_first_vote_index = vote_index + 1;
			break;
		}
	}

	// Reset internal state to what it was at the first vote containing the previously inserted item
	start_index_ = 0;
	end_index_ = number_of_sorted_items_;

	// Simulate voting to set the start- and end indices correctly
	for (uint32_t vote_index = previously_inserted_item_first_vote_index; vote_index < votes.size() - 1; vote_index++) {
		uint32_t const mid_index = (start_index_ + end_index_) / 2;
		if (votes[vote_index].winner == Option::A) {
			start_index_ = mid_index + 1;
		}
		else {
			end_index_ = mid_index;
		}
	}
	return true;
}
auto VotingRound::RankBased::currentIndexPair() const -> IndexPair {
	uint32_t const mid_index = (start_index_ + end_index_) / 2;
	return { mid_index, number_of_sorted_items_ };
}
auto VotingRound::RankBased::numberOfSortedItems() const->uint32_t {
	return number_of_sorted_items_;
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

	if (!voting_round.createFormatImpl()) {
		return std::nullopt;
	}

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
	if (voting_round.items().size() < 2) {
		printError("Parsed items are fewer than two");
		return std::nullopt;
	}
	if (!itemsAreUnique(voting_round.items())) {
		printError("Parsed items are not unique");
		return std::nullopt;
	}

	// Retain item order, to make seeded item shuffling deterministic
	voting_round.original_items_order_ = voting_round.items();

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

	// Load votes
	for (; line_index < lines.size(); line_index++) {
		auto const vote = parseVote(lines[line_index]);
		if (vote.a_idx == 0 && vote.b_idx == 0) {
			printError("Failed to parse vote");
			return std::nullopt;
		}
		voting_round.votes_.emplace_back(vote);
	}
	if (!votingIndicesAreSmallerThanNumberOfItems(voting_round.votes(), voting_round.items().size())) {
		printError("Some parsed indices in vote are larger than allowed");
		return std::nullopt;
	}

	if (hasDuplicateMatchups(voting_round.votes())) {
		printError("Some vote matchups are duplicated");
		return std::nullopt;
	}

	if (!voting_round.createFormatImpl()) {
		return std::nullopt;
	}

	switch (voting_round.format()) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		if (voting_round.votes().size() > voting_round.numberOfScheduledVotes()) {
			printError("Too many votes parsed. Voting round is invalidated");
			return std::nullopt;
		}
		break;
	case VotingFormat::Ranked:
	case VotingFormat::Invalid:
	default:
		break;
	}

	voting_round.shuffle();

	switch (voting_round.format()) {
	case VotingFormat::Ranked:
		for (auto const& vote : voting_round.votes()) {
			voting_round.rank_based_.value().vote(voting_round.items_, vote.winner);
		}
		break;
	case VotingFormat::Full:
	case VotingFormat::Reduced:
	case VotingFormat::Invalid:
	default:
		break;
	}

	voting_round.is_saved_ = true;
	return voting_round;
}
auto VotingRound::shuffle() -> bool {
	std::default_random_engine random_engine(seed_);
	std::shuffle(items_.begin(), items_.end(), random_engine);
	return shuffleImpl();
}
auto VotingRound::vote(Option option) -> bool {
	if (!hasRemainingVotes()) {
		return false;
	}
	auto const index_pair = currentIndexPairImpl();
	votes_.emplace_back(index_pair.first, index_pair.second, option);
	voteImpl(option);
	is_saved_ = false;
	return true;
}
auto VotingRound::undoVote() -> bool {
	// Need to check since pop_back() decrements vector size even when it's 0, causing a size_t underflow
	if (votes_.empty()) {
		return false;
	}

	if (!undoVoteImpl()) {
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
auto VotingRound::numberOfSortedItems() const -> uint32_t {
	return numberOfSortedItemsImpl();
}
auto VotingRound::numberOfScheduledVotes() const -> uint32_t {
	return numberOfScheduledVotesImpl();
}
auto VotingRound::currentMatchup() const -> std::optional<Matchup> {
	if (!hasRemainingVotes()) {
		printError("No active matchup");
		return std::nullopt;
	}
	IndexPair const index_pair = currentIndexPairImpl();
	if (index_pair == IndexPair{ 0, 0 }) {
		return std::nullopt;
	}
	return Matchup{ items_[index_pair.first], items_[index_pair.second] };
}
auto VotingRound::currentVotingLine() const -> std::optional<std::string> {
	if (!hasRemainingVotes()) {
		printError("Voting finished. No active votes to print");
		return std::nullopt;
	}
	auto const matchup = currentMatchup();
	if (!matchup.has_value()) {
		printError("No matchup");
		return std::nullopt;
	}

	auto const max_length = findMaxLength(items_);
	auto const length_a = matchup.value().item_a.size();
	auto const length_b = matchup.value().item_b.size();
	auto const padding_length_a = max_length - length_a;
	auto const padding_length_b = max_length - length_b;

	return counterStringImpl() + " "
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

auto VotingRound::counterStringImpl() const -> std::string {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		return counterString(votes_.size() + 1, numberOfScheduledVotes());
	case VotingFormat::Ranked:
		return counterString(votes_.size() + 1, '~' + std::to_string(static_cast<uint32_t>(items_.size() * std::log2(items_.size()))));
	case VotingFormat::Invalid:
	default:
		return {};
	}
}
auto VotingRound::voteImpl(Option option) -> bool {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		return true;
	case VotingFormat::Ranked:
		if (!rank_based_.has_value()) {
			return false;
		}
		rank_based_.value().vote(items_, option);
		return true;
	case VotingFormat::Invalid:
	default:
		return false;
	}
}
auto VotingRound::undoVoteImpl() -> bool {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		return true;
	case VotingFormat::Ranked:
		if (!rank_based_.has_value()) {
			return false;
		}
		return rank_based_.value().undoVote(items_, votes_);
	case VotingFormat::Invalid:
	default:
		return false;
	}
}
auto VotingRound::createFormatImpl() -> bool {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		score_based_ = ScoreBased::create(static_cast<uint32_t>(items_.size()), voting_format_);
		return score_based_.has_value();
	case VotingFormat::Ranked:
		rank_based_ = RankBased::create();
		return rank_based_.has_value();
	case VotingFormat::Invalid:
	default:
		return false;
	}
}
auto VotingRound::currentIndexPairImpl() const -> IndexPair {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		return score_based_.value().indexPairs()[votes_.size()];
	case VotingFormat::Ranked:
		return rank_based_.value().currentIndexPair();
	case VotingFormat::Invalid:
	default:
		return {};
	}
}
auto VotingRound::shuffleImpl() -> bool {
	switch (voting_format_) {
	case VotingFormat::Full:
	case VotingFormat::Reduced:
		if (!score_based_.has_value()) {
			return false;
		}
		return score_based_.value().shuffle(seed_);
	case VotingFormat::Ranked:
		return true;
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
	case VotingFormat::Ranked:
		if (!rank_based_.has_value()) {
			return false;
		}
		return rank_based_.value().numberOfSortedItems() < items_.size();
	case VotingFormat::Invalid:
	default:
		return false;
	}
}
auto VotingRound::numberOfSortedItemsImpl() const -> uint32_t {
	switch (voting_format_) {
	case VotingFormat::Ranked:
		if (!rank_based_.has_value()) {
			return 0;
		}
		return rank_based_.value().numberOfSortedItems();
	case VotingFormat::Full:
	case VotingFormat::Reduced:
	case VotingFormat::Invalid:
	default:
		return 0;
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
	case VotingFormat::Ranked:
	case VotingFormat::Invalid:
	default:
		return 0;
	}
}
