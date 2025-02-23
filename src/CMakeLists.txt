cmake_minimum_required(VERSION 3.15)

add_executable(${PROJECT_NAME}
	pairwise_ranking.cpp
	constants.h
	functions.cpp
	functions.h
	helpers.cpp
	helpers.h
	print.cpp
	print.h
	voting_round.cpp
	voting_round.h
)

add_subdirectory(test)
