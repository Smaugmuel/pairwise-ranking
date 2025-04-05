import glob, os, re

os.chdir(os.path.dirname(__file__))

test_suite_files = glob.glob("test_*.cpp")
test_suites = [ name[:-4] for name in test_suite_files]

with open("CMakeLists.txt", 'r') as cmake_file:
	cmake_file_content = cmake_file.readlines()

def findAllTestsInFile(file_name):
	with open(file_name, 'r') as file:
		file_content = file.read()
		return [test[5:-2] for test in re.findall(r'void .*\(\)', file_content)]

def findLineInList(list_to_search, line_to_find):
	for num, line in enumerate(list_to_search, 1):
		if line_to_find in line:
			return num
	print("Failed to find " + line_to_find)
	return -1

def findPreviousTestCaseList(list_to_search, line_number):
	for num, line in reversed([x for x in enumerate(list_to_search, 1) ]):
		if num >= line_number:
			continue
		if "addTestSuite" in line:
			break
		if "list(APPEND test_cases" in line:
			return num
	print("No test_cases list found")
	return -1

def findNextReturnInTestSuite(list_to_search, line_number):
	for num, line in enumerate(list_to_search, 1):
		if num < line_number:
			continue
		if "return 1" in line:
			return num
	print("No return found")
	return -1

#### Update CMakeLists.txt #####
for test_suite in test_suites:
	add_test_suite_line = findLineInList(cmake_file_content, test_suite)
	test_cases_list_line = findPreviousTestCaseList(cmake_file_content, add_test_suite_line)

	# Note: Conversion to index kept for clarity
	test_cases_start_index = (test_cases_list_line - 1) + 1
	test_cases_end_index   = (add_test_suite_line  - 1) - 2

	del cmake_file_content[test_cases_start_index:test_cases_end_index+1]

	cmake_file_content[test_cases_start_index:test_cases_start_index] = [
		'\t' + x + '\n'
		for x in findAllTestsInFile(test_suite + ".cpp")
	]
	
	with open("CMakeLists.txt", "w") as file:
		file.write("".join(cmake_file_content))
		
##### Update test suite file #####
for test_suite in test_suites:
	with open(test_suite + ".cpp", 'r') as test_suite_file:
		test_suite_file_content = test_suite_file.readlines()
	run_tests_line = findLineInList(test_suite_file_content, "auto run_tests(char* argv[])")
	return_line = findNextReturnInTestSuite(test_suite_file_content, run_tests_line)

	# Note: Conversion to index kept for clarity
	run_test_start_index = (run_tests_line - 1) + 1
	run_test_end_index   = (return_line - 1) - 1

	del test_suite_file_content[run_test_start_index:run_test_end_index+1]
	test_suite_file_content[run_test_start_index:run_test_start_index] = [
		"\tRUN_TEST_IF_ARGUMENT_EQUALS(" + x + ");\n"
		for x in findAllTestsInFile(test_suite + ".cpp")
	]

	with open(test_suite + ".cpp", "w") as file:
		file.write("".join(test_suite_file_content))
