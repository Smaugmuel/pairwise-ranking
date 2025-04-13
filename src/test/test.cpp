#include <string>

extern auto test_calculate_scores(std::string const&) -> int;
extern auto test_combine_scores(std::string const&) -> int;
extern auto test_create_score_table(std::string const&) -> int;
extern auto test_current_voting_line(std::string const&) -> int;
extern auto test_e2e_voting_round(std::string const&) -> int;
extern auto test_end_to_end(std::string const&) -> int;
extern auto test_generate_new_voting_round(std::string const&) -> int;
extern auto test_generate_score_file_data(std::string const&) -> int;
extern auto test_generate_voting_round_file_data(std::string const&) -> int;
extern auto test_get_active_menu_string(std::string const&) -> int;
extern auto test_item_order_randomized(std::string const&) -> int;
extern auto test_load_and_save_file(std::string const&) -> int;
extern auto test_parse_scores(std::string const&) -> int;
extern auto test_parse_voting_round(std::string const&) -> int;
extern auto test_prune_votes(std::string const&) -> int;
extern auto test_rank_based_voting(std::string const&) -> int;
extern auto test_save_scores(std::string const&) -> int;
extern auto test_save_votes(std::string const&) -> int;
extern auto test_shuffle_voting_order(std::string const&) -> int;
extern auto test_undo(std::string const&) -> int;
extern auto test_vote(std::string const&) -> int;
extern auto test_voting_format(std::string const&) -> int;

namespace
{

auto run_test(std::string const& suite, std::string const& test) -> int {
	if (suite == "test_calculate_scores") {
		return test_calculate_scores(test);
	}
	if (suite == "test_combine_scores") {
		return test_combine_scores(test);
	}
	if (suite == "test_create_score_table") {
		return test_create_score_table(test);
	}
	if (suite == "test_current_voting_line") {
		return test_current_voting_line(test);
	}
	if (suite == "test_e2e_voting_round") {
		return test_e2e_voting_round(test);
	}
	if (suite == "test_end_to_end") {
		return test_end_to_end(test);
	}
	if (suite == "test_generate_new_voting_round") {
		return test_generate_new_voting_round(test);
	}
	if (suite == "test_generate_score_file_data") {
		return test_generate_score_file_data(test);
	}
	if (suite == "test_generate_voting_round_file_data") {
		return test_generate_voting_round_file_data(test);
	}
	if (suite == "test_get_active_menu_string") {
		return test_get_active_menu_string(test);
	}
	if (suite == "test_item_order_randomized") {
		return test_item_order_randomized(test);
	}
	if (suite == "test_load_and_save_file") {
		return test_load_and_save_file(test);
	}
	if (suite == "test_parse_scores") {
		return test_parse_scores(test);
	}
	if (suite == "test_parse_voting_round") {
		return test_parse_voting_round(test);
	}
	if (suite == "test_prune_votes") {
		return test_prune_votes(test);
	}
	if (suite == "test_rank_based_voting") {
		return test_rank_based_voting(test);
	}
	if (suite == "test_save_scores") {
		return test_save_scores(test);
	}
	if (suite == "test_save_votes") {
		return test_save_votes(test);
	}
	if (suite == "test_shuffle_voting_order") {
		return test_shuffle_voting_order(test);
	}
	if (suite == "test_undo") {
		return test_undo(test);
	}
	if (suite == "test_vote") {
		return test_vote(test);
	}
	if (suite == "test_voting_format") {
		return test_voting_format(test);
	}
	return 1;
}

} // namespace

int main(int argc, char* argv[]) {
	if (argc != 3) {
		return 1;
	}
	return run_test(argv[1], argv[2]);
}
