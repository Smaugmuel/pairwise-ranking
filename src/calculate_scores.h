#pragma once

#include <vector>

#include "score.h"
#include "vote.h"

using Item = std::string;
using Items = std::vector<Item>;

auto calculateScores(Items const& items, Votes const& votes) -> Scores;
