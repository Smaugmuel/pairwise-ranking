#include "helpers.h"

#include <sstream>

#include "constants.h"
#include "print.h"

auto sumOfFirstIntegers(size_t n) -> size_t {
	return (n * (n + 1)) / 2;
}
auto pruningAmount(uint32_t const number_of_items) -> uint32_t {
	if (number_of_items < kMinimumItemsForPruning) {
		return 0;
	}
	return 1 + (number_of_items - kMinimumItemsForPruning) / 2;
}
auto isNumber(std::string const& str) -> bool {
	if (str.empty()) {
		return false;
	}
	for (auto c : str) {
		if (!std::isdigit(c)) {
			return false;
		}
	}
	return true;
}
auto parseWords(std::string const& str) -> std::vector<std::string> {
	std::stringstream stream(str);
	std::vector<std::string> words{};
	std::string word{};
	while (stream >> word) {
		words.push_back(word);
	}
	return words;
}
auto parseNumber(std::string const& str) -> std::optional<uint32_t> {
	if (!isNumber(str)) {
		printError("Unable to parse '" + str + "'. Not a number");
		return std::nullopt;
	}
	return static_cast<uint32_t>(std::stoul(str));
}
