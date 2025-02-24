#include "vote.h"

auto operator==(Vote const& a, Vote const& b) -> bool {
	return a.a_idx == b.a_idx && a.b_idx == b.b_idx && a.winner == b.winner;
}
