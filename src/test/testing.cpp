#include "testing.h"

#include <iostream>

namespace
{

void assertion_failure(std::string const& error, std::source_location const& location) {
	std::cout << "Assertion failed " << location.file_name() << ":" << location.line() << ": " << error << 
		". In " << location.function_name() << "()" << std::endl;
	exit(1);
}

} // namespace

namespace detail
{

void assert_true(bool val, std::source_location const& location) {
	if (!val) {
		assertion_failure("", location);
	}
}

void assert_eq(std::nullopt_t const& a, std::nullopt_t const& b, std::source_location const& location) {
	assert_true(true, location);
}

void assert_ne(std::nullopt_t const& a, std::nullopt_t const& b, std::source_location const& location) {
	assert_true(false, location);
}

} // namespace detail

void ASSERT_TRUE(bool a, std::source_location const location) {
	detail::assert_true(a, location);
}
void ASSERT_FALSE(bool a, std::source_location const location) {
	detail::assert_true(!a, location);
}

auto getNItems(size_t n) -> std::vector<std::string> {
	std::vector<std::string> items;
	items.reserve(n);
	for (size_t i = 0; i < n; i++) {
		items.push_back("item" + std::to_string(i + 1));
	}
	return items;
}
