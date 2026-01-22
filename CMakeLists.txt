cmake_minimum_required(VERSION 3.20)
project(BxScript)

set(CMAKE_CXX_STANDARD 17)

# ==========================================
# 1. 编译器选项优化
# ==========================================
if (MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-Wa,-mbig-obj)
        add_compile_options(-Os -ffunction-sections -fdata-sections)
        add_link_options(-Wl,--gc-sections -s)
    endif ()
endif ()

# ==========================================
# 2. 平台特定配置
# ==========================================
set(PLATFORM_SOURCES "")
set(PLATFORM_LIBS "")

if (WIN32)
    message(STATUS "Configuring for Windows...")
    set(MANIFEST_CONTENT "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>
    <assembly xmlns='urn:schemas-microsoft-com:asm.v1' manifestVersion='1.0'>
      <dependency>
        <dependentAssembly>
          <assemblyIdentity type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'/>
        </dependentAssembly>
      </dependency>
      <trustInfo xmlns='urn:schemas-microsoft-com:asm.v3'>
        <security>
          <requestedPrivileges>
            <requestedExecutionLevel level='asInvoker' uiAccess='false'/>
          </requestedPrivileges>
        </security>
      </trustInfo>
      <application xmlns='urn:schemas-microsoft-com:asm.v3'>
        <windowsSettings>
          <dpiAware xmlns='http://schemas.microsoft.com/SMI/2005/WindowsSettings'>true</dpiAware>
        </windowsSettings>
      </application>
    </assembly>")

    file(WRITE "${CMAKE_BINARY_DIR}/app.manifest" "${MANIFEST_CONTENT}")
    file(WRITE "${CMAKE_BINARY_DIR}/app.rc" "1 24 \"app.manifest\"")

    list(APPEND PLATFORM_SOURCES "${CMAKE_BINARY_DIR}/app.rc")

    list(APPEND PLATFORM_SOURCES
            gui/windows/internal/ComCtl32.h
            gui/windows/internal/ComDlg32.h
            gui/windows/internal/DwmApi.h
            gui/windows/internal/Win32Enum.h
            gui/windows/internal/Gdi32.h
            gui/windows/internal/GdiPlus.h
            gui/windows/internal/Kernel32.h
            gui/windows/internal/Ole32.h
            gui/windows/internal/OleAut32.h
            gui/windows/internal/ShCore.h
            gui/windows/internal/Shell32.h
            gui/windows/internal/Shlwapi.h
            gui/windows/internal/User32.h
            gui/windows/internal/UxTheme.h
            gui/windows/internal/WinMM.h
            gui/windows/App.h
            gui/windows/WindowRegistry.h
            gui/windows/ControlBase.h
            gui/windows/Bitmap.h
            gui/windows/Brush.h
            gui/windows/Button.h
            gui/windows/Canvas.h
            gui/windows/Color.h
            gui/windows/ComboBox.h
            gui/windows/CommonDlgs.h
            gui/windows/Event.h
            gui/windows/EventData.h
            gui/windows/EventManager.h
            gui/windows/Controller.h
            gui/windows/Font.h
            gui/windows/Form.h
            gui/windows/GlobalVars.h
            gui/windows/Icon.h
            gui/windows/ImageView.h
            gui/windows/Input.h
            gui/windows/Keyboard.h
            gui/windows/Label.h
            gui/windows/ListView.h
            gui/windows/Menu.h
            gui/windows/MouseControl.h
            gui/windows/Panel.h
            gui/windows/Path.h
            gui/windows/Pen.h
            gui/windows/ProgressBar.h
            gui/windows/Slider.h
            gui/windows/Rect.h
            gui/windows/Toolbar.h
            gui/windows/Utils.h
            gui/windows/Voice.h
            gui/windows/ListInterfaces.h
            gui/windows/App.cpp
            gui/windows/GlobalVars.cpp
            gui/windows/Bitmap.cpp
            gui/windows/Brush.cpp
            gui/windows/Button.cpp
            gui/windows/Canvas.cpp
            gui/windows/ComboBox.cpp
            gui/windows/CommonDlgs.cpp
            gui/windows/ControlBase.cpp
            gui/windows/Font.cpp
            gui/windows/Form.cpp
            gui/windows/Icon.cpp
            gui/windows/ImageView.cpp
            gui/windows/Input.cpp
            gui/windows/Keyboard.cpp
            gui/windows/Label.cpp
            gui/windows/ListView.cpp
            gui/windows/Menu.cpp
            gui/windows/MouseControl.cpp
            gui/windows/Panel.cpp
            gui/windows/Path.cpp
            gui/windows/Pen.cpp
            gui/windows/ProgressBar.cpp
            gui/windows/Rect.cpp
            gui/windows/Slider.cpp
            gui/windows/Toolbar.cpp
            gui/windows/Utils.cpp
            gui/windows/Voice.cpp
    )
    # Windows 基础库
    list(APPEND PLATFORM_LIBS
            wininet opengl32 comctl32 comdlg32
            gdi32 gdiplus user32 kernel32
            ole32 oleaut32 shell32 shlwapi
            uxtheme winmm)

elseif (APPLE)
    message(STATUS "Configuring for macOS...")
    # list(APPEND PLATFORM_LIBS "-framework Cocoa")
elseif (UNIX)
    message(STATUS "Configuring for Linux...")
    # list(APPEND PLATFORM_LIBS m X11)
endif ()

# ==========================================
# 3. 源文件列表
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
        common/FontKit.h
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
        stdlib/GuiModule.h
        gui/GuiRenderer.h
        gui/GuiRuntime.cpp
        gui/GuiRuntime.h
        common/ColorKit.h
        libs/stb_image.h
        common/TextureKit.h
        error/RuntimeError.h
        stdlib/TimerModule.h
        common/EventKit.h
)

# ==========================================
# 主程序 BxScript
# ==========================================
# 在这里就把 PLATFORM_SOURCES (app.rc) 加进去
add_executable(BxScript ${SOURCE_FILES} ${PLATFORM_SOURCES})

target_include_directories(BxScript PRIVATE ${PROJECT_SOURCE_DIR})

# 链接库 (使用上面根据平台计算好的 PLATFORM_LIBS)
if (PLATFORM_LIBS)
    target_link_libraries(BxScript PRIVATE ${PLATFORM_LIBS})
endif ()

# ==========================================
# 5. GoogleTest 引入
# ==========================================
enable_testing()
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

# ==========================================
# 6. 测试程序 test_parser
# ==========================================
# 从列表移除 main.cpp
list(REMOVE_ITEM SOURCE_FILES main.cpp)

# 测试程序也最好加上资源文件(PLATFORM_SOURCES)，防止测试时涉及到UI崩溃或样式不对
add_executable(test_parser
        tests/test_parser.cpp
        tests/test_interpreter.cpp
        ${SOURCE_FILES}
        ${PLATFORM_SOURCES}
)

target_include_directories(test_parser PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(test_parser PRIVATE gtest_main)

if (PLATFORM_LIBS)
    target_link_libraries(test_parser PRIVATE ${PLATFORM_LIBS})
endif ()

include(GoogleTest)
gtest_discover_tests(test_parser)