#ifndef MULTITHREAD_PARSER_H
#define MULTITHREAD_PARSER_H

#include "JsonValue.h"
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

// Класс для многопоточного парсинга JSON-массивов
class MultiThreadParser {
public:
    // Структура для результатов бенчмарка
    struct BenchmarkResult {
        double singleThreadTime;  // Время однопоточной обработки (мс)
        double multiThreadTime;   // Время многопоточной обработки (мс)
        double speedup;           // Ускорение (singleThread / multiThread)
        size_t itemsProcessed;    // Количество обработанных элементов
        size_t threadCount;       // Количество потоков
    };

    // Парсинг массива JSON в несколько потоков
    static JsonArray parseArrayMultiThread(const std::string& jsonArrayStr, size_t threadCount = 4) {
        // Сначала парсим весь массив чтобы получить элементы
        JsonValue parsed = JsonParser::parse(jsonArrayStr);
        
        if (!parsed.isArray()) {
            throw std::runtime_error("Input is not a JSON array");
        }
        
        const JsonArray& sourceArray = parsed.asArray();
        return processArrayInParallel(sourceArray, threadCount);
    }

    // Обработка массива батчами в несколько потоков
    static JsonArray processArrayInParallel(const JsonArray& array, size_t threadCount = 4) {
        if (array.empty()) {
            return JsonArray();
        }

        // Ограничиваем количество потоков
        threadCount = std::min(threadCount, array.size());
        threadCount = std::max(size_t(1), threadCount);

        // Размер батча для каждого потока
        size_t batchSize = (array.size() + threadCount - 1) / threadCount;

        // Результаты для каждого потока
        std::vector<JsonArray> threadResults(threadCount);
        std::vector<std::thread> threads;
        std::mutex coutMutex;  // Для безопасного вывода

        // Запускаем потоки
        for (size_t i = 0; i < threadCount; i++) {
            size_t start = i * batchSize;
            size_t end = std::min(start + batchSize, array.size());

            if (start >= array.size()) break;

            threads.emplace_back([&, i, start, end]() {
                // Обработка батча
                JsonArray localResult;
                for (size_t j = start; j < end; j++) {
                    // Здесь можно добавить сложную обработку каждого элемента
                    localResult.push_back(array[j]);
                }
                threadResults[i] = std::move(localResult);

                // Безопасный вывод
                {
                    std::lock_guard<std::mutex> lock(coutMutex);
                    std::cout << "Thread " << i << " processed items [" 
                              << start << " - " << end << ")" << std::endl;
                }
            });
        }

        // Ждем завершения всех потоков
        for (auto& thread : threads) {
            thread.join();
        }

        // Объединяем результаты
        JsonArray finalResult;
        for (const auto& result : threadResults) {
            for (const auto& item : result) {
                finalResult.push_back(item);
            }
        }

        return finalResult;
    }

    // Бенчмарк: сравнение однопоточной и многопоточной обработки
    static BenchmarkResult benchmark(const std::string& jsonArrayStr, size_t threadCount = 4) {
        BenchmarkResult result;
        result.threadCount = threadCount;

        std::cout << "\n=== JSON Parser Benchmark ===" << std::endl;
        std::cout << "Parsing JSON array..." << std::endl;

        // Парсим массив один раз
        JsonValue parsed = JsonParser::parse(jsonArrayStr);
        if (!parsed.isArray()) {
            throw std::runtime_error("Input is not a JSON array");
        }
        const JsonArray& array = parsed.asArray();
        result.itemsProcessed = array.size();

        std::cout << "Array size: " << array.size() << " items" << std::endl;
        std::cout << "\n--- Single Thread ---" << std::endl;

        // Однопоточная обработка
        auto start = std::chrono::high_resolution_clock::now();
        JsonArray singleThreadResult = processArrayInParallel(array, 1);
        auto end = std::chrono::high_resolution_clock::now();
        
        result.singleThreadTime = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << "Time: " << result.singleThreadTime << " ms" << std::endl;

        std::cout << "\n--- Multi Thread (" << threadCount << " threads) ---" << std::endl;

        // Многопоточная обработка
        start = std::chrono::high_resolution_clock::now();
        JsonArray multiThreadResult = processArrayInParallel(array, threadCount);
        end = std::chrono::high_resolution_clock::now();
        
        result.multiThreadTime = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << "Time: " << result.multiThreadTime << " ms" << std::endl;

        // Вычисляем ускорение
        result.speedup = result.singleThreadTime / result.multiThreadTime;

        std::cout << "\n=== Results ===" << std::endl;
        std::cout << "Speedup: " << result.speedup << "x" << std::endl;
        std::cout << "Efficiency: " << (result.speedup / threadCount * 100.0) << "%" << std::endl;

        return result;
    }

    // Загрузка большого JSON файла и бенчмарк
    static BenchmarkResult benchmarkFile(const std::string& filename, size_t threadCount = 4) {
        std::cout << "Reading file: " << filename << std::endl;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonContent = buffer.str();
        file.close();

        std::cout << "File size: " << (jsonContent.size() / 1024.0 / 1024.0) << " MB" << std::endl;

        return benchmark(jsonContent, threadCount);
    }
};

#endif // MULTITHREAD_PARSER_H