#include "JsonValue.h"
#include <gtest/gtest.h>

// Тест парсинга строк
TEST(JsonParserTest, ParseString) {
    std::string input = "\"hello world\"";
    size_t pos = 0;
    JsonValue result = JsonParser::parseString(input, pos);
    
    EXPECT_TRUE(result.isString());
    EXPECT_EQ(result.asString(), "hello world");
}

// Тест escape-последовательностей
TEST(JsonParserTest, ParseStringWithEscapes) {
    std::string input = "\"Hello\\nWorld\\t!\"";
    size_t pos = 0;
    JsonValue result = JsonParser::parseString(input, pos);
    
    EXPECT_TRUE(result.isString());
    EXPECT_EQ(result.asString(), "Hello\nWorld\t!");
}

TEST(JsonParserTest, ParseStringWithQuotes) {
    std::string input = "\"Say \\\"Hello\\\"\"";
    size_t pos = 0;
    JsonValue result = JsonParser::parseString(input, pos);
    
    EXPECT_TRUE(result.isString());
    EXPECT_EQ(result.asString(), "Say \"Hello\"");
}

// Тест парсинга чисел
TEST(JsonParserTest, ParseInteger) {
    std::string input = "42";
    size_t pos = 0;
    JsonValue result = JsonParser::parseNumber(input, pos);
    
    EXPECT_TRUE(result.isNumber());
    EXPECT_DOUBLE_EQ(result.asNumber(), 42.0);
}

TEST(JsonParserTest, ParseNegativeNumber) {
    std::string input = "-123.456";
    size_t pos = 0;
    JsonValue result = JsonParser::parseNumber(input, pos);
    
    EXPECT_TRUE(result.isNumber());
    EXPECT_DOUBLE_EQ(result.asNumber(), -123.456);
}

TEST(JsonParserTest, ParseNumberWithExponent) {
    std::string input = "1.5e2";
    size_t pos = 0;
    JsonValue result = JsonParser::parseNumber(input, pos);
    
    EXPECT_TRUE(result.isNumber());
    EXPECT_DOUBLE_EQ(result.asNumber(), 150.0);
}

// Тест парсинга boolean
TEST(JsonParserTest, ParseTrue) {
    std::string input = "true";
    size_t pos = 0;
    JsonValue result = JsonParser::parseBool(input, pos);
    
    EXPECT_TRUE(result.isBool());
    EXPECT_TRUE(result.asBool());
}

TEST(JsonParserTest, ParseFalse) {
    std::string input = "false";
    size_t pos = 0;
    JsonValue result = JsonParser::parseBool(input, pos);
    
    EXPECT_TRUE(result.isBool());
    EXPECT_FALSE(result.asBool());
}

// Тест парсинга null
TEST(JsonParserTest, ParseNull) {
    std::string input = "null";
    size_t pos = 0;
    JsonValue result = JsonParser::parseNull(input, pos);
    
    EXPECT_TRUE(result.isNull());
}

// Тест обработки пробелов
TEST(JsonParserTest, SkipWhitespace) {
    std::string input = "   \t\n  hello";
    size_t pos = 0;
    JsonParser::skipWhitespace(input, pos);
    
    EXPECT_EQ(pos, 7);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}