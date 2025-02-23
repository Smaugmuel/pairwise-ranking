#include "testing.h"
#include "score_helpers.h"

namespace
{

void stringIsEmpty() {
	ASSERT_EQ(parseScores({ "" }), Scores{});
}
void stringIsValid() {
	ASSERT_EQ(parseScores({ "0 0 Item" }), Scores{ { "Item", 0, 0 } });
	ASSERT_EQ(parseScores({ "10 23 Item" }), Scores{ { "Item", 10, 23 } });
	ASSERT_EQ(parseScores({ "100 1 Item" }), Scores{ { "Item", 100, 1 } });
}
void itemIsAnInteger() {
	ASSERT_EQ(parseScores({ "100 1 3" }), Scores{ { "3", 100, 1 } });
	ASSERT_EQ(parseScores({ "100 1 -3" }), Scores{ { "-3", 100, 1 } });
}
void itemHasMultipleWords() {
	ASSERT_EQ(parseScores({ "0 0 Multi-word item" }), Scores{ { "Multi-word item", 0, 0} });
	ASSERT_EQ(parseScores({ "4 65 Multi-word item 1 3 4 with numbers" }), Scores{ { "Multi-word item 1 3 4 with numbers", 4, 65 } });
}
void itemHasIntegersAfterSpace() {
	ASSERT_EQ(parseScores({ "2 4 item 1" }), Scores{ { "item 1", 2, 4 } });
	ASSERT_EQ(parseScores({ "2 4 item 123" }), Scores{ { "item 123", 2, 4 } });
	ASSERT_EQ(parseScores({ "2 4 item 1 2 3" }), Scores{ { "item 1 2 3", 2, 4 } });
}
void stringMissingWinsOrLosses() {
	ASSERT_EQ(parseScores({ "100 item" }), Scores{});
	ASSERT_EQ(parseScores({ "item" }), Scores{});
}
void stringMissingItem() {
	ASSERT_EQ(parseScores({ "100" }), Scores{});
	ASSERT_EQ(parseScores({ "100 1" }), Scores{});
}
void winsOrLossesAreNegative() {
	ASSERT_EQ(parseScores({ "-10 23 Item" }), Scores{});
	ASSERT_EQ(parseScores({ "10 -23 Item" }), Scores{});
	ASSERT_EQ(parseScores({ "-10 -23 Item" }), Scores{});
}
void winsOrLossesAreAfterItem() {
	ASSERT_EQ(parseScores({ "100 item 1" }), Scores{});
	ASSERT_EQ(parseScores({ "item 100 1" }), Scores{});
}
void multipleItemsWithoutWinsOrLosses() {
	ASSERT_EQ(parseScores({ "item item2" }), Scores{});
	ASSERT_EQ(parseScores({ "item item2 item3" }), Scores{});
	ASSERT_EQ(parseScores({ "item 100 item2" }), Scores{});
}
void noStringsSent() {
	ASSERT_EQ(parseScores({}), Scores{});
}
void allStringsAreValid() {
	ASSERT_EQ(parseScores({ "2 4 item 1", "5 1 item 2" }), Scores{ Score{"item 1", 2, 4}, Score{"item 2", 5, 1} });
}
void onlyOneStringIsValid() {
	ASSERT_EQ(parseScores({ "2 4 item 1", "5 1" }), Scores{ Score{"item 1", 2, 4} });
	ASSERT_EQ(parseScores({ "5 1", "2 4 item 1" }), Scores{ Score{"item 1", 2, 4} });
}
void validNumbers() {
	ASSERT_EQ(parseScores({ "0 1 item1" }), Scores{ Score{ "item1", 0, 1 } });
	ASSERT_EQ(parseScores({ "0 2 item1" }), Scores{ Score{ "item1", 0, 2 } });
	ASSERT_EQ(parseScores({ "0 3 item1" }), Scores{ Score{ "item1", 0, 3 } });
	ASSERT_EQ(parseScores({ "0 4 item1" }), Scores{ Score{ "item1", 0, 4 } });
	ASSERT_EQ(parseScores({ "0 5 item1" }), Scores{ Score{ "item1", 0, 5 } });
	ASSERT_EQ(parseScores({ "0 6 item1" }), Scores{ Score{ "item1", 0, 6 } });
	ASSERT_EQ(parseScores({ "0 7 item1" }), Scores{ Score{ "item1", 0, 7 } });
	ASSERT_EQ(parseScores({ "0 8 item1" }), Scores{ Score{ "item1", 0, 8 } });
	ASSERT_EQ(parseScores({ "0 9 item1" }), Scores{ Score{ "item1", 0, 9 } });
	ASSERT_EQ(parseScores({ "0 12 item1" }), Scores{ Score{ "item1", 0, 12 } });
	ASSERT_EQ(parseScores({ "0 123 item1" }), Scores{ Score{ "item1", 0, 123 } });
	ASSERT_EQ(parseScores({ "0 1234 item1" }), Scores{ Score{ "item1", 0, 1234 } });
	ASSERT_EQ(parseScores({ "0 12345 item1" }), Scores{ Score{ "item1", 0, 12345 } });
	ASSERT_EQ(parseScores({ "0 123456 item1" }), Scores{ Score{ "item1", 0, 123456 } });
	ASSERT_EQ(parseScores({ "0 1234567 item1" }), Scores{ Score{ "item1", 0, 1234567 } });
	ASSERT_EQ(parseScores({ "0 12345678 item1" }), Scores{ Score{ "item1", 0, 12345678 } });
	ASSERT_EQ(parseScores({ "0 123456789 item1" }), Scores{ Score{ "item1", 0, 123456789 } });
	ASSERT_EQ(parseScores({ "0 1234567890 item1" }), Scores{ Score{ "item1", 0, 1234567890 } });
	// Could exhaust more, but it's likely unnecessary
}
void nonNumbers() {
	ASSERT_EQ(Scores{}, parseScores({ "0 a item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 a1 item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 1a item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 11a item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 1a1 item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 a11 item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 abc item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 ! item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 - item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 . item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 , item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 ? item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 _ item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 # item1" }));
	ASSERT_EQ(Scores{}, parseScores({ "0 @ item1" }));
	// Could exhaust more, but it's likely unnecessary
}
} // namespace

int main() {
	stringIsEmpty();
	stringIsValid();
	itemIsAnInteger();
	itemHasMultipleWords();
	itemHasIntegersAfterSpace();
	stringMissingWinsOrLosses();
	stringMissingItem();
	winsOrLossesAreNegative();
	winsOrLossesAreAfterItem();
	multipleItemsWithoutWinsOrLosses();
	noStringsSent();
	allStringsAreValid();
	onlyOneStringIsValid();
	validNumbers();
	nonNumbers();
	return 0;
}