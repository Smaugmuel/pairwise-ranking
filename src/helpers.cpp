#include "helpers.h"

#include <sstream>
#include <filesystem>
#include <fstream>

#include "constants.h"
#include "print.h"

auto sumOfFirstIntegers(size_t n) noexcept -> size_t {
	return (n * (n + 1)) / 2;
}
auto pruningAmount(uint32_t const number_of_items) noexcept -> uint32_t {
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
auto numberOfDigits(size_t n) noexcept -> size_t {
	if (n == 0) {
		return 1;
	}
	return static_cast<size_t>(std::log10(n)) + 1;
}
auto loadFile(std::string const& file_name) -> std::vector<std::string> {
	std::ifstream file(file_name);
	if (!file.is_open()) {
		printError("Could not open file \'" + file_name + "\' in " + std::filesystem::current_path().string());
		return {};
	}

	std::vector<std::string> lines{};
	std::string line;
	while (std::getline(file, line)) {
		lines.emplace_back(line);
	}
	file.close();
	return lines;
}
auto saveFile(std::string const& file_name, std::vector<std::string> const& lines) -> bool {
	if (lines.empty()) {
		printError("No lines to save");
		return false;
	}
	std::ofstream file(file_name);
	if (!file.is_open()) {
		printError("Could not create file " + file_name);
		return false;
	}
	for (auto const& line : lines) {
		file << line << '\n';
	}
	file.close();
	return true;
}
