# Pairwise ranking

TBD

## Building the application

1. Navigate to the root of the project
2. To generate project files, run ``cmake -S . -B build``
3. To build project and tests, run ``cmake --build ./build --config Release``
	- The binary will be located in ``./build/src/Release/``
4. To run tests, run ``ctest --test-dir ./build/src/test --build-config Release``
	- Add ``--output-on-failure`` for standard output on tests which fail

## Usage

TBD

## Testing

TBD

### Adding a test

1. Create ``test_my_new_feature.cpp`` in ``src/tests``
2. Add ``test_my_new_feature`` to ``tests`` list in ``src/tests/CMakeLists``
3. Generate project files as instructed in [Building the applications](#building-the-application)
