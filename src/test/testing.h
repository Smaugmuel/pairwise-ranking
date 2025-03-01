#pragma once

#include <optional>
#include <source_location>
#include <string>
#include <vector>

/* -------------- Test definitions -------------- */
#define RUN_TEST_IF_ARGUMENT_EQUALS(test_name) if (std::string{ argv[1] } == #test_name) { test_name(); return 0; }

/* -------------- Assertions details -------------- */
void assert_true(std::source_location const& location, bool val);

template<typename T>
void assert_eq(std::source_location const& location, T const& a, T const& b) {
	assert_true(location, a == b);
}
template<typename T>
void assert_ne(std::source_location const& location, T const& a, T const& b) {
	assert_true(location, a != b);
}

template<typename T>
void assert_eq(std::source_location const& location, std::optional<T> const& a, std::optional<T> const& b) {
	if (a == b) {
		return;
	}
	if (!a.has_value()) {
		assert_true(location, false);
		return;
	}
	if (!b.has_value()) {
		assert_true(location, false);
		return;
	}
	assert_true(location, false);
}
template<typename T>
void assert_ne(std::source_location const& location, std::optional<T> const& a, std::optional<T> const& b) {
	if (!a.has_value() && !b.has_value()) {
		assert_true(location, false);
		return;
	}
	if (a.has_value() && b.has_value()) {
		assert_ne(location, a.value() == b.value());
	}
}
template<typename T>
void assert_eq(std::source_location const& location, std::optional<T> const& a, std::nullopt_t const& b) {
	assert_true(location, !a.has_value());
}
template<typename T>
void assert_ne(std::source_location const& location, std::optional<T> const& a, std::nullopt_t const& b) {
	assert_true(location, a.has_value());
}
void assert_eq(std::source_location const& location, std::nullopt_t const& a, std::nullopt_t const& b);
void assert_ne(std::source_location const& location, std::nullopt_t const& a, std::nullopt_t const& b);

/* -------------- Assertions to use in tests -------------- */
template<typename A, typename B>
void ASSERT_EQ(A const& a, B const& b, std::source_location const location = std::source_location::current()) {
	assert_eq(location, a, b);
}
template<typename T>
void ASSERT_NE(T const& a, T const& b, std::source_location const location = std::source_location::current()) {
	assert_ne(location, a, b);
}
void ASSERT_TRUE(bool a, std::source_location const location = std::source_location::current());
void ASSERT_FALSE(bool a, std::source_location const location = std::source_location::current());

/* -------------- Test helpers -------------- */
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
