#include <string>
#include <iostream>

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

int main(int argc, char* argv[]) {
	if (argc != 3) {
		return 1;
	}
	if (std::string{ argv[1] } == "test_calculate_scores") {
		return test_calculate_scores(argv[2]);
	}
	if (std::string{ argv[1] } == "test_combine_scores") {
		return test_combine_scores(argv[2]);
	}
	if (std::string{ argv[1] } == "test_create_score_table") {
		return test_create_score_table(argv[2]);
	}
	if (std::string{ argv[1] } == "test_current_voting_line") {
		return test_current_voting_line(argv[2]);
	}
	if (std::string{ argv[1] } == "test_e2e_voting_round") {
		return test_e2e_voting_round(argv[2]);
	}
	if (std::string{ argv[1] } == "test_end_to_end") {
		return test_end_to_end(argv[2]);
	}
	if (std::string{ argv[1] } == "test_generate_new_voting_round") {
		return test_generate_new_voting_round(argv[2]);
	}
	if (std::string{ argv[1] } == "test_generate_score_file_data") {
		return test_generate_score_file_data(argv[2]);
	}
	if (std::string{ argv[1] } == "test_generate_voting_round_file_data") {
		return test_generate_voting_round_file_data(argv[2]);
	}
	if (std::string{ argv[1] } == "test_get_active_menu_string") {
		return test_get_active_menu_string(argv[2]);
	}
	if (std::string{ argv[1] } == "test_item_order_randomized") {
		return test_item_order_randomized(argv[2]);
	}
	if (std::string{ argv[1] } == "test_load_and_save_file") {
		return test_load_and_save_file(argv[2]);
	}
	if (std::string{ argv[1] } == "test_parse_scores") {
		return test_parse_scores(argv[2]);
	}
	if (std::string{ argv[1] } == "test_parse_voting_round") {
		return test_parse_voting_round(argv[2]);
	}
	if (std::string{ argv[1] } == "test_prune_votes") {
		return test_prune_votes(argv[2]);
	}
	if (std::string{ argv[1] } == "test_rank_based_voting") {
		return test_rank_based_voting(argv[2]);
	}
	if (std::string{ argv[1] } == "test_save_scores") {
		return test_save_scores(argv[2]);
	}
	if (std::string{ argv[1] } == "test_save_votes") {
		return test_save_votes(argv[2]);
	}
	if (std::string{ argv[1] } == "test_shuffle_voting_order") {
		return test_shuffle_voting_order(argv[2]);
	}
	if (std::string{ argv[1] } == "test_undo") {
		return test_undo(argv[2]);
	}
	if (std::string{ argv[1] } == "test_vote") {
		return test_vote(argv[2]);
	}
	if (std::string{ argv[1] } == "test_voting_format") {
		return test_voting_format(argv[2]);
	}
	return 1;
}
