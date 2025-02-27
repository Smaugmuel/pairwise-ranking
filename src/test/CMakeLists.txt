cmake_minimum_required(VERSION 3.15)

enable_testing()

set(test_name test_calculate_scores)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_combine_scores)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_create_score_table)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_current_voting_line)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_generate_new_voting_round)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_generate_score_file_data)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_generate_voting_round_file_data)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_get_active_menu_string)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/functions.cpp			# TODO: Remove dependency to this file here
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp				# TODO: Remove dependency to this file here
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp		# TODO: Remove dependency to this file here
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp		# TODO: Remove dependency to this file here
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_item_order_randomized)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp				# calculateScores compares scores
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp		# For calculateScores
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_load_and_save_file)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_parse_scores)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_parse_voting_round)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_prune_votes)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_save_scores)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/score.cpp
	${PROJECT_SOURCE_DIR}/src/score_helpers.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_save_votes)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_shuffle_voting_order)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_undo)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_vote)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/helpers.cpp
	${PROJECT_SOURCE_DIR}/src/print.cpp
	${PROJECT_SOURCE_DIR}/src/vote.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp
	${PROJECT_SOURCE_DIR}/src/voting_round.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})

set(test_name test_voting_format)
add_executable(${test_name}
	${test_name}.cpp
	testing.cpp
	${PROJECT_SOURCE_DIR}/src/voting_format.cpp)
target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME ${test_name} COMMAND ${test_name})
