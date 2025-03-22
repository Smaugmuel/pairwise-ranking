#pragma once

#include <optional>
#include <source_location>
#include <string>
#include <vector>

/* -------------- Test definitions -------------- */
#define RUN_TEST_IF_ARGUMENT_EQUALS(test_name) if (std::string{ argv[1] } == #test_name) { test_name(); return 0; }

namespace detail
{

void assert_true(bool val, std::source_location const& location);

template<typename A, typename B>
void assert_eq(A const& a, B const& b, std::source_location const& location) {
	assert_true(a == b, location);
}
template<typename A, typename B>
void assert_ne(A const& a, B const& b, std::source_location const& location) {
	assert_true(a != b, location);
}

} // namespace detail

/* -------------- Assertions to use in tests -------------- */
template<typename A, typename B>
void ASSERT_EQ(A const& a, B const& b, std::source_location const location = std::source_location::current()) {
	detail::assert_eq(a, b, location);
}
template<typename T>
void ASSERT_NE(T const& a, T const& b, std::source_location const location = std::source_location::current()) {
	detail::assert_ne(a, b, location);
}
void ASSERT_TRUE(bool a, std::source_location const location = std::source_location::current());
void ASSERT_FALSE(bool a, std::source_location const location = std::source_location::current());

/* -------------- Miscellaneous test helpers -------------- */
template<typename T>
auto operator+(std::vector<T> const& vec, T&& str) -> decltype(auto) {
	auto new_vec = vec;
	new_vec.emplace_back(str);
	return new_vec;
}
template<typename T>
auto operator+(std::vector<T> const& a, std::vector<T> const& b) -> decltype(auto) {
	std::vector<T> vec;
	vec.reserve(a.size() + b.size());
	vec.insert(vec.end(), a.begin(), a.end());
	vec.insert(vec.end(), b.begin(), b.end());
	return vec;
}
auto getNItems(size_t n) -> std::vector<std::string>;
