#ifndef JSON_VALUE_H
#define JSON_VALUE_H

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <stdexcept>

// Forward declaration для рекурсивной структуры
class JsonValue;

// Типы JSON
using JsonNull = std::nullptr_t;
using JsonBool = bool;
using JsonNumber = double;
using JsonString = std::string;
using JsonArray = std::vector<JsonValue>;
using JsonObject = std::map<std::string, JsonValue>;

// Основная структура данных
class JsonValue {
public:
    using ValueType = std::variant<
        JsonNull,
        JsonBool,
        JsonNumber,
        JsonString,
        JsonArray,
        JsonObject
    >;

    JsonValue() : value(nullptr) {}
    JsonValue(JsonNull n) : value(n) {}
    JsonValue(JsonBool b) : value(b) {}
    JsonValue(JsonNumber n) : value(n) {}
    JsonValue(const JsonString& s) : value(s) {}
    JsonValue(const JsonArray& a) : value(a) {}
    JsonValue(const JsonObject& o) : value(o) {}

    // Проверка типов
    bool isNull() const { return std::holds_alternative<JsonNull>(value); }
    bool isBool() const { return std::holds_alternative<JsonBool>(value); }
    bool isNumber() const { return std::holds_alternative<JsonNumber>(value); }
    bool isString() const { return std::holds_alternative<JsonString>(value); }
    bool isArray() const { return std::holds_alternative<JsonArray>(value); }
    bool isObject() const { return std::holds_alternative<JsonObject>(value); }

    // Получение значений
    JsonBool asBool() const { return std::get<JsonBool>(value); }
    JsonNumber asNumber() const { return std::get<JsonNumber>(value); }
    const JsonString& asString() const { return std::get<JsonString>(value); }
    const JsonArray& asArray() const { return std::get<JsonArray>(value); }
    const JsonObject& asObject() const { return std::get<JsonObject>(value); }

    // Доступ к элементам (будет реализован одногруппником)
    JsonValue& operator[](const std::string& key);
    const JsonValue& operator[](const std::string& key) const;
    JsonValue& operator[](size_t index);
    const JsonValue& operator[](size_t index) const;

    ValueType value;
};

// Функции парсинга (ваша часть)
class JsonParser {
public:
    // Парсинг примитивов
    static JsonValue parseString(const std::string& input, size_t& pos);
    static JsonValue parseNumber(const std::string& input, size_t& pos);
    static JsonValue parseBool(const std::string& input, size_t& pos);
    static JsonValue parseNull(const std::string& input, size_t& pos);
    
    // Обработка escape-последовательностей
    static std::string unescapeString(const std::string& escaped);
    
    // Вспомогательные функции
    static void skipWhitespace(const std::string& input, size_t& pos);
    
    // Основной парсинг (будет реализован одногруппником)
    static JsonValue parse(const std::string& input);
    static JsonValue parseValue(const std::string& input, size_t& pos);
    static JsonValue parseArray(const std::string& input, size_t& pos);
    static JsonValue parseObject(const std::string& input, size_t& pos);
};

#endif // JSON_VALUE_H