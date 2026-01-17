#include "JsonValue.h"
#include "MultiThreadParser.h"
#include <gtest/gtest.h>

// Тест базовой многопоточной обработки
TEST(MultiThreadTest, BasicMultiThread) {
    std::string json = R"([
        {"id": 1, "name": "Item1"},
        {"id": 2, "name": "Item2"},
        {"id": 3, "name": "Item3"},
        {"id": 4, "name": "Item4"}
    ])";
    
    JsonArray result = MultiThreadParser::parseArrayMultiThread(json, 2);
    
    EXPECT_EQ(result.size(), 4);
    EXPECT_EQ(result[0]["id"].asNumber(), 1.0);
    EXPECT_EQ(result[3]["name"].asString(), "Item4");
}

// Тест с одним потоком (должен работать как обычный парсер)
TEST(MultiThreadTest, SingleThread) {
    std::string json = R"([1, 2, 3, 4, 5])";
    
    JsonArray result = MultiThreadParser::parseArrayMultiThread(json, 1);
    
    EXPECT_EQ(result.size(), 5);
    EXPECT_DOUBLE_EQ(result[0].asNumber(), 1.0);
}

// Тест с большим количеством элементов
TEST(MultiThreadTest, LargeArray) {
    // Генерируем массив из 1000 элементов
    std::string json = "[";
    for (int i = 0; i < 1000; i++) {
        json += std::to_string(i);
        if (i < 999) json += ",";
    }
    json += "]";
    
    JsonArray result = MultiThreadParser::parseArrayMultiThread(json, 4);
    
    EXPECT_EQ(result.size(), 1000);
    EXPECT_DOUBLE_EQ(result[0].asNumber(), 0.0);
    EXPECT_DOUBLE_EQ(result[999].asNumber(), 999.0);
}

// Тест пустого массива
TEST(MultiThreadTest, EmptyArray) {
    std::string json = "[]";
    
    JsonArray result = MultiThreadParser::parseArrayMultiThread(json, 4);
    
    EXPECT_EQ(result.size(), 0);
}

// Тест обработки батчей
TEST(MultiThreadTest, BatchProcessing) {
    JsonValue parsed = JsonParser::parse("[1,2,3,4,5,6,7,8,9,10]");
    const JsonArray& array = parsed.asArray();
    
    // Обработка в 3 потока
    JsonArray result = MultiThreadParser::processArrayInParallel(array, 3);
    
    EXPECT_EQ(result.size(), 10);
}

// Тест что многопоточная версия дает тот же результат
TEST(MultiThreadTest, SameResultAsSerial) {
    std::string json = R"([
        {"x": 1}, {"x": 2}, {"x": 3}, {"x": 4},
        {"x": 5}, {"x": 6}, {"x": 7}, {"x": 8}
    ])";
    
    JsonArray serial = MultiThreadParser::parseArrayMultiThread(json, 1);
    JsonArray parallel = MultiThreadParser::parseArrayMultiThread(json, 4);
    
    EXPECT_EQ(serial.size(), parallel.size());
    
    for (size_t i = 0; i < serial.size(); i++) {
        EXPECT_DOUBLE_EQ(
            serial[i]["x"].asNumber(),
            parallel[i]["x"].asNumber()
        );
    }
}

// Тест бенчмарка (просто проверяем что не падает)
TEST(MultiThreadTest, BenchmarkRuns) {
    std::string json = "[";
    for (int i = 0; i < 100; i++) {
        json += "{\"id\":" + std::to_string(i) + "}";
        if (i < 99) json += ",";
    }
    json += "]";
    
    EXPECT_NO_THROW({
        auto result = MultiThreadParser::benchmark(json, 2);
        EXPECT_GT(result.itemsProcessed, 0);
        EXPECT_GT(result.singleThreadTime, 0);
        EXPECT_GT(result.multiThreadTime, 0);
    });
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}