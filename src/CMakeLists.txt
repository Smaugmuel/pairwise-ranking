cmake_minimum_required(VERSION 3.15)

add_executable(${PROJECT_NAME}
	functions.cpp
	functions.h
	pairwise-ranking.cpp
)

add_subdirectory(test)