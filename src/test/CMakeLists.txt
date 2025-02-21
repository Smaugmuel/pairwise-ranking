cmake_minimum_required(VERSION 3.15)

enable_testing()

list(APPEND test_helper_files
	testing.cpp
	testing.h
	${PROJECT_SOURCE_DIR}/src/functions.cpp
	${PROJECT_SOURCE_DIR}/src/functions.h
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/print.h
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.h
)

list(APPEND tests
	test_load_and_save_file
	
	# Tests for generation and convertion of voting round
	test_generate_new_voting_round
	test_parse_voting_round
	test_generate_voting_round_file_data
	test_verify_voting_round
	
	# Tests for modification of votes
	test_shuffle_voting_order
	test_prune_votes
	
	# Tests for calculation and presentation of score
	test_calculate_scores
	test_combine_scores
	test_create_score_table
	test_generate_score_file_data
	test_parse_scores
	
	test_current_voting_line
	test_get_active_menu_string

	test_undo
	test_vote

	test_item_order_randomized
)

foreach(test_name ${tests})
	add_executable(${test_name} ${test_name}.cpp ${test_helper_files})
	target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
	add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
