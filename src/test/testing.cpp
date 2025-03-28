#include "testing.h"

#include <iostream>

namespace detail
{

void assertion_failure(std::string const& error, std::source_location const& location) {
	std::cout << "Assertion failed " << location.file_name() << ":" << location.line() << ": " << error <<
		". In " << location.function_name() << "()" << std::endl;
	exit(1);
}

auto equalityStr(std::string const& a, std::string const& b, std::string const& eq_str) -> std::string {
	return "Expected " + eq_str + " between " + a + " and " + b;
}

} // namespace detail

void ASSERT_TRUE(bool a, std::source_location const location) {
	if (!a) {
		detail::assertion_failure("", location);
	}
}
void ASSERT_FALSE(bool a, std::source_location const location) {
	if (a) {
		detail::assertion_failure("", location);
	}
}

auto getNItems(size_t n) -> std::vector<std::string> {
	std::vector<std::string> items;
	items.reserve(n);
	for (size_t i = 0; i < n; i++) {
		items.push_back("item" + std::to_string(i + 1));
	}
	return items;
}
