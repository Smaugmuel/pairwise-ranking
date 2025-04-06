#include "calculate_scores.h"
#include "score.h"
#include "testing.h"

namespace
{
auto findScoreForItem(Scores const& scores, std::string const& item) -> Score {
	return *std::find_if(scores.begin(), scores.end(), [&item](Score const& score) {
		return score.item == item;
	});
}

void zeroItemsAndVotes() {
	ASSERT_EQ(calculateScores({}, {}).size(), 0ui64);
}
void zeroVotes() {
	for (size_t number_of_items = 0; number_of_items < 25; number_of_items++) {
		ASSERT_EQ(calculateScores(getNItems(number_of_items), {}).size(), 0ui64);
	}
}
void oneVoteForA() {
	uint64_t const kNumberOfItems{ 2 };
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{ Vote{ 0, 1, Option::A } });
	ASSERT_EQ(scores.size(), kNumberOfItems);
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 1, 0 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 0, 1 });
}
void oneVoteForB() {
	uint64_t const kNumberOfItems{ 2 };
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{ Vote{ 0, 1, Option::B } });
	ASSERT_EQ(scores.size(), kNumberOfItems);
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 0, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 1, 0 });
}
void allVotesForA() {
	uint64_t const kNumberOfItems{ 4 };
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::A },
		Vote{ 0, 2, Option::A },
		Vote{ 0, 3, Option::A },
		Vote{ 1, 2, Option::A },
		Vote{ 1, 3, Option::A },
		Vote{ 2, 3, Option::A },
	});
	ASSERT_EQ(scores.size(), kNumberOfItems);
	
	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 3, 0 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 0, 3 });
}
void allVotesForB() {
	uint64_t const kNumberOfItems{ 4 };
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::B },
		Vote{ 0, 2, Option::B },
		Vote{ 0, 3, Option::B },
		Vote{ 1, 2, Option::B },
		Vote{ 1, 3, Option::B },
		Vote{ 2, 3, Option::B },
	});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 3, 0 });
}
void votingForItem2AndOptionAIfNotPresent() {
	uint64_t const kNumberOfItems{ 4 };
	// ASSUMPTION: With getNItems(), item2 will be the second element (index 1)
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::B },
		Vote{ 0, 2, Option::A },
		Vote{ 0, 3, Option::A },
		Vote{ 1, 2, Option::A },
		Vote{ 1, 3, Option::A },
		Vote{ 2, 3, Option::A },
		});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 0, 3 });
}
void votingForItem3AndOptionAIfNotPresent() {
	uint64_t const kNumberOfItems{ 4 };
	// ASSUMPTION: With getNItems(), item3 will be the third element (index 2)
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::A },
		Vote{ 0, 2, Option::B },
		Vote{ 0, 3, Option::A },
		Vote{ 1, 2, Option::B },
		Vote{ 1, 3, Option::A },
		Vote{ 2, 3, Option::A },
		});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 0, 3 });
}
void votingForItem2AndOptionBIfNotPresent() {
	uint64_t const kNumberOfItems{ 4 };
	// ASSUMPTION: With getNItems(), item2 will be the second element (index 1)
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::B },
		Vote{ 0, 2, Option::B },
		Vote{ 0, 3, Option::B },
		Vote{ 1, 2, Option::A },
		Vote{ 1, 3, Option::A },
		Vote{ 2, 3, Option::B },
		});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 2, 1 });
}
void votingForItem3AndOptionBIfNotPresent() {
	uint64_t const kNumberOfItems{ 4 };
	// ASSUMPTION: With getNItems(), item3 will be the third element (index 2)
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::B },
		Vote{ 0, 2, Option::B },
		Vote{ 0, 3, Option::B },
		Vote{ 1, 2, Option::B },
		Vote{ 1, 3, Option::B },
		Vote{ 2, 3, Option::A },
		});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 2, 1 });
}
void votingForItem2AndAgainstItem3AndOptionAIfNeither() {
	uint64_t const kNumberOfItems{ 4 };
	// ASSUMPTION: With getNItems(), item2 and item3 will be the second and third element respectively (index 1 and 2)
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::B },
		Vote{ 0, 2, Option::A },
		Vote{ 0, 3, Option::A },
		Vote{ 1, 2, Option::A },
		Vote{ 1, 3, Option::A },
		Vote{ 2, 3, Option::B },
		});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 3, 0 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 0, 3 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 1, 2 });
}
void votingForItem2AndAgainstItem3AndOptionBIfNeither() {
	uint64_t const kNumberOfItems{ 4 };
	// ASSUMPTION: With getNItems(), item2 and item3 will be the second and third element respectively (index 1 and 2)
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::B },
		Vote{ 0, 2, Option::A },
		Vote{ 0, 3, Option::B },
		Vote{ 1, 2, Option::A },
		Vote{ 1, 3, Option::A },
		Vote{ 2, 3, Option::B },
		});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 3, 0 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 0, 3 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 2, 1 });
}
void votingForItem3AndAgainstItem2AndOptionAIfNeither() {
	uint64_t const kNumberOfItems{ 4 };
	// ASSUMPTION: With getNItems(), item2 and item3 will be the second and third element respectively (index 1 and 2)
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::A },
		Vote{ 0, 2, Option::B },
		Vote{ 0, 3, Option::A },
		Vote{ 1, 2, Option::B },
		Vote{ 1, 3, Option::B },
		Vote{ 2, 3, Option::A },
		});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 2, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 1, 2 });
}
void votingForItem3AndAgainstItem2AndOptionBIfNeither() {
	uint64_t const kNumberOfItems{ 4 };
	// ASSUMPTION: With getNItems(), item2 and item3 will be the second and third element respectively (index 1 and 2)
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::A },
		Vote{ 0, 2, Option::B },
		Vote{ 0, 3, Option::B },
		Vote{ 1, 2, Option::B },
		Vote{ 1, 3, Option::B },
		Vote{ 2, 3, Option::A },
		});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 3, 0 });

	// ASSUMPTION: In each matchup, the earlier item will be option A (left)
	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 1, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 2, 1 });
}
void votingForOptionAButNotFullRound() {
	uint64_t const kNumberOfItems{ 4 };
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::A },
		Vote{ 0, 2, Option::A },
		Vote{ 0, 3, Option::A },
		Vote{ 1, 2, Option::A },
		});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 3, 0 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 1, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 0, 2 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 0, 1 });
}
void votingForOptionBButNotFullRound() {
	uint64_t const kNumberOfItems{ 4 };
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::B },
		Vote{ 0, 2, Option::B },
		Vote{ 0, 3, Option::B },
		Vote{ 1, 2, Option::B },
		});
	ASSERT_EQ(scores.size(), kNumberOfItems);

	ASSERT_EQ(findScoreForItem(scores, "item1"), Score{ "item1", 0, 3 });
	ASSERT_EQ(findScoreForItem(scores, "item2"), Score{ "item2", 1, 1 });
	ASSERT_EQ(findScoreForItem(scores, "item3"), Score{ "item3", 2, 0 });
	ASSERT_EQ(findScoreForItem(scores, "item4"), Score{ "item4", 1, 0 });
}
void tooFewVotesToScoreAllItems() {
	uint64_t const kNumberOfItems{ 5 };
	auto const scores = calculateScores(getNItems(kNumberOfItems), Votes{
		Vote{ 0, 1, Option::A },
		Vote{ 0, 2, Option::A },
		Vote{ 0, 3, Option::A },
		});
	ASSERT_TRUE(scores.size() < kNumberOfItems);
}

auto run_tests(std::string const& test) -> bool {
	RUN_TEST_IF_ARGUMENT_EQUALS(zeroItemsAndVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(zeroVotes);
	RUN_TEST_IF_ARGUMENT_EQUALS(oneVoteForA);
	RUN_TEST_IF_ARGUMENT_EQUALS(oneVoteForB);
	RUN_TEST_IF_ARGUMENT_EQUALS(allVotesForA);
	RUN_TEST_IF_ARGUMENT_EQUALS(allVotesForB);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForItem2AndOptionAIfNotPresent);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForItem3AndOptionAIfNotPresent);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForItem2AndOptionBIfNotPresent);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForItem3AndOptionBIfNotPresent);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForItem2AndAgainstItem3AndOptionAIfNeither);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForItem2AndAgainstItem3AndOptionBIfNeither);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForItem3AndAgainstItem2AndOptionAIfNeither);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForItem3AndAgainstItem2AndOptionBIfNeither);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForOptionAButNotFullRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(votingForOptionBButNotFullRound);
	RUN_TEST_IF_ARGUMENT_EQUALS(tooFewVotesToScoreAllItems);
	return true;
}

} // namespace

auto test_calculate_scores(std::string const& test_case) -> int {
	if (run_tests(test_case)) {
		return 1;
	}
	return 0;
}
