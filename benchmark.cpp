#include "JsonValue.h"
#include "MultiThreadParser.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    std::string filename = "test_data_500mb.json";
    
    if (argc > 1) {
        filename = argv[1];
    }

    std::cout << "==================================" << std::endl;
    std::cout << "  JSON Parser Benchmark Tool" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;

    try {
        // Запускаем бенчмарк с разным количеством потоков
        std::cout << "Testing with 1 thread vs 4 threads..." << std::endl;
        std::cout << std::endl;

        auto result = MultiThreadParser::benchmarkFile(filename, 4);

        std::cout << "\n==================================" << std::endl;
        std::cout << "  Final Results" << std::endl;
        std::cout << "==================================" << std::endl;
        std::cout << "Items processed: " << result.itemsProcessed << std::endl;
        std::cout << "Single thread time: " << result.singleThreadTime << " ms" << std::endl;
        std::cout << "Multi thread time (4): " << result.multiThreadTime << " ms" << std::endl;
        std::cout << "Speedup: " << result.speedup << "x" << std::endl;
        std::cout << "Efficiency: " << (result.speedup / 4.0 * 100.0) << "%" << std::endl;
        std::cout << std::endl;

        // Дополнительный тест с 2 потоками
        std::cout << "\n--- Testing with 2 threads ---" << std::endl;
        auto result2 = MultiThreadParser::benchmarkFile(filename, 2);
        std::cout << "Speedup (2 threads): " << result2.speedup << "x" << std::endl;
        
        // Дополнительный тест с 8 потоками
        std::cout << "\n--- Testing with 8 threads ---" << std::endl;
        auto result8 = MultiThreadParser::benchmarkFile(filename, 8);
        std::cout << "Speedup (8 threads): " << result8.speedup << "x" << std::endl;

        std::cout << "\n==================================" << std::endl;
        std::cout << "  Summary" << std::endl;
        std::cout << "==================================" << std::endl;
        std::cout << "1 thread:  " << result.singleThreadTime << " ms (baseline)" << std::endl;
        std::cout << "2 threads: " << result2.multiThreadTime << " ms (" << result2.speedup << "x)" << std::endl;
        std::cout << "4 threads: " << result.multiThreadTime << " ms (" << result.speedup << "x)" << std::endl;
        std::cout << "8 threads: " << result8.multiThreadTime << " ms (" << result8.speedup << "x)" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}