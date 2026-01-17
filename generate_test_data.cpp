#include <iostream>
#include <fstream>
#include <string>
#include <random>

// Генератор больших JSON файлов для тестирования
class JsonDataGenerator {
private:
    std::mt19937 rng;
    std::uniform_int_distribution<int> intDist;
    std::uniform_real_distribution<double> doubleDist;
    std::uniform_int_distribution<int> boolDist;
    std::uniform_int_distribution<int> stringLengthDist;

public:
    JsonDataGenerator() 
        : rng(std::random_device{}()),
          intDist(1, 10000),
          doubleDist(0.0, 10000.0),
          boolDist(0, 1),
          stringLengthDist(5, 20) {}

    // Генерация случайной строки
    std::string generateRandomString(int length) {
        static const char charset[] = 
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789";
        
        std::string result;
        result.reserve(length);
        
        for (int i = 0; i < length; i++) {
            result += charset[rng() % (sizeof(charset) - 1)];
        }
        
        return result;
    }

    // Генерация одного JSON объекта
    std::string generateJsonObject() {
        std::string json = "{";
        
        // ID
        json += "\"id\":" + std::to_string(intDist(rng)) + ",";
        
        // Name
        int nameLength = stringLengthDist(rng);
        json += "\"name\":\"" + generateRandomString(nameLength) + "\",";
        
        // Age
        json += "\"age\":" + std::to_string(intDist(rng) % 100) + ",";
        
        // Email
        json += "\"email\":\"user" + std::to_string(intDist(rng)) + "@example.com\",";
        
        // Score
        json += "\"score\":" + std::to_string(doubleDist(rng)) + ",";
        
        // Active
        json += "\"active\":" + std::string(boolDist(rng) ? "true" : "false") + ",";
        
        // Address object
        json += "\"address\":{";
        json += "\"city\":\"" + generateRandomString(10) + "\",";
        json += "\"street\":\"" + generateRandomString(15) + "\",";
        json += "\"zipcode\":\"" + std::to_string(10000 + intDist(rng) % 90000) + "\"";
        json += "},";
        
        // Tags array
        json += "\"tags\":[";
        int tagCount = 3 + (rng() % 5);
        for (int i = 0; i < tagCount; i++) {
            json += "\"" + generateRandomString(8) + "\"";
            if (i < tagCount - 1) json += ",";
        }
        json += "]";
        
        json += "}";
        return json;
    }

    // Генерация большого JSON файла
    void generateLargeJsonFile(const std::string& filename, size_t targetSizeMB) {
        std::cout << "Generating " << targetSizeMB << " MB JSON file..." << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot create file " << filename << std::endl;
            return;
        }

        file << "[";
        
        size_t currentSize = 1; // для '['
        size_t targetSize = targetSizeMB * 1024 * 1024;
        size_t objectCount = 0;
        bool firstObject = true;

        while (currentSize < targetSize) {
            if (!firstObject) {
                file << ",";
                currentSize += 1;
            }
            firstObject = false;

            std::string obj = generateJsonObject();
            file << obj;
            currentSize += obj.size();
            objectCount++;

            // Прогресс каждые 10000 объектов
            if (objectCount % 10000 == 0) {
                double progress = (currentSize * 100.0) / targetSize;
                std::cout << "Progress: " << progress << "% (" 
                          << objectCount << " objects, "
                          << (currentSize / 1024.0 / 1024.0) << " MB)" << std::endl;
            }
        }

        file << "]";
        file.close();

        std::cout << "\nGeneration complete!" << std::endl;
        std::cout << "File: " << filename << std::endl;
        std::cout << "Objects: " << objectCount << std::endl;
        std::cout << "Size: " << (currentSize / 1024.0 / 1024.0) << " MB" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    size_t sizeMB = 500;
    std::string filename = "test_data_500mb.json";

    // Проверка аргументов командной строки
    if (argc > 1) {
        sizeMB = std::stoi(argv[1]);
    }
    if (argc > 2) {
        filename = argv[2];
    }

    std::cout << "JSON Data Generator" << std::endl;
    std::cout << "===================" << std::endl;
    
    JsonDataGenerator generator;
    generator.generateLargeJsonFile(filename, sizeMB);

    return 0;
}