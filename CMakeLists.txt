cmake_minimum_required(VERSION 3.14)
project(JsonParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поддержка потоков
find_package(Threads REQUIRED)

# Основная библиотека
add_library(jsonparser
    JsonValue.cpp
)

target_link_libraries(jsonparser Threads::Threads)

# Главная программа
add_executable(main
    main.cpp
)

target_link_libraries(main jsonparser)

# Генератор тестовых данных
add_executable(generate_data
    generate_test_data.cpp
)

# Программа для бенчмарка
add_executable(benchmark
    benchmark.cpp
)

target_link_libraries(benchmark jsonparser Threads::Threads)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Тесты для примитивов
add_executable(test_primitives
    test_primitives.cpp
)

target_link_libraries(test_primitives
    jsonparser
    GTest::gtest_main
)

# Тесты для парсера
add_executable(test_parser
    test_parser.cpp
)

target_link_libraries(test_parser
    jsonparser
    GTest::gtest_main
)

# Тесты для многопоточности
add_executable(test_multithread
    test_multithread.cpp
)

target_link_libraries(test_multithread
    jsonparser
    GTest::gtest_main
    Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(test_primitives)
gtest_discover_tests(test_parser)
gtest_discover_tests(test_multithread)