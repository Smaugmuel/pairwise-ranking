import glob, os, re

def findAllTestsInFile(file_name):
	with open(file_name, 'r') as file:
		file_content = file.read()
		return [test[5:-2] for test in re.findall(r'void .*\(\)', file_content)]

def findLineIndexOfMatch(list_to_search, element) -> int:
	for num, line in enumerate(list_to_search, 1):
		if element in line:
			return num - 1
	return -1

def findLineIndexOfNextClosingParenthesis(list_to_search, starting_index):
	for num, line in enumerate(list_to_search, 1):
		if num < starting_index + 1:
			continue
		if "addTestSuite" in line:
			break
		if ")" in line:
			return num - 1
	return -1

def findLineIndexOfNextReturn(list_to_search, starting_index):
	for num, line in enumerate(list_to_search, 1):
		if num < starting_index + 1:
			continue
		if "return true" in line:
			return num - 1
	return -1

def addTestSuiteInCMakeIfNotPresent(data, test_suite):
	if findLineIndexOfMatch(data, test_suite) != -1:
		return

	print(f"Creating CMake test suite for {test_suite}")

	test_suites_end_index = findLineIndexOfMatch(data, "function(addTestExe")
	data[test_suites_end_index:test_suites_end_index] = [
		f"addTestSuite({test_suite}" + "\n",
		")" + "\n",
		"\n",
	]

def addRunTestsIfNotPresent(data, test_suite):
	if findLineIndexOfMatch(data, "auto run_tests") != -1:
		return

	print(f"Creating run_tests for {test_suite}")

	namespace_end_index = findLineIndexOfMatch(data, "} // namespace")
	data[namespace_end_index:namespace_end_index] = [
		"auto run_tests(std::string const& test) -> bool {" + "\n",
		"\t" + "return true;" + "\n",
		"}" + "\n",
		"\n",
	]

def addTestSuiteEntryPointIfNotPresent(data, test_suite):
	if findLineIndexOfMatch(data, f"auto {test_suite}") != -1:
		return

	print(f"Creating test suite entry point for {test_suite}")

	data += [
		"\n",
		f"auto {test_suite}(std::string const& test_case) -> int {{" + "\n",
		"\t" + "if (run_tests(test_case)) {" + "\n",
		"\t\t" + "return 1;" + "\n",
		"\t" + "}" + "\n",
		"\t" + "return 0;" + "\n",
		"}" + "\n",
	]

def updateCMakeLists(test_suite):
	with open("CMakeLists.txt", 'r') as file:
		cmake_file_data = file.readlines()

	addTestSuiteInCMakeIfNotPresent(cmake_file_data, test_suite)

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

	addRunTestsIfNotPresent(test_suite_data, test_suite)
	addTestSuiteEntryPointIfNotPresent(test_suite_data, test_suite)

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
