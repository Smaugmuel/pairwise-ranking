#include <iostream>
#include <sstream>
#include <string>

class OutputCatcher final {
public:
	OutputCatcher() : original_buffer{ std::cout.rdbuf(stream.rdbuf()) } {
	}
	void stop() {
		std::cout.rdbuf(original_buffer);
	}
	auto output() const -> std::string {
		return stream.str();
	}
	auto contains(std::string const& str, size_t occurrences = 1) const -> bool {
		if (occurrences == 0) {
			return output().find(str) == std::string::npos;
		}
		size_t offset = 0;
		for (size_t i = 0; i < occurrences; i++) {
			size_t idx = output().find(str, offset);
			if (idx == std::string::npos) {
				return false;
			}
			offset = idx + str.size();
		}
		return true;
	}
private:
	std::stringstream stream{};
	std::streambuf* original_buffer{};
};
