cmake_minimum_required(VERSION 3.14)
project(JsonParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основная библиотека
add_library(jsonparser
    JsonValue.cpp
)

# Главная программа
add_executable(main
    main.cpp
)

target_link_libraries(main jsonparser)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Тесты для примитивов (ваша часть)
add_executable(test_primitives
    test_primitives.cpp
)

target_link_libraries(test_primitives
    jsonparser
    GTest::gtest_main
)

# Тесты для парсера (часть одногруппника)
add_executable(test_parser
    test_parser.cpp
)

target_link_libraries(test_parser
    jsonparser
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_primitives)
gtest_discover_tests(test_parser)