#include "keyboard_input.h"

#include <queue>

std::queue<char> g_keys{};
std::queue<std::string> g_lines{};

auto getKey() -> char {
	if (g_keys.empty()) {
		exit(1);
	}
	auto const key = g_keys.front();
	g_keys.pop();
	return key;
}
auto getConfirmation() -> bool {
	return {};
}
auto getLine() -> std::string {
	if (g_lines.empty()) {
		exit(1);
	}
	auto const line = g_lines.front();
	g_lines.pop();
	return line;
}
