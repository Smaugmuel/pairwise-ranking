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
	auto contains(std::string const& str) const -> bool {
		return output().find(str) != std::string::npos;
	}
	auto occurrences(std::string const& str) const -> uint32_t {
		size_t offset = output().find(str, 0);
		uint32_t count = 0;
		while (offset != std::string::npos) {
			count++;
			offset = output().find(str, offset + str.size());
		}
		return count;
	}
private:
	std::stringstream stream{};
	std::streambuf* original_buffer{};
};
