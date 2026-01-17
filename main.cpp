#include "JsonValue.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main() {
    try {
        // Пример 1: Парсинг строки
        std::string jsonStr = R"({
            "user": {
                "name": "Ivan",
                "age": 21,
                "address": {
                    "city": "Moscow",
                    "street": "Tverskaya"
                }
            },
            "courses": ["C++", "Python", "JavaScript"]
        })";
        
        JsonValue data = JsonParser::parse(jsonStr);
        
        std::cout << "Name: " << data["user"]["name"].asString() << std::endl;
        std::cout << "Age: " << data["user"]["age"].asNumber() << std::endl;
        std::cout << "City: " << data["user"]["address"]["city"].asString() << std::endl;
        
        std::cout << "\nCourses:" << std::endl;
        for (size_t i = 0; i < data["courses"].asArray().size(); i++) {
            std::cout << "  - " << data["courses"][i].asString() << std::endl;
        }
        
        // Пример 2: Чтение из файла
        std::ifstream file("test.json");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            
            JsonValue fileData = JsonParser::parse(buffer.str());
            std::cout << "\nFile parsed successfully!" << std::endl;
            
            file.close();
        } else {
            std::cout << "\nFile not found (это нормально для примера)" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}