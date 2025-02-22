cmake_minimum_required(VERSION 3.15)

add_executable(${PROJECT_NAME}
	constants.h
	functions.cpp
	functions.h
	pairwise_ranking.cpp
	print.cpp
	print.h
	voting_round.cpp
	voting_round.h
)

add_subdirectory(test)
