#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

auto sumOfFirstIntegers(size_t n) -> size_t;
auto pruningAmount(uint32_t const number_of_items) -> uint32_t;
auto isNumber(std::string const& str) -> bool;
auto parseWords(std::string const& str) -> std::vector<std::string>;
auto parseNumber(std::string const& str) -> std::optional<uint32_t>;
auto numberOfDigits(size_t n) -> size_t;
auto saveFile(std::string const& file_name, std::vector<std::string> const& lines) -> bool;
auto loadFile(std::string const& file_name) -> std::vector<std::string>;

template<typename T>
constexpr auto to_underlying(T t) -> std::underlying_type_t<T> {
	return static_cast<std::underlying_type_t<T>>(t);
}
