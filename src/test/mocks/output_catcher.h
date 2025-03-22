#include <iostream>
#include <sstream>
#include <string>

class OutputCatcher final {
public:
	OutputCatcher() : original_buffer{ std::cout.rdbuf(stream.rdbuf()) } {}
	~OutputCatcher() { std::cout.rdbuf(original_buffer); }
	auto operator==(std::string const& str) const -> bool { return stream.str() == str; }
private:
	std::stringstream stream{};
	std::streambuf* original_buffer{};
};
