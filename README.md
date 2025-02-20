# Introduction

Pairwise ranking is a method with which users are presented two items, and vote on which of these two items they prefer.
After voting, a new pair if items is presented. This repeats, either until each item has been compared to every other item, or with smarter algorithms which are able to determine a full ranking order with fewer votes.

This implementation of the pairwise ranking method gives the user the option to vote either in full - i.e. every item against every other item - or by pruning a majority of the votes to get an inconclusive but partial result.

One of the main reasons for creating the project is to be able to combine multiple instances of results, e.g. from multiple users, into one common result. While the main intention for this is to combine full or partial results from multiple users, it can also be used to combine multiple partial results from the same user.

# Running the application

This project has been developed with CMake, which is the intended tool for building and testing the application.

## How to build the application
1. Navigate to the root of the project
2. To generate project files, run ``cmake -S . -B build``
3. To build project and tests, run ``cmake --build ./build --config Release``
	- The binary will be located in ``./build/src/Release/``

## How to run the application

After building the project, launch ``./build/src/Release/pairwise-ranking``.

## How to test the application

CTest is used to test the application logic, and can be run from the command line.

1. Navigate to the root of the project
2. To build project and tests, run ``cmake --build ./build --config Release``
3. To run tests, run ``ctest --test-dir ./build/src/test --build-config Release``
	- Add ``--output-on-failure`` for standard output on tests which fail

# How to use the application

TBD

## Menu options

TBD

## Pruning votes

If the full voting round is too big of an undertaking, the user can choose to prune votes when starting a new voting round and get significantly fewer pairs presented.
Pruning will not happen if the number of items $N$ are fewer than $6$, and the pruning will remove $N$ matchups $K$ times, where $K$ increments once for every two items above $6$

Full voting will consist of $N(N-1) \over 2$ votes. Pruned voting will consist of ${N(N-1) \over 2} - N{\lfloor {1 + {{N-6} \over 2}} \rfloor}$, for $N \ge 6$.

The $N*K$ matchups are removed in a way to ensure each item is represented equally during a voting round.
Here are some examples of how many votes the user can expect with and without pruning:

| Number of items | Full votes | Votes after pruning | Matchups per item (full) | Matchups per item (pruned) |
|-----------------|------------|---------------------|--------------------------|----------------------------|
| 5               | 10         | 10                  | 4                        | 4                          |
| 6               | 15         | 9                   | 5                        | 3                          |
| 7               | 21         | 14                  | 6                        | 4                          |
| 8               | 28         | 12                  | 7                        | 3                          |
| 9               | 36         | 18                  | 8                        | 4                          |
| 10              | 45         | 15                  | 9                        | 3                          |
| 15              | 105        | 30                  | 14                       | 4                          |
| 20              | 190        | 30                  | 19                       | 3                          |
| 25              | 300        | 50                  | 24                       | 4                          |
| 30              | 435        | 45                  | 29                       | 3                          |
| 35              | 595        | 70                  | 34                       | 4                          |
| 40              | 780        | 60                  | 39                       | 3                          |
| 45              | 990        | 90                  | 44                       | 4                          |
| 50              | 1225       | 75                  | 49                       | 3                          |
| 60              | 1770       | 90                  | 59                       | 3                          |
| 70              | 2415       | 105                 | 69                       | 3                          |
| 80              | 3160       | 120                 | 79                       | 3                          |
| 90              | 4005       | 135                 | 89                       | 3                          |
| 99              | 4851       | 198                 | 98                       | 4                          |
| 100             | 4950       | 150                 | 99                       | 3                          |

# Architecture

TBD

# Adding to the project

TBD

## Adding a test case

1. Create ``test_my_new_feature.cpp`` in ``src/tests``
2. Add ``test_my_new_feature`` to ``tests`` list in ``src/tests/CMakeLists``
3. Generate project files as instructed in [Running the application](#running-the-application)
