﻿cmake_minimum_required(VERSION 3.15)

project(pairwise-ranking LANGUAGES CXX)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)