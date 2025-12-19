cmake_minimum_required(VERSION 3.20)
project(BxScript)

set(CMAKE_CXX_STANDARD 17)

# ==========================================
# 1. 跨平台资源嵌入 (自动生成汇编)
# ==========================================
set(FONT_FILE "${CMAKE_SOURCE_DIR}/font.ttf")
set(EMBED_FILE "${CMAKE_BINARY_DIR}/embedded_font.S")

# 检查字体文件是否存在，防止编译报错一脸懵
if(NOT EXISTS "${FONT_FILE}")
    message(WARNING "Font file not found at: ${FONT_FILE}. Embedding will fail.")
endif()

# 根据平台确定符号前缀
if(APPLE)
    set(ASM_SECTION ".section __TEXT,__const")
    set(SYM_PREFIX "_")
elseif(WIN32)
    set(ASM_SECTION ".section .rdata,\"dr\"")
    set(SYM_PREFIX "_")
else()
    set(ASM_SECTION ".section .rodata")
    set(SYM_PREFIX "")
endif()

# 生成汇编文件
file(WRITE ${EMBED_FILE} "
${ASM_SECTION}
.global ${SYM_PREFIX}bx_font_start
.global ${SYM_PREFIX}bx_font_end
.balign 16
${SYM_PREFIX}bx_font_start:
    .incbin \"${FONT_FILE}\"
${SYM_PREFIX}bx_font_end:
    .byte 0
")

# 开启 ASM 支持
enable_language(ASM)

# ==========================================
# 2. 编译器选项优化
# ==========================================
if (MINGW)
    add_compile_options(-Wa,-mbig-obj)
    add_compile_options(-Os -ffunction-sections -fdata-sections)
    add_link_options(-Wl,--gc-sections -s)
endif ()

# ==========================================
# 3. 引入 GLFW
# ==========================================
set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-src")
# 防止 GLFW 构建不必要的 Demo 和文档，加快编译
option(GLFW_BUILD_EXAMPLES "Build examples" OFF)
option(GLFW_BUILD_TESTS "Build tests" OFF)
option(GLFW_BUILD_DOCS "Build docs" OFF)
option(GLFW_INSTALL "Install" OFF)

add_subdirectory("${GLFW_DIR}")
include_directories("${GLFW_DIR}/deps")
include_directories("${GLFW_DIR}/include")

# ==========================================
# 4. 源文件列表
# ==========================================
set(SOURCE_FILES
        main.cpp
        libs/json.hpp
        lexer/Token.h
        common/TokenKind.h
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
        evaluator/Interpreter.cpp
        evaluator/Logger.h
        evaluator/EventLoop.h
        evaluator/values/StringValue.cpp
        evaluator/values/ObjectValue.cpp
        evaluator/values/ArrayValue.cpp
        evaluator/values/NumberValue.cpp
        evaluator/values/FunctionValue.cpp
        evaluator/values/BooleanValue.cpp
        evaluator/values/NullValue.cpp
        evaluator/values/BufferValue.cpp
        common/ModuleHelper.h
        common/StringKit.h
        common/TimeKit.h
        common/JsonKit.h
        common/FontKit.h       # 你的字体读取辅助类
        stdlib/DateModule.h
        stdlib/ThreadModule.h
        stdlib/MathModule.h
        stdlib/JsonModule.h
        stdlib/IOModule.h
        stdlib/CryptModule.h
        stdlib/NetModule.h
        stdlib/OsModule.h
        stdlib/RegexModule.h
        libs/md5/md5.cpp
        libs/md5/md5.h
        libs/sha256/sha256.cpp
        libs/sha256/sha256.h
        libs/hmac.h

        # === 关键修复：把生成的汇编文件加进来 ===
        ${EMBED_FILE}
)

# ==========================================
# 5. 主程序 BxScript
# ==========================================
add_executable(BxScript ${SOURCE_FILES})

target_include_directories(BxScript PRIVATE ${PROJECT_SOURCE_DIR})

# ==========================================
# 6. GoogleTest 引入
# ==========================================
enable_testing()
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

# ==========================================
# 7. 测试程序 test_parser
# ==========================================
# 从列表移除 main.cpp
list(REMOVE_ITEM SOURCE_FILES main.cpp)

add_executable(test_parser
        tests/test_parser.cpp
        tests/test_interpreter.cpp
        ${SOURCE_FILES} # 这里包含了 embedded_font.S
)

target_include_directories(test_parser PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(test_parser PRIVATE gtest_main)

# ==========================================
# 8. 平台链接配置
# ==========================================
if (WIN32)
    # BxScript 需要链接
    target_link_libraries(BxScript PRIVATE wininet glfw opengl32)

    # 测试程序也需要链接 (因为包含了 NetModule 等)
    target_link_libraries(test_parser PRIVATE wininet glfw opengl32)

    message(STATUS "Configuring for Windows: Linked wininet, glfw, opengl32")
endif ()

include(GoogleTest)
gtest_discover_tests(test_parser)