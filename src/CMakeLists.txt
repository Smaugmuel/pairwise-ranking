cmake_minimum_required(VERSION 3.15)

add_executable(${PROJECT_NAME}
	functions.cpp
	pairwise-ranking.cpp
	${PROJECT_SOURCE_DIR}/include/functions.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(test)