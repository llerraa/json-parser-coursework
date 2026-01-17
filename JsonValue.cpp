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