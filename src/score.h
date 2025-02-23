#pragma once

#include <cstdint>
#include <string>
#include <vector>

using Item = std::string;

struct Score {
	Item item{};
	uint32_t wins = 0;
	uint32_t losses = 0;
};
auto operator==(Score const& a, Score const& b) -> bool;
using Scores = std::vector<Score>;
