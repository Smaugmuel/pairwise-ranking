#include "testing.h"

#include <iostream>

namespace
{

void assertion_failure(std::source_location const& location, std::string const& error) {
	std::cout << "Assertion failed " << location.file_name() << ":" << location.line() << ": " << error << 
		". In " << location.function_name() << "()" << std::endl;
	exit(1);
}

} // namespace

void assert_true(std::source_location const& location, bool val) {
	if (!val) {
		assertion_failure(location, "");
	}
}

void assert_eq(std::source_location const& location, std::nullopt_t const& a, std::nullopt_t const& b) {
	assert_true(location, true);
}

void assert_ne(std::source_location const& location, std::nullopt_t const& a, std::nullopt_t const& b) {
	assert_true(location, false);
}

void ASSERT_TRUE(bool a, std::source_location const location) {
	assert_true(location, a);
}
void ASSERT_FALSE(bool a, std::source_location const location) {
	assert_true(location, !a);
}

auto getNItems(size_t n) -> std::vector<std::string> {
	std::vector<std::string> items;
	items.reserve(n);
	for (size_t i = 0; i < n; i++) {
		items.push_back("item" + std::to_string(i + 1));
	}
	return items;
}
