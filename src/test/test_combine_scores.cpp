#include "functions.h"
#include "testing.h"

namespace
{
void combiningNoScoreSet() {
	ASSERT_EQ(combineScores({}).size(), 0ui64);
}
void combiningOneScoreSetWithZeroScores() {
	ASSERT_EQ(combineScores({ Scores{} }).size(), 0ui64);
}
void combiningOneScoreSetWithOneScore() {
	Score const score{ "item1", 12, 23 };
	Scores const combined_scores = combineScores({ { score } });
	ASSERT_EQ(combined_scores.size(), 1ui64);
	ASSERT_EQ(combined_scores[0], score);
}
void combiningOneScoreSetWithMultipleScores() {
	Score const score1{ "item1", 12, 23 };
	Score const score2{ "item2", 5, 3 };
	Scores const combined_scores = combineScores({ { score1, score2 } });
	ASSERT_EQ(combined_scores.size(), 2ui64);
	ASSERT_EQ(combined_scores[0], score1);
	ASSERT_EQ(combined_scores[1], score2);
}
void combiningTwoScoreSetsWhenOneScoreSetIsEmpty() {
	Score const score1{ "item1", 12, 23 };
	Scores const combined_scores = combineScores({ { score1 }, {}});
	ASSERT_EQ(combined_scores.size(), 1ui64);
	ASSERT_EQ(combined_scores[0], score1);
}
void combiningTwoScoreSetsWithSameItem() {
	Score const score1{ "item1", 12, 23 };
	Score const score2{ "item1", 4, 1 };
	Scores const combined_scores = combineScores({ { score1 }, { score2 }});
	ASSERT_EQ(combined_scores.size(), 1ui64);
	ASSERT_EQ(combined_scores[0], Score{score1.item, score1.wins + score2.wins, score1.losses + score2.losses});
}
void combiningTwoScoreSetsWithDifferentItems() {
	Score const score1{ "item1", 12, 23 };
	Score const score2{ "item2", 4, 1 };
	Scores const combined_scores = combineScores({ { score1 }, { score2 } });
	ASSERT_EQ(combined_scores.size(), 2ui64);
	ASSERT_EQ(combined_scores[0], score1);
	ASSERT_EQ(combined_scores[1], score2);
}

}

int main() {
	combiningNoScoreSet();
	combiningOneScoreSetWithZeroScores();
	combiningOneScoreSetWithOneScore();
	combiningOneScoreSetWithMultipleScores();
	combiningTwoScoreSetsWhenOneScoreSetIsEmpty();
	combiningTwoScoreSetsWithSameItem();
	combiningTwoScoreSetsWithDifferentItems();
	return 0;
}