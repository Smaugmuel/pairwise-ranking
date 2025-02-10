#include "functions.h"
#include "testing.h"

int main() {
	ASSERT_EQ(generateScoreFileData({}), std::vector<std::string>{});
	ASSERT_EQ(generateScoreFileData({ Score{ "item1", 3, 6 }}), std::vector<std::string>{
		"3 6 item1"
	});
	ASSERT_EQ(generateScoreFileData({ Score{ "item with space", 3, 6 } }), std::vector<std::string>{
		"3 6 item with space"
	});
	ASSERT_EQ(generateScoreFileData({ Score{ "item1", 3, 6 }, Score{ "item2", 71, 4 } }), std::vector<std::string>{
		"3 6 item1",
		"71 4 item2"
	});

	return 0;
}