# Introduction

Pairwise comparison is a method with which users are presented two items, and vote on which of
these two items they prefer. After voting, a new pair if items is presented. This repeats, either
until each item has been compared to every other item, or with smarter algorithms which are able to
determine a full ranking order with fewer votes. Refer to
https://en.wikipedia.org/wiki/Pairwise_comparison_(psychology) for more information on this method.

This implementation of the pairwise comparison method is a simple terminal-based application. One
of the primary reasons for creating this project is to be able to combine multiple instances of
results, e.g. from multiple users, into one common result.

# Voting formats

The user has multiple voting formats to choose from when starting a new voting round.
- The **Full** format pairs each item against every other item in a matchup, keeping track of the
number of wins and losses each item receives
- The **Reduced** format is based on the Full format, but prunes matchups such that each item will
be seen only three or four times
- The **Ranked** format uses the insertion sort algorithm
([reference](https://en.wikipedia.org/wiki/Insertion_sort)) to rank items in order relative to each
other

## Expected voting length

These different voting formats will have different amounts of expected matchups, depending on the
number of items N. The Reduced voting format will ensure each item is represented equally.
Each item will then have three matchups when N is an even number, and four matchups when N is
uneven.

| Voting format        | Expected number of matchups with N items                                   |
|----------------------|----------------------------------------------------------------------------|
| Full score-based     | $N(N-1) \over 2$                                                           |
| Reduced score-based  | ${N(N-1) \over 2} - N{\lfloor {1+{{N-6} \over 2}} \rfloor}$, for $N \ge 6$ |
| Insertion rank-based | less than $Nlog{_2}(N)$                                                    |

# Combining scores

When two or more voting rounds have been completed and their respective scores saved to files,
those score files can be combined into one. This is done by starting the program, initiating the
*Combine* option, and passing all score file names when requested. If successful, the application
will create a new file with the combined scores. Any item present in multiple files will have its
wins and losses respectively summed up.

# Running the application

This project has been developed with CMake, which is the intended tool for building and testing the application.

## How to build the application
1. Navigate to the root of the project
2. To generate Visual Studio project files, run ``cmake -S . -B build``
3. To build project and tests, run ``cmake --build ./build --config Release``
	- The binary will be located in ``./build/src/Release/``

## How to run the application

After building the project, launch ``./build/src/Release/pairwise-ranking``.

## How to test the application logic

CTest is used to test the application logic, and can be run from the command line.

1. Navigate to the root of the project
2. To build project and tests, run ``cmake --build ./build --config Release``
3. To run tests, run ``ctest --test-dir ./build/src/test --build-config Release``
	- Add ``--output-on-failure`` for standard output on tests which fail

# Adding test cases

1. If a new test suite needs to be added for the test, start with the following
    i. create ``test_<my_new_feature>.cpp`` in ``src/tests``
	ii. In ``src/tests/CMakeLists.txt``, add a *test_dependencies* list, a *test_cases* list, and
		call *addTestSuite*, passing the test suite name, the *test_dependencies* list and the
		*test_cases* list
2. In the selected new or existing test suite, a test case is defined as one function with a
   descriptive name in which the test logic is placed
3. Within the test suite file's main function, call *RUN_TEST_IF_ARGUMENT_EQUALS* and pass the test
   case name
4. Populate the *test_dependencies* with any source files required to build the test suite
5. Populate the *test_cases* with each test case name within the test suite file
