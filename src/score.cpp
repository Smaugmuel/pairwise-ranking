#include "score.h"

auto operator==(Score const& a, Score const& b) -> bool {
	return a.item == b.item && a.wins == b.wins && a.losses == b.losses;
}
