cmake_minimum_required(VERSION 3.20)
project(BxScript)

set(CMAKE_CXX_STANDARD 17)

add_executable(BxScript main.cpp
        lexer/Token.h
        common/TokenType.h
        lexer/Lexer.cpp
        lexer/Lexer.h
        common/KeyWord.h
        common/Symbols.h
        parser/Expression.h
        parser/Parser.cpp
        parser/Parser.h
        parser/ParserVM.h
        evaluator/Value.h
        evaluator/Value.cpp
        evaluator/Environment.h
        evaluator/Interpreter.h
        evaluator/Logger.h
        common/ModuleHelper.h
        common/StringKit.h
)

target_include_directories(BxScript PRIVATE
        ${PROJECT_SOURCE_DIR}
)

#-----------------------------------------
# GoogleTest 引入
#-----------------------------------------
enable_testing()
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

#-----------------------------------------
# 测试
#-----------------------------------------
add_executable(test_parser
        tests/test_parser.cpp
        parser/Parser.cpp
        lexer/Lexer.cpp
        tests/test_interpreter.cpp
        evaluator/Environment.h
        evaluator/Interpreter.h
        evaluator/Value.h
        evaluator/Value.cpp
        common/ModuleHelper.h
)

target_include_directories(test_parser PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(test_parser
        PRIVATE
        gtest_main
)

#-----------------------------------------
# gtest discover
#-----------------------------------------
include(GoogleTest)
gtest_discover_tests(test_parser)