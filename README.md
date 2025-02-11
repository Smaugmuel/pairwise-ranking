# Introduction

TBD

# Running the application

## How to build
1. Navigate to the root of the project
2. To generate project files, run ``cmake -S . -B build``
3. To build project and tests, run ``cmake --build ./build --config Release``
	- The binary will be located in ``./build/src/Release/``

## How to run the application

After building the project, launch ``./build/src/Release/pairwise-ranking``.

## How to run tests

CTest is used to test the application logic, and can be run from the command line.

1. Navigate to the root of the project
2. To build project and tests, run ``cmake --build ./build --config Release``
3. To run tests, run ``ctest --test-dir ./build/src/test --build-config Release``
	- Add ``--output-on-failure`` for standard output on tests which fail

# Architecture

TBD

# Adding to the project

TBD

## Adding a test case

1. Create ``test_my_new_feature.cpp`` in ``src/tests``
2. Add ``test_my_new_feature`` to ``tests`` list in ``src/tests/CMakeLists``
3. Generate project files as instructed in [Running the application](#running-the-application)
