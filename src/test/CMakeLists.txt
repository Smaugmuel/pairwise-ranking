cmake_minimum_required(VERSION 3.15)

enable_testing()

function(addTestSuite suite_name dependencies cases)
	add_executable(${suite_name}
		${suite_name}.cpp
		testing.cpp
		${dependencies}
	)
	target_include_directories(${suite_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
	foreach (test_case ${cases})
		add_test(NAME "${suite_name}/${test_case}" COMMAND ${suite_name} ${test_case})
	endforeach()
	unset(test_dependencies PARENT_SCOPE)
	unset(test_cases PARENT_SCOPE)
endfunction()


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/calculate_scores.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
)
list(APPEND test_cases
	zeroVotes
	oneVoteForA
	oneVoteForB
	votingForItem2AndOptionAIfNotPresent
	votingForItem3AndOptionAIfNotPresent
	votingForItem2AndOptionBIfNotPresent
	votingForItem3AndOptionBIfNotPresent
	votingForItem2AndAgainstItem3AndOptionAIfNeither
	votingForItem2AndAgainstItem3AndOptionBIfNeither
	votingForItem3AndAgainstItem2AndOptionAIfNeither
	votingForItem3AndAgainstItem2AndOptionBIfNeither
	votingForOptionAButNotFullRound
	votingForOptionBButNotFullRound
	tooFewVotesToScoreAllItems
)
addTestSuite(test_calculate_scores "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp
)
list(APPEND test_cases
	combiningNoScoreSet
	combiningOneScoreSetWithZeroScores
	combiningOneScoreSetWithOneScore
	combiningOneScoreSetWithMultipleScores
	combiningTwoScoreSetsWhenOneScoreSetIsEmpty
	combiningTwoScoreSetsWithSameItem
	combiningTwoScoreSetsWithDifferentItems
	endToEnd_combineFromTwoFiles
	endToEnd_combineFromThreeFiles
	endToEnd_combineFromTooFewFiles
	endToEnd_combineFromInvalidFileName
	endToEnd_combineFromAllEmptyFiles
	endToEnd_combineFromOneEmptyFile
)
addTestSuite(test_combine_scores "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp
)
list(APPEND test_cases
	noScores
	alreadySorted
	reversedOrder
	winLossDifferenceIsZeroAndTotalIsDifferent
	winLossDifferenceIsZeroAndTotalIsTheSame
	winLossDifferenceIsEqualAndPositive
	winLossDifferenceIsEqualAndNegative
	shortItemNameAndWinsAndLosses
	longItemNameAndWinsAndLosses
)
addTestSuite(test_create_score_table "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	counterLengthEqualsTotalLength
	counterLengthIsLessThanTotalLength
	itemIsShorterThanLongestItem
	itemLengthIsEqualToHeaderLength
	votingIsCompleted
	incompleteVotingIncreasesCounterAndChangesItem
)
addTestSuite(test_current_voting_line "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/calculate_scores.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	votingRoundIsTheSameAfterLoading
	scoresAreTheSameDespiteDifferentItemOrderAndSeed
)
addTestSuite(test_e2e_voting_round "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	generateWithTooFewItems
	generateWithOnlyEmptyItems
	generateWithSomeEmptyItems
	generateWithFullVoting
	generateWithReducedVotingIfTooFewItemsToReduce
	generateWithReducedVotingIfEnoughItemsToReduce
	generateWithFullVotingGivesCorrectAmountOfScheduledVotes
	generateWithReducedVotingGivesCorrectAmountOfScheduledVotes
	generateWithFullVotingGivesCorrectScheduledVotes
)
addTestSuite(test_generate_new_voting_round "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp
)
list(APPEND test_cases
	noScores
	oneScore
	oneScoreWithSpacesInItemName
	multipleScores
)
addTestSuite(test_generate_score_file_data "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	emptyVotingRound
	votingRoundWithFullVoting
	votingRoundWithReducedVoting
	votingRoundWithOneVote
)
addTestSuite(test_generate_voting_round_file_data "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/menus.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	showIntro
	noVotingRoundCreated
	votingRoundStarted
	votingRoundCompleted
)
addTestSuite(test_get_active_menu_string "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/calculate_scores.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	itemOrderIsShuffledWhenShufflingNewVotingRound
	originalItemOrderIsRetainedWhenShufflingNewVotingRound
	itemOrderIsShuffledWhenShufflingParsedVotingRound
	originalItemOrderIsRetainedWhenShufflingParsedVotingRound
	convertingVotingRoundToTextUsesOriginalItemOrder
	scoresAreCalculatedWithCorrectItems
)
addTestSuite(test_item_order_randomized "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
)
list(APPEND test_cases
	savingAndLoadingLines
	savingAndLoadingLinesWithEmptyLines
	savingAndLoadingOnlyEmptyLines
	savingNoLines
	loadingNoLines
	savingToExistingFile
	loadingNonExistingFile
)
addTestSuite(test_load_and_save_file "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp
)
list(APPEND test_cases
	stringIsEmpty
	stringIsValid
	itemIsAnInteger
	itemHasMultipleWords
	itemHasIntegersAfterSpace
	stringMissingWinsOrLosses
	stringMissingItem
	winsOrLossesAreNegative
	winsOrLossesAreAfterItem
	multipleItemsWithoutWinsOrLosses
	noStringsSent
	allStringsAreValid
	onlyOneStringIsValid
	validNumbers
	nonNumbers
)
addTestSuite(test_parse_scores "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	noLinesToParse
	noItemsBeforeEmptyLine
	fewerThanTwoItems
	itemAppearsMultipleTimes
	anItemIsEmpty
	noEmptyLineBetweenItemsAndSeed
	seedIsMissing
	seedIsEmpty
	seedIsNotAPositiveInteger
	fullVoting
	reducedVotingWithTooFewItems
	reducedVotingWithEnoughItems
	reducedVotingSettingIsMissing
	reducedVotingSettingIsEmpty
	reducedVotingSettingIsInvalid
	noVotes
	voteDoesNotHaveThreeIntegers
	voteOptionIndicesAreGreaterThanNumberOfItems
	moreVotesThanPossible
	sameMatchupMultipleTimes
	chosenVoteIsNotZeroOrOne
	fourItemsAndReducedVotingAndFourVotes
	fourItemsAndFullVotingAndOneVote
	fourItemsAndFullVotingAndZeroVotes
)
addTestSuite(test_parse_voting_round "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	pruningDuringVotingRoundCreationWithTooFewItems
	pruningAmountDuringVotingRoundCreationDependsOnNumberOfItems
	parseVotingRoundWithPruning
	pruningRemovesCorrectScheduledVotes
)
addTestSuite(test_prune_votes "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/menus.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	newRoundFormatStringPresentsAvailableVotingFormatsAndNumberOfVotes
	createNewRankedVotingRound
	parseRankedVotingRoundWithNoVotes
	parseRankedVotingRoundWithSomeVotes
	parseRankedVotingRoundWithTooManyVotes
	undoWhenNoRankVote
	undoFirstRankVoteReducesNumberOfSortedItems
	undoLastRankVoteReducesNumberOfSortedItems
	undoAllRankVotesThenVoteTheSame
	undoRankVoteDisplaysPreviousOptions
	numberOfSortedItemsIncrementsWhenRightOptionChanges
	presentedOptionsDependOnVoting
	itemsAreSortedCorrectlyForNumberOfSortedItems
	rankItemsBySortingWhenInitiallyRanked
	rankItemsBySortingWhenInitiallyUnranked
	voteCounterAndApproximateTotalVotesIsPresented
)
addTestSuite(test_rank_based_voting "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp
)
list(APPEND test_cases
	saveWhenNoScores
	saveWhenSomeScores
)
addTestSuite(test_save_scores "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	saveWhenSomeVotesRemain
	saveWhenNoVotesRemain
)
addTestSuite(test_save_votes "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	shufflingWithSeedIsDeterministic
)
addTestSuite(test_shuffle_voting_order "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	undoWhenVotesExist
	undoWhenNoVotesExist
)
addTestSuite(test_undo "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	firstVoteForA
	firstVoteForB
	votingForAForEachScheduledVote
	votingForBForEachScheduledVote
	votingAfterRoundCompleted
)
addTestSuite(test_vote "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
)
list(APPEND test_cases
	charToFormatWithValidOptions
	charToFormatWithInvalidOptions
	stringToFormatWithValidOptions
	stringToFormatWithInalidOptions
	formatToString
)
addTestSuite(test_voting_format "${test_dependencies}" "${test_cases}")


list(APPEND test_dependencies
	${PROJECT_SOURCE_DIR}/src/test/mocks/mock_keyboard_input.cpp
	${PROJECT_SOURCE_DIR}/src/calculate_scores.cpp
	${PROJECT_SOURCE_DIR}/src/functions.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/menus.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/program_loop.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
)
list(APPEND test_cases
	mainMenuLegendPrinted
	mainMenuLegendNotReprinted
	mainMenuNewVotingRound
	mainMenuLoadVotingRound
	mainMenuCombine
	mainMenuQuit
	newVotingRoundSelectItemsPromptPrintedEachTime
	newVotingRoundSelectItemsEmptyFileName
	newVotingRoundSelectItemsCancel
	newVotingRoundSelectItemsNonExistingFile
	newVotingRoundSelectItemsTooFewItems
	newVotingRoundSelectItemsSuccessful
	newVotingRoundSelectFormatLegendPrinted
	newVotingRoundSelectFormatEstimatedVotesPrinted
	newVotingRoundSelectFormatLegendNotReprinted
	newVotingRoundSelectFormatCancel
	newVotingRoundSelectFormatPrintHelp
	newVotingRoundSelectFormatSuccessful
	quitWhenUnsaved
	quitWhenUnsavedThenCancel
	loadVotingRoundPromptPrintedEachTime
	loadVotingRoundEmptyFileName
	loadVotingRoundCancel
	loadVotingRoundNonExistingFile
	loadVotingRoundEmptyFile
	loadVotingRoundInvalidFile
	loadVotingRoundSuccessful
	saveVotingRoundPromptPrintedEachTime
	saveVotingRoundEmptyFileName
	saveVotingRoundCancel
	saveVotingRoundSuccessful
	saveVotingRoundScoresNoVotes
	saveVotingRoundScoresPromptPrintedEachTime
	saveVotingRoundScoresEmptyFileName
	saveVotingRoundScoresCancel
	saveVotingRoundScoresSuccessful
	saveVotingRoundRankingNoVotes
	saveVotingRoundRankingPromptPrintedEachTime
	saveVotingRoundRankingEmptyFileName
	saveVotingRoundRankingCancel
	saveVotingRoundRankingSuccessful
	saveVotingRoundRankingSortedMarked
	vote
	voteWhenVotingRoundCompleted
	undoWithNoVotes
	undoScoreVote
	undoRankVote
	printScore
	printRank
	combineEmptyFileNames
	combine
	combineThenPrintScores
	combineThenSaveScores
	combineThenSaveScoresCancel
	combineCancel
	combineWithTooFewFiles
	combineWithNonExistingFiles
)
addTestSuite(test_end_to_end "${test_dependencies}" "${test_cases}")
