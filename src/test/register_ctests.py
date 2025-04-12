import glob, os, re

def findAllTestsInFile(file_name):
	with open(file_name, 'r') as file:
		file_content = file.read()
		return [test[5:-2] for test in re.findall(r'void .*\(\)', file_content)]

def findLineIndexOfMatch(list_to_search, element) -> int:
	for num, line in enumerate(list_to_search, 1):
		if element in line:
			return num - 1
	print("No match found for '" + element + "'")
	return -1

def findLineIndexOfNextClosingParenthesis(list_to_search, starting_index):
	for num, line in enumerate(list_to_search, 1):
		if num < starting_index + 1:
			continue
		if "addTestSuite" in line:
			break
		if ")" in line:
			return num - 1
	print("No match found for ')'")
	return -1

def findLineIndexOfNextReturn(list_to_search, starting_index):
	for num, line in enumerate(list_to_search, 1):
		if num < starting_index + 1:
			continue
		if "return true" in line:
			return num - 1
	print("No match found for 'return'")
	return -1

def updateCMakeLists(test_suite):
	with open("CMakeLists.txt", 'r') as file:
		cmake_file_data = file.readlines()

	# Find range of existing CTest cases
	tests_start_index = findLineIndexOfMatch(cmake_file_data, test_suite) + 1
	tests_end_index = findLineIndexOfNextClosingParenthesis(cmake_file_data, tests_start_index) - 1

	# Replace exiting CTest cases with tests found in source code
	del cmake_file_data[tests_start_index:tests_end_index+1]
	cmake_file_data[tests_start_index:tests_start_index] = [
		'\t' + x + '\n'
		for x in findAllTestsInFile(test_suite + ".cpp")
	]

	with open("CMakeLists.txt", "w") as file:
		file.write("".join(cmake_file_data))

def updateTestSuiteFile(test_suite):
	with open(test_suite + ".cpp", 'r') as file:
		test_suite_data = file.readlines()

	# Find range of existing test macros
	tests_start_index = findLineIndexOfMatch(test_suite_data, "auto run_tests(std::string const& test)") + 1
	tests_end_index = findLineIndexOfNextReturn(test_suite_data, tests_start_index) - 1

	# Replace existing test macros with tests found in source code
	del test_suite_data[tests_start_index:tests_end_index+1]
	test_suite_data[tests_start_index:tests_start_index] = [
		"\tRUN_TEST_IF_ARGUMENT_EQUALS(" + x + ");\n"
		for x in findAllTestsInFile(test_suite + ".cpp")
	]

	with open(test_suite + ".cpp", "w") as file:
		file.write("".join(test_suite_data))

os.chdir(os.path.dirname(__file__))

# Find all test suites
test_suites = [ name[:-4] for name in glob.glob("test_*.cpp")]

for test_suite in test_suites:
	updateCMakeLists(test_suite)
	updateTestSuiteFile(test_suite)
