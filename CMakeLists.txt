cmake_minimum_required(VERSION 3.20)
project(BxScript)

set(CMAKE_CXX_STANDARD 17)

add_executable(BxScript main.cpp
        libs/json.hpp
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
        evaluator/values/StringValue.cpp
        evaluator/values/ObjectValue.cpp
        evaluator/values/ArrayValue.cpp
        evaluator/values/NumberValue.cpp
        evaluator/values/FunctionValue.cpp
        evaluator/values/BooleanValue.cpp
        evaluator/values/NullValue.cpp
        evaluator/values/BufferValue.cpp
        evaluator/Interpreter.cpp
        stdlib/DateModule.h
        stdlib/ThreadModule.h
        stdlib/MathModule.h
        stdlib/JsonModule.h
        stdlib/IOModule.h
        stdlib/CryptModule.h
        stdlib/NetModule.h
        libs/md5/md5.cpp
        libs/md5/md5.h
        libs/sha256/sha256.cpp
        libs/sha256/sha256.h
        libs/hmac.h
        evaluator/values/BufferValue.cpp
        common/TimeKit.h
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
        libs/json.hpp
        tests/test_parser.cpp
        parser/Parser.cpp
        lexer/Lexer.cpp
        tests/test_interpreter.cpp
        evaluator/Environment.h
        evaluator/Interpreter.h
        evaluator/Value.h
        evaluator/Value.cpp
        common/ModuleHelper.h
        evaluator/values/StringValue.cpp
        evaluator/values/ObjectValue.cpp
        evaluator/values/ArrayValue.cpp
        evaluator/values/NumberValue.cpp
        evaluator/values/FunctionValue.cpp
        evaluator/values/BooleanValue.cpp
        evaluator/values/NullValue.cpp
        evaluator/values/BufferValue.cpp
        evaluator/Interpreter.cpp
        stdlib/DateModule.h
        stdlib/ThreadModule.h
        stdlib/MathModule.h
        stdlib/JsonModule.h
        stdlib/IOModule.h
        stdlib/CryptModule.h
        stdlib/NetModule.h
        libs/md5/md5.cpp
        libs/md5/md5.h
        libs/sha256/sha256.cpp
        libs/sha256/sha256.h
        libs/hmac.h
        evaluator/values/BufferValue.cpp
        common/TimeKit.h
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