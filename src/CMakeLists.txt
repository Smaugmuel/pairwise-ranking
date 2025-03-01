cmake_minimum_required(VERSION 3.15)

add_executable(${PROJECT_NAME}
	pairwise_ranking.cpp
	calculate_scores.cpp
	calculate_scores.h
	constants.h
	functions.cpp
	functions.h
	helpers.cpp
	helpers.h
	menus.cpp
	menus.h
	print.cpp
	print.h
	score.cpp
	score.h
	score_helpers.cpp
	score_helpers.h
	vote.cpp
	vote.h
	voting_format.cpp
	voting_format.h
	voting_round.cpp
	voting_round.h
)

add_subdirectory(test)
