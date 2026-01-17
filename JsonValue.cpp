#include "JsonValue.h"
#include <sstream>
#include <cctype>

// Пропуск пробелов (новый комментарий)
void JsonParser::skipWhitespace(const std::string& input, size_t& pos) {
    while (pos < input.length() && std::isspace(input[pos])) {
        pos++;
    }
}

// Обработка escape-последовательностей в строках
std::string JsonParser::unescapeString(const std::string& escaped) {
    std::string result;
    size_t i = 0;
    
    while (i < escaped.length()) {
        if (escaped[i] == '\\' && i + 1 < escaped.length()) {
            char next = escaped[i + 1];
            switch (next) {
                case '"':  result += '"'; break;
                case '\\': result += '\\'; break;
                case '/':  result += '/'; break;
                case 'b':  result += '\b'; break;
                case 'f':  result += '\f'; break;
                case 'n':  result += '\n'; break;
                case 'r':  result += '\r'; break;
                case 't':  result += '\t'; break;
                default:
                    throw std::runtime_error("Invalid escape sequence");
            }
            i += 2;
        } else {
            result += escaped[i];
            i++;
        }
    }
    
    return result;
}

// Парсинг строки
JsonValue JsonParser::parseString(const std::string& input, size_t& pos) {
    if (input[pos] != '"') {
        throw std::runtime_error("Expected '\"' at start of string");
    }
    
    pos++; // Пропускаем открывающую кавычку
    std::string result;
    
    while (pos < input.length() && input[pos] != '"') {
        if (input[pos] == '\\') {
            // Сохраняем escape-последовательность для обработки
            result += input[pos];
            pos++;
            if (pos < input.length()) {
                result += input[pos];
                pos++;
            }
        } else {
            result += input[pos];
            pos++;
        }
    }
    
    if (pos >= input.length()) {
        throw std::runtime_error("Unterminated string");
    }
    
    pos++; // Пропускаем закрывающую кавычку
    
    return JsonValue(unescapeString(result));
}

// Парсинг числа
JsonValue JsonParser::parseNumber(const std::string& input, size_t& pos) {
    size_t start = pos;
    
    // Минус
    if (pos < input.length() && input[pos] == '-') {
        pos++;
    }
    
    // Целая часть
    if (pos >= input.length() || !std::isdigit(input[pos])) {
        throw std::runtime_error("Invalid number format");
    }
    
    while (pos < input.length() && std::isdigit(input[pos])) {
        pos++;
    }
    
    // Дробная часть
    if (pos < input.length() && input[pos] == '.') {
        pos++;
        if (pos >= input.length() || !std::isdigit(input[pos])) {
            throw std::runtime_error("Invalid number format");
        }
        while (pos < input.length() && std::isdigit(input[pos])) {
            pos++;
        }
    }
    
    // Экспонента
    if (pos < input.length() && (input[pos] == 'e' || input[pos] == 'E')) {
        pos++;
        if (pos < input.length() && (input[pos] == '+' || input[pos] == '-')) {
            pos++;
        }
        if (pos >= input.length() || !std::isdigit(input[pos])) {
            throw std::runtime_error("Invalid number format");
        }
        while (pos < input.length() && std::isdigit(input[pos])) {
            pos++;
        }
    }
    
    std::string numStr = input.substr(start, pos - start);
    double num = std::stod(numStr);
    
    return JsonValue(num);
}

// Парсинг boolean
JsonValue JsonParser::parseBool(const std::string& input, size_t& pos) {
    if (input.substr(pos, 4) == "true") {
        pos += 4;
        return JsonValue(true);
    } else if (input.substr(pos, 5) == "false") {
        pos += 5;
        return JsonValue(false);
    }
    
    throw std::runtime_error("Invalid boolean value");
}

// Парсинг null
JsonValue JsonParser::parseNull(const std::string& input, size_t& pos) {
    if (input.substr(pos, 4) == "null") {
        pos += 4;
        return JsonValue(nullptr);
    }
    
    throw std::runtime_error("Invalid null value");
}
// Дополнение к JsonValue.cpp (часть одногруппника)

#include "JsonValue.h"
#include <fstream>
#include <sstream>

// Операторы доступа к элементам объекта
JsonValue& JsonValue::operator[](const std::string& key) {
    if (!isObject()) {
        throw std::runtime_error("Value is not an object");
    }
    return const_cast<JsonObject&>(std::get<JsonObject>(value))[key];
}

const JsonValue& JsonValue::operator[](const std::string& key) const {
    if (!isObject()) {
        throw std::runtime_error("Value is not an object");
    }
    auto& obj = std::get<JsonObject>(value);
    auto it = obj.find(key);
    if (it == obj.end()) {
        throw std::runtime_error("Key not found: " + key);
    }
    return it->second;
}

// Операторы доступа к элементам массива
JsonValue& JsonValue::operator[](size_t index) {
    if (!isArray()) {
        throw std::runtime_error("Value is not an array");
    }
    auto& arr = const_cast<JsonArray&>(std::get<JsonArray>(value));
    if (index >= arr.size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    return arr[index];
}

const JsonValue& JsonValue::operator[](size_t index) const {
    if (!isArray()) {
        throw std::runtime_error("Value is not an array");
    }
    auto& arr = std::get<JsonArray>(value);
    if (index >= arr.size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    return arr[index];
}

// Парсинг значения (рекурсивный)
JsonValue JsonParser::parseValue(const std::string& input, size_t& pos) {
    skipWhitespace(input, pos);
    
    if (pos >= input.length()) {
        throw std::runtime_error("Unexpected end of input");
    }
    
    char c = input[pos];
    
    if (c == '"') {
        return parseString(input, pos);
    } else if (c == '{') {
        return parseObject(input, pos);
    } else if (c == '[') {
        return parseArray(input, pos);
    } else if (c == 't' || c == 'f') {
        return parseBool(input, pos);
    } else if (c == 'n') {
        return parseNull(input, pos);
    } else if (c == '-' || std::isdigit(c)) {
        return parseNumber(input, pos);
    }
    
    throw std::runtime_error("Invalid JSON value");
}

// Парсинг массива
JsonValue JsonParser::parseArray(const std::string& input, size_t& pos) {
    if (input[pos] != '[') {
        throw std::runtime_error("Expected '['");
    }
    
    pos++; // Пропускаем '['
    JsonArray array;
    
    skipWhitespace(input, pos);
    
    // Пустой массив
    if (pos < input.length() && input[pos] == ']') {
        pos++;
        return JsonValue(array);
    }
    
    while (pos < input.length()) {
        // Парсим элемент
        array.push_back(parseValue(input, pos));
        
        skipWhitespace(input, pos);
        
        if (pos >= input.length()) {
            throw std::runtime_error("Unexpected end of array");
        }
        
        if (input[pos] == ']') {
            pos++;
            return JsonValue(array);
        }
        
        if (input[pos] != ',') {
            throw std::runtime_error("Expected ',' or ']' in array");
        }
        
        pos++; // Пропускаем ','
        skipWhitespace(input, pos);
    }
    
    throw std::runtime_error("Unterminated array");
}

// Парсинг объекта
JsonValue JsonParser::parseObject(const std::string& input, size_t& pos) {
    if (input[pos] != '{') {
        throw std::runtime_error("Expected '{'");
    }
    
    pos++; // Пропускаем '{'
    JsonObject object;
    
    skipWhitespace(input, pos);
    
    // Пустой объект
    if (pos < input.length() && input[pos] == '}') {
        pos++;
        return JsonValue(object);
    }
    
    while (pos < input.length()) {
        skipWhitespace(input, pos);
        
        // Парсим ключ
        if (input[pos] != '"') {
            throw std::runtime_error("Expected string key in object");
        }
        
        JsonValue keyValue = parseString(input, pos);
        std::string key = keyValue.asString();
        
        skipWhitespace(input, pos);
        
        if (pos >= input.length() || input[pos] != ':') {
            throw std::runtime_error("Expected ':' after key");
        }
        
        pos++; // Пропускаем ':'
        skipWhitespace(input, pos);
        
        // Парсим значение
        object[key] = parseValue(input, pos);
        
        skipWhitespace(input, pos);
        
        if (pos >= input.length()) {
            throw std::runtime_error("Unexpected end of object");
        }
        
        if (input[pos] == '}') {
            pos++;
            return JsonValue(object);
        }
        
        if (input[pos] != ',') {
            throw std::runtime_error("Expected ',' or '}' in object");
        }
        
        pos++; // Пропускаем ','
    }
    
    throw std::runtime_error("Unterminated object");
}

// Основная функция парсинга
JsonValue JsonParser::parse(const std::string& input) {
    size_t pos = 0;
    JsonValue result = parseValue(input, pos);
    
    skipWhitespace(input, pos);
    
    if (pos < input.length()) {
        throw std::runtime_error("Extra data after JSON value");
    }
    
    return result;
}