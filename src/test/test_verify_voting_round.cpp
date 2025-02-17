#include "functions.h"
#include "testing.h"

namespace
{

/* -------------- Test helpers -------------- */
auto getNItems(size_t n) -> Items {
	Items items;
	items.reserve(n);
	for (size_t i = 0; i < n; i++) {
		items.push_back("item" + std::to_string(i + 1));
	}
	return items;
}

/* -------------- Tests -------------- */
void newVotingRoundWithFullVoting() {
	for (size_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		ASSERT_TRUE(verifyVotingRound(generateNewVotingRound(getNItems(number_of_items), false).value()));
	}
}
void newVotingRoundWithPrunedVoting() {
	for (size_t number_of_items = 2; number_of_items < 25; number_of_items++) {
		ASSERT_TRUE(verifyVotingRound(generateNewVotingRound(getNItems(number_of_items), true).value()));
	}
}
void votingRoundWithTooFewItems() {
	auto voting_round = generateNewVotingRound({ "item1", "item2" }, false).value();
	voting_round.items.pop_back();
	ASSERT_FALSE(verifyVotingRound(voting_round));
}
void votingRoundWithDuplicateItems() {
	for (auto prune_votes : { true, false }) {
		for (size_t number_of_items = 2; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes).value();
			voting_round.items.pop_back();
			voting_round.items.emplace_back("item1");
			ASSERT_FALSE(verifyVotingRound(voting_round));
		}
	}
}
void votingRoundWithTooFewScheduledVotes() {
	for (auto prune_votes : { true, false }) {
		for (size_t number_of_items = 2; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes).value();
			voting_round.index_pairs.pop_back();
			ASSERT_FALSE(verifyVotingRound(voting_round));
		}
	}
}
void votingRoundWithTooManyScheduledVotes() {
	for (auto prune_votes : { true, false }) {
		for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes).value();
			voting_round.index_pairs.emplace_back(0, number_of_items);
			ASSERT_FALSE(verifyVotingRound(voting_round));
		}
	}
}
void votingRoundWithScheduledVotesIndicesOutOfRange() {
	for (auto prune_votes : { true, false }) {
		for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes).value();
			voting_round.index_pairs[number_of_items / 2 - 1] = { 0, number_of_items };
			ASSERT_FALSE(verifyVotingRound(voting_round));
		}
	}
}
void votingRoundWithScheduledVotesIndicesBeingEqual() {
	for (auto prune_votes : { true, false }) {
		for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes).value();
			voting_round.index_pairs[number_of_items / 2 - 1] = { number_of_items - 1, number_of_items - 1 };
			ASSERT_FALSE(verifyVotingRound(voting_round));
		}
	}
}
void votingRoundWithDuplicateScheduledVotes() {
	for (auto prune_votes : { true, false }) {
		for (uint32_t number_of_items = 3; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes).value();
			voting_round.index_pairs[0] = voting_round.index_pairs.back();
			ASSERT_FALSE(verifyVotingRound(voting_round));
		}
	}
}
void votingRoundWithInvalidSeed() {
	auto voting_round = generateNewVotingRound(getNItems(10), false).value();
	voting_round.seed = 0;
	ASSERT_FALSE(verifyVotingRound(voting_round));
}
void votingRoundWithValidVotes() {
	for (auto prune_votes : { true, false }) {
		for (size_t number_of_items = 2; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes);
			vote(voting_round, Option::A);
			ASSERT_TRUE(verifyVotingRound(voting_round.value()));
		}
	}
}
void votingRoundWithCompletedVoting() {
	for (auto prune_votes : { true, false }) {
		for (size_t number_of_items = 2; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes);
			for (size_t i = 0; i < numberOfScheduledVotes(voting_round.value()); i++) {
				vote(voting_round, static_cast<Option>(i % 2));
			}
			ASSERT_TRUE(verifyVotingRound(voting_round.value()));
		}
	}
}
void votingRoundWithTooManyVotes() {
	for (auto prune_votes : { true, false }) {
		for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes);
			for (size_t i = 0; i < numberOfScheduledVotes(voting_round.value()); i++) {
				vote(voting_round, static_cast<Option>(i % 2));
			}
			voting_round.value().votes.emplace_back(IndexPair{ 0, number_of_items }, Option::A);
			ASSERT_FALSE(verifyVotingRound(voting_round.value()));
		}
	}
}
void votingRoundWithInvalidVoteIndices() {
	for (auto prune_votes : { true, false }) {
		for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes);
			voting_round.value().votes.emplace_back(IndexPair{ 0, number_of_items }, Option::A);
			ASSERT_FALSE(verifyVotingRound(voting_round.value()));
		}
	}
}
void votingRoundWithInvalidVoteOption() {
	for (auto prune_votes : { true, false }) {
		for (uint32_t number_of_items = 2; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes);
			voting_round.value().votes.emplace_back(IndexPair{ 0, number_of_items - 1 }, static_cast<Option>(2));
			ASSERT_FALSE(verifyVotingRound(voting_round.value()));
		}
	}
}
void votingRoundWithDuplicateVotes() {
	for (auto prune_votes : { true, false }) {
		for (size_t number_of_items = 3; number_of_items < 25; number_of_items++) {
			auto voting_round = generateNewVotingRound(getNItems(number_of_items), prune_votes);
			vote(voting_round, Option::B);
			voting_round.value().votes.emplace_back(voting_round.value().votes[0].index_pair, Option::A);
			ASSERT_FALSE(verifyVotingRound(voting_round.value()));
		}
	}
}

}

int main() {
	newVotingRoundWithFullVoting();
	newVotingRoundWithPrunedVoting();
	votingRoundWithTooFewItems();
	votingRoundWithDuplicateItems();
	votingRoundWithTooFewScheduledVotes();
	votingRoundWithTooManyScheduledVotes();
	votingRoundWithScheduledVotesIndicesOutOfRange();
	votingRoundWithScheduledVotesIndicesBeingEqual();
	votingRoundWithDuplicateScheduledVotes();
	votingRoundWithInvalidSeed();
	votingRoundWithValidVotes();
	votingRoundWithCompletedVoting();
	votingRoundWithTooManyVotes();
	votingRoundWithInvalidVoteIndices();
	votingRoundWithInvalidVoteOption();
	votingRoundWithDuplicateVotes();
	return 0;
}