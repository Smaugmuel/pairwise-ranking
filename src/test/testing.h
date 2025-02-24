#pragma once

#include <iostream>
#include <optional>
#include <source_location>
#include <string>
#include <type_traits>
#include <vector>

void assertion_failure(std::source_location const& location, std::string const& error);
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

#if false
class BaseTest {
public:
	virtual void test() const = 0;
};

template<typename T>
class TypedBaseTest : public BaseTest {
public:
	void test() const override {
		constexpr size_t kOmitHeadLength = 6;
		std::cout << std::string(typeid(T).name()).substr(kOmitHeadLength) << std::endl;
		test_impl();
	}
protected:
	virtual void test_impl() const = 0;
};

#define TEST(name) class name : public TypedBaseTest<name> { \
protected: \
	void test_impl() const override; \
}; \
void name::test_impl() const

template<typename ... T>
void run_tests() {
	std::tuple<T...> tests;

	std::apply([](auto&&... args) { (args.test(), ...); }, tests);
}
#endif

auto getNItems(size_t n) -> std::vector<std::string>;
