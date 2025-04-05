#include <filesystem>
#include <fstream>

#include "helpers.h"
#include "testing.h"
#include "score_helpers.h"

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
void endToEnd_combineFromTwoFiles() {
	constexpr char const* kScoresFile1 = "scores_1.txt";
	constexpr char const* kScoresFile2 = "scores_2.txt";
	constexpr char const* kCombinedFile = "combined_scores.txt";
	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kScoresFile2);
	std::filesystem::remove(kCombinedFile);

	ASSERT_TRUE(saveScores({ { "item1", 3, 0 }, { "item2", 2, 1 }, { "item3", 1, 2 }, { "item4", 0, 3 } }, kScoresFile1));
	ASSERT_TRUE(saveScores({ { "item1", 1, 2 }, { "item2", 0, 3 }, { "item3", 3, 0 }, { "item4", 2, 1 } }, kScoresFile2));

	ASSERT_TRUE(combine(std::string{ kScoresFile1 } + ' ' + kScoresFile2, kCombinedFile));
	ASSERT_TRUE(std::filesystem::exists(kCombinedFile));

	Scores const combined_scores = parseScores(loadFile(kCombinedFile));
	ASSERT_EQ(combined_scores, Scores{ { "item1", 4, 2 }, { "item3", 4, 2 }, { "item2", 2, 4 }, { "item4", 2, 4 } });

	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kScoresFile2);
	std::filesystem::remove(kCombinedFile);
}
void endToEnd_combineFromThreeFiles() {
	constexpr char const* kScoresFile1 = "scores_1.txt";
	constexpr char const* kScoresFile2 = "scores_2.txt";
	constexpr char const* kScoresFile3 = "scores_3.txt";
	constexpr char const* kCombinedFile = "combined_scores.txt";
	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kScoresFile2);
	std::filesystem::remove(kScoresFile3);
	std::filesystem::remove(kCombinedFile);

	ASSERT_TRUE(saveScores({ { "item1", 3, 0 }, { "item2", 2, 1 }, { "item3", 1, 2 }, { "item4", 0, 3 } }, kScoresFile1));
	ASSERT_TRUE(saveScores({ { "item1", 1, 2 }, { "item2", 0, 3 }, { "item3", 3, 0 }, { "item4", 2, 1 } }, kScoresFile2));
	ASSERT_TRUE(saveScores({ { "item2", 2, 1 }, { "item4", 3, 0 }, { "item5", 1, 2 }, { "item6", 0, 3 } }, kScoresFile3));

	ASSERT_TRUE(combine(std::string{ kScoresFile1 } + ' ' + kScoresFile2 + ' ' + kScoresFile3, kCombinedFile));
	ASSERT_TRUE(std::filesystem::exists(kCombinedFile));

	Scores const combined_scores = parseScores(loadFile(kCombinedFile));
	ASSERT_EQ(combined_scores, Scores{ { "item1", 4, 2 }, { "item3", 4, 2 }, { "item4", 5, 4 }, { "item2", 4, 5 }, { "item5", 1, 2 }, { "item6", 0, 3 } });

	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kScoresFile2);
	std::filesystem::remove(kScoresFile3);
	std::filesystem::remove(kCombinedFile);
}
void endToEnd_combineFromTooFewFiles() {
	constexpr char const* kScoresFile1 = "scores_1.txt";
	constexpr char const* kCombinedFile = "combined_scores.txt";
	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kCombinedFile);

	ASSERT_TRUE(saveScores({ { "item1", 3, 0 }, { "item2", 2, 1 }, { "item3", 1, 2 }, { "item4", 0, 3 } }, kScoresFile1));

	ASSERT_FALSE(combine(std::string{ kScoresFile1 }, kCombinedFile));
	ASSERT_FALSE(std::filesystem::exists(kCombinedFile));

	std::filesystem::remove(kScoresFile1);
}
void endToEnd_combineFromInvalidFileName() {
	constexpr char const* kScoresFile1 = "scores_1.txt";
	constexpr char const* kScoresFile2 = "scores_2.txt";
	constexpr char const* kScoresFile3 = "scores_3.txt";
	constexpr char const* kCombinedFile = "combined_scores.txt";
	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kScoresFile2);
	std::filesystem::remove(kScoresFile3);
	std::filesystem::remove(kCombinedFile);

	ASSERT_TRUE(saveScores({ { "item1", 3, 0 }, { "item2", 2, 1 }, { "item3", 1, 2 }, { "item4", 0, 3 } }, kScoresFile1));
	ASSERT_TRUE(saveScores({ { "item1", 1, 2 }, { "item2", 0, 3 }, { "item3", 3, 0 }, { "item4", 2, 1 } }, kScoresFile2));
	ASSERT_TRUE(saveScores({ { "item2", 2, 1 }, { "item4", 3, 0 }, { "item5", 1, 2 }, { "item6", 0, 3 } }, kScoresFile3));

	ASSERT_FALSE(combine(std::string{ kScoresFile1 } + " invalid_name.txt " + kScoresFile3, kCombinedFile));
	ASSERT_FALSE(std::filesystem::exists(kCombinedFile));

	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kScoresFile2);
	std::filesystem::remove(kScoresFile3);
}
void endToEnd_combineFromAllEmptyFiles() {
	constexpr char const* kScoresFile1 = "scores_1.txt";
	constexpr char const* kScoresFile2 = "scores_2.txt";
	constexpr char const* kCombinedFile = "combined_scores.txt";
	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kScoresFile2);
	std::filesystem::remove(kCombinedFile);

	std::ofstream file1(kScoresFile1);
	file1.close();
	std::ofstream file2(kScoresFile2);
	file2.close();

	ASSERT_FALSE(combine(std::string{ kScoresFile1 } + ' ' + kScoresFile2, kCombinedFile));
	ASSERT_FALSE(std::filesystem::exists(kCombinedFile));

	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kScoresFile2);
}
void endToEnd_combineFromOneEmptyFile() {
	constexpr char const* kScoresFile1 = "scores_1.txt";
	constexpr char const* kScoresFile2 = "scores_2.txt";
	constexpr char const* kCombinedFile = "combined_scores.txt";
	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kScoresFile2);
	std::filesystem::remove(kCombinedFile);

	ASSERT_TRUE(saveScores({ { "item1", 3, 0 }, { "item2", 2, 1 }, { "item3", 1, 2 }, { "item4", 0, 3 } }, kScoresFile1));
	std::ofstream file(kScoresFile2);
	file.close();

	ASSERT_TRUE(combine(std::string{ kScoresFile1 } + ' ' + kScoresFile2, kCombinedFile));
	ASSERT_TRUE(std::filesystem::exists(kCombinedFile));

	Scores const combined_scores = parseScores(loadFile(kCombinedFile));
	ASSERT_EQ(combined_scores, Scores{ { "item1", 3, 0 }, { "item2", 2, 1 }, { "item3", 1, 2 }, { "item4", 0, 3 } });

	std::filesystem::remove(kScoresFile1);
	std::filesystem::remove(kScoresFile2);
	std::filesystem::remove(kCombinedFile);
}

} // namespace

auto run_tests(char* argv[]) -> int {
	RUN_TEST_IF_ARGUMENT_EQUALS(combiningNoScoreSet);
	RUN_TEST_IF_ARGUMENT_EQUALS(combiningOneScoreSetWithZeroScores);
	RUN_TEST_IF_ARGUMENT_EQUALS(combiningOneScoreSetWithOneScore);
	RUN_TEST_IF_ARGUMENT_EQUALS(combiningOneScoreSetWithMultipleScores);
	RUN_TEST_IF_ARGUMENT_EQUALS(combiningTwoScoreSetsWhenOneScoreSetIsEmpty);
	RUN_TEST_IF_ARGUMENT_EQUALS(combiningTwoScoreSetsWithSameItem);
	RUN_TEST_IF_ARGUMENT_EQUALS(combiningTwoScoreSetsWithDifferentItems);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_combineFromTwoFiles);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_combineFromThreeFiles);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_combineFromTooFewFiles);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_combineFromInvalidFileName);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_combineFromAllEmptyFiles);
	RUN_TEST_IF_ARGUMENT_EQUALS(endToEnd_combineFromOneEmptyFile);
	return 1;
}

int main(int argc, char* argv[]) {
	ASSERT_EQ(argc, 2);
	if (run_tests(argv) != 0) {
		return 1;
	}
	return 0;
}
