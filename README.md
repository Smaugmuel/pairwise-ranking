# Introduction

Pairwise comparison is a method with which users are presented two items, and vote on which of
these two items they prefer. After voting, a new pair of items is presented. This repeats, either
until each item has been compared to every other item, or with smarter algorithms which are able to
determine a result with fewer votes. Refer to
https://en.wikipedia.org/wiki/Pairwise_comparison_(psychology) for more information on this method.

This implementation of the pairwise comparison method is a terminal-based application. One of the
primary reasons for creating this project is to be able to combine multiple instances of results,
e.g. from multiple users and at different times, into one common result.

# Basics and prerequisites

To start a voting round with this application, two things are needed:
- The application executable. See **Building the application** below.
- An ASCII text file with all items to vote on, with exactly one item per line.

When creating a new voting round, the user will select the file from which to load the items, and
then select which format to use for the voting round. After this, the voting starts and the first
matchup (pair of items) is presented. Voting for one of the two items will then present the next
matchup, which repeats until the voting round is completed. At any point during the voting round,
the user can undo their previous vote, print the current scores/ranks, or save the current state of
the voting round, as well as their votes and scores so far.

A previously saved voting round can be loaded and continued from the main menu, instead of having
to create a new one every time.

# Voting formats

The user has multiple formats to choose from when starting a new voting round.
- The **Full** format pairs each item against every other item in a matchup, keeping track of the
number of wins and losses each item receives
- The **Reduced** format is based on the full format, but prunes matchups to give a partial result
and significantly reduce the number of votes
- The **Ranked** format uses the insertion sort algorithm
([reference](https://en.wikipedia.org/wiki/Insertion_sort)) to rank items in order relative to each
other

## Expected voting length

These different voting formats have different amounts of expected matchups, based on the number of
items N. Full voting requires the most amount of votes, scaling with $O(N^2)$, as every pair of
items is presented. Reduced voting scales with $O(N)$, greatly reducing the number of votes
required at the cost of giving only a partial result. Each item is seen either three or four times.
Ranked voting has have an undetermined number of votes scaling with $O(Nlog{_2}(N))$, as the
matchups will vary depending on how the user votes.

For exact voting lengths, see the table below.
<!---
This works outside the table, but not in it
$$ \begin{cases} 1 \\\ 0 \end{cases} $$
-->

| Voting format        | Number of matchups with N items                                                                                                                          |
|----------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------|
| Full score-based     | $N(N-1) \over 2$                                                                                                                                         |
| Reduced score-based  | <ul><li>$N(N-1) \over 2$, for $N < 6$</li><li>$3N \over 2$, for $N \ge 6$ and when $N$ is even</li><li>$2N$, for $N \ge 6$ and when $N$ is odd</li></ul> |
| Insertion rank-based | less than $Nlog{_2}(N)$                                                                                                                                  |

# Combining scores

When two or more voting rounds have been completed and their respective scores saved to files,
those score files can be combined into one. This is done by running the application, initiating the
*Combine* option, and selecting which score files to combine. The application will create a new
file with the combined scores, and any item present in both files will have its wins and losses
respectively summed up.

# How to build and run the application

This project has been developed with CMake, which is the intended tool for building and testing the
application.

## Building the application
1. Clone this repository
2. Navigate to the root of the project
3. To generate Visual Studio project files, run ``cmake -S . -B build``
4. To build project and tests, run ``cmake --build ./build --config Release``
	- The executable binary will be located in ``./build/src/Release/``

## Running the application

After building the project, launch ``./build/src/Release/pairwise-ranking``.

# Testing

CMake's CTest is used to test the application logic, and can be run from the command line.

1. Navigate to the root of the project
2. To build project and tests, run ``cmake --build ./build --config Release``
3. To run tests, run ``ctest --test-dir ./build/src/test --build-config Release``
	- Add ``--output-on-failure`` for standard output on tests which fail

## Adding test cases

1. If a new test suite needs to be added for the test, start with the following
    i. create ``test_<my_new_feature>.cpp`` in ``src/tests``
	ii. In ``src/tests/CMakeLists.txt``, add a ``test_dependencies`` list, a ``test_cases`` list,
		and call ``addTestSuite()``, passing the test suite name,``test_dependencies`` and
		``test_cases``
2. A test case is defined as one function with a descriptive name in which the test logic is
   placed. For each test case, add one such function in the selected test suite file.
3. Within the test suite file's main function, for each test case, call
   ``RUN_TEST_IF_ARGUMENT_EQUALS`` and pass the test case name to it.
4. Populate the ``test_dependencies`` list in the ``CMakeLists.txt`` with any source files required
   to build the test suite.
5. Populate the ``test_cases`` list in the ``CMakeLists.txt`` with each test case name within the
   test suite file

## Mock input and catch printout

Most applications logic is tested with unit tests which verify features which don't require user
input. Some logic however, such as the menus and the application loop, can only be tested if user
input is present.

While the real application gets user input from the keyboard, tests can mock this input by queueing
up key presses and text lines. To mock input, the test case must be compiled with
``src/test/mocks/mock_keyboard_input.cpp`` instead of ``src/keyboard_input.cpp``. In the test case,
the full application loop is then invoked, and whenever user input is required the queued input is
retrieved instead of real keyboard input.
