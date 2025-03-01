#include "calculate_scores.h"

#include <algorithm>

namespace
{

void incrementWinner(Scores& scores, Item const& item) {
	auto it = std::find_if(scores.begin(), scores.end(), [&](Score const& score) {
		return score.item == item;
		});
	if (it == scores.end()) {
		Score score{};
		score.item = item;
		score.wins = 1;
		scores.emplace_back(score);
	}
	else {
		it->wins++;
	}
}
void incrementLoser(Scores& scores, Item const& item) {
	auto it = std::find_if(scores.begin(), scores.end(), [&](Score const& score) {
		return score.item == item;
		});
	if (it == scores.end()) {
		Score score{};
		score.item = item;
		score.losses = 1;
		scores.emplace_back(score);
	}
	else {
		it->losses++;
	}
}

} // namespace

auto calculateScores(Items const& items, Votes const& votes) -> Scores {
	Scores scores{};
	for (Vote const& vote : votes) {
		if (vote.winner == Option::A) {
			incrementWinner(scores, items[vote.a_idx]);
			incrementLoser(scores, items[vote.b_idx]);
		}
		else {
			incrementWinner(scores, items[vote.b_idx]);
			incrementLoser(scores, items[vote.a_idx]);
		}
	}
	return scores;
}
