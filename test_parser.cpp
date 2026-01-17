#include "JsonValue.h"
#include <gtest/gtest.h>
#include <fstream>

// Тест парсинга простого объекта
TEST(JsonParserTest, ParseSimpleObject) {
    std::string json = R"({"name": "John", "age": 30})";
    JsonValue result = JsonParser::parse(json);
    
    EXPECT_TRUE(result.isObject());
    EXPECT_EQ(result["name"].asString(), "John");
    EXPECT_DOUBLE_EQ(result["age"].asNumber(), 30.0);
}

// Тест парсинга простого массива
TEST(JsonParserTest, ParseSimpleArray) {
    std::string json = R"([1, 2, 3, 4, 5])";
    JsonValue result = JsonParser::parse(json);
    
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.asArray().size(), 5);
    EXPECT_DOUBLE_EQ(result[0].asNumber(), 1.0);
    EXPECT_DOUBLE_EQ(result[4].asNumber(), 5.0);
}

// Тест вложенных объектов
TEST(JsonParserTest, ParseNestedObject) {
    std::string json = R"({
        "user": {
            "name": "Alice",
            "address": {
                "city": "Moscow",
                "street": "Lenina"
            }
        }
    })";
    
    JsonValue result = JsonParser::parse(json);
    
    EXPECT_TRUE(result.isObject());
    EXPECT_EQ(result["user"]["name"].asString(), "Alice");
    EXPECT_EQ(result["user"]["address"]["city"].asString(), "Moscow");
    EXPECT_EQ(result["user"]["address"]["street"].asString(), "Lenina");
}

// Тест массива объектов
TEST(JsonParserTest, ParseArrayOfObjects) {
    std::string json = R"([
        {"id": 1, "name": "Item1"},
        {"id": 2, "name": "Item2"}
    ])";
    
    JsonValue result = JsonParser::parse(json);
    
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.asArray().size(), 2);
    EXPECT_DOUBLE_EQ(result[0]["id"].asNumber(), 1.0);
    EXPECT_EQ(result[1]["name"].asString(), "Item2");
}

// Тест смешанных типов
TEST(JsonParserTest, ParseMixedTypes) {
    std::string json = R"({
        "string": "hello",
        "number": 42,
        "bool": true,
        "null": null,
        "array": [1, 2, 3],
        "object": {"key": "value"}
    })";
    
    JsonValue result = JsonParser::parse(json);
    
    EXPECT_TRUE(result["string"].isString());
    EXPECT_TRUE(result["number"].isNumber());
    EXPECT_TRUE(result["bool"].isBool());
    EXPECT_TRUE(result["null"].isNull());
    EXPECT_TRUE(result["array"].isArray());
    EXPECT_TRUE(result["object"].isObject());
}

// Тест пустых структур
TEST(JsonParserTest, ParseEmptyStructures) {
    std::string json1 = "{}";
    JsonValue result1 = JsonParser::parse(json1);
    EXPECT_TRUE(result1.isObject());
    EXPECT_EQ(result1.asObject().size(), 0);
    
    std::string json2 = "[]";
    JsonValue result2 = JsonParser::parse(json2);
    EXPECT_TRUE(result2.isArray());
    EXPECT_EQ(result2.asArray().size(), 0);
}

// Тест обработки ошибок
TEST(JsonParserTest, InvalidJson) {
    EXPECT_THROW(JsonParser::parse("{invalid}"), std::runtime_error);
    EXPECT_THROW(JsonParser::parse("[1, 2,]"), std::runtime_error);
    EXPECT_THROW(JsonParser::parse("{\"key\"}"), std::runtime_error);
}

// Тест доступа к несуществующему ключу
TEST(JsonParserTest, AccessNonExistentKey) {
    std::string json = R"({"name": "John"})";
    JsonValue result = JsonParser::parse(json);
    
    EXPECT_THROW(result["age"], std::runtime_error);
}

// Тест доступа к элементу не того типа
TEST(JsonParserTest, AccessWrongType) {
    std::string json = R"({"name": "John"})";
    JsonValue result = JsonParser::parse(json);
    
    EXPECT_THROW(result[0], std::runtime_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}