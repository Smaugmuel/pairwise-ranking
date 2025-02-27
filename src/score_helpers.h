#pragma once

#include "score.h"
#include "vote.h"

using Items = std::vector<Item>;

auto calculateScores(Items const& items, Votes const& votes) -> Scores;

/* -------------- Print scores -------------- */
auto sortScores(Scores const& scores) -> Scores;
auto findMaxLengthItem(Scores const& scores) -> size_t;
auto findMaxLengthWins(Scores const& scores) -> size_t;
auto findMaxLengthLosses(Scores const& scores) -> size_t;
auto createScoreTable(Scores const& scores) -> std::string;

/* -------------- Combine scores -------------- */
void addScore(Scores& scores, Score const& new_score);
auto addScores(Scores const& a, Scores const& b) -> Scores;
auto combineScores(std::vector<Scores> const& score_sets) -> Scores;

/* -------------- Score conversion -------------- */
auto parseScore(std::string const& str) -> Score;
auto parseScores(std::vector<std::string> const& lines) -> Scores;
auto generateScoreFileData(Scores const& scores) -> std::vector<std::string>;

/* -------------- File management -------------- */
auto saveScores(Scores const& scores, std::string const file_name) -> bool;
