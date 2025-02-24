#pragma once

#include <cstdint>
#include <vector>

using Index = uint32_t;

enum class Option : uint32_t {
	A,
	B
};

struct Vote {
	Index a_idx{};
	Index b_idx{};
	Option winner{};
};
auto operator==(Vote const& a, Vote const& b) -> bool;
using Votes = std::vector<Vote>;
