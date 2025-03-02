#pragma once

#include <string>
#include <vector>

#include "score.h"

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
auto combine(std::string const& file_names_line, std::string const& combined_score_file_name) -> bool;

/* -------------- Score conversion -------------- */
auto parseScore(std::string const& str) -> Score;
auto parseScores(std::vector<std::string> const& lines) -> Scores;
auto generateScoreFileData(Scores const& scores) -> std::vector<std::string>;

/* -------------- File management -------------- */
auto saveScores(Scores const& scores, std::string const file_name) -> bool;
