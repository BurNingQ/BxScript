/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/4
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    App
 */
#ifndef BXSCRIPT_APP_H
#define BXSCRIPT_APP_H

#include <cstdint>

// ============================================================================
// 1. 声明部分 (Declaration)
// ============================================================================

class App {
private:
    // 使用 void* 和 uintptr_t 代替 Windows 类型
    inline static uintptr_t g_gdiplusToken = 0;

public:
    /**
     * 初始化应用程序环境。
     */
    static void Init();

    /**
     * 获取当前应用程序实例句柄。
     */
    static void *GetInstance();

    /**
     * 运行主消息循环 (阻塞)。
     */
    static int Run();

    /**
     * 退出应用程序。
     */
    static void Exit(int exitCode = 0);

    /**
     * 处理挂起的消息 (防止耗时操作卡死界面)。
     */
    static void ProcessPendingMessages();

private:
    static void Shutdown();

    /**
     * 消息预处理 (拦截键盘快捷键等)。
     * 参数 msg 使用 void* 隔离
     */
    static bool PreTranslateMessage(void *msg);
};

#endif // BXSCRIPT_APP_H


// ============================================================================
// 2. 实现部分 (Implementation)
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include <gdiplus.h>

// 引入底层封装
#include "internal/Kernel32.h"
#include "internal/User32.h"
#include "internal/ComCtl32.h"
#include "internal/GdiPlus.h"
#include "internal/Ole32.h"
#include "internal/ShCore.h"

// 引入运行时支持
#include "WindowRegistry.h"
#include "ControlBase.h"

// 宏转换辅助
#define HINSTANCE_CAST(ptr) static_cast<HINSTANCE>(ptr)
#define MSG_CAST(ptr) static_cast<MSG*>(ptr)

void App::Init() {
    // 1. 获取实例句柄
    HINSTANCE hInst = Kernel32::W32_GetModuleHandle(nullptr);
    if (!hInst) {
        MessageBoxW(NULL, L"Failed to get application instance.", L"Error", MB_ICONERROR);
        exit(1);
    }
    gAppInstance = Kernel32::W32_GetModuleHandle(nullptr);

    // 2. 初始化 COM
    Ole32::W32_CoInitializeEx(COINIT_APARTMENTTHREADED);

    // 3. High DPI
    if (ShCore::IsAvailable()) {
        ShCore::W32_SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    }

    // 4. 通用控件
    INITCOMMONCONTROLSEX icex = {0};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES |
                 ICC_PROGRESS_CLASS | ICC_TAB_CLASSES | ICC_BAR_CLASSES |
                 ICC_USEREX_CLASSES | ICC_STANDARD_CLASSES;
    ComCtl32::InitCommonControls(&icex);

    // 5. GDI+
    ULONG_PTR token;
    GdiPlus::W32_GdiplusStartup(&token);
    g_gdiplusToken = token;

    if (DefaultFont == nullptr) {
        DefaultFont = new Font(L"MS Shell Dlg 2", 8, 0);
    }
}

void *App::GetInstance() {
    return gAppInstance;
}

void App::Shutdown() {
    if (g_gdiplusToken != 0) {
        GdiPlus::W32_GdiplusShutdown((ULONG_PTR) g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    Ole32::W32_CoUninitialize();
}

void App::Exit(int exitCode) {
    User32::W32_PostQuitMessage(exitCode);
}

int App::Run() {
    MSG msg = {0};

    while (User32::W32_GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (!PreTranslateMessage(&msg)) {
            User32::W32_TranslateMessage(&msg);
            User32::W32_DispatchMessage(&msg);
        }
    }

    if (DefaultFont) { delete DefaultFont; DefaultFont = nullptr; }

    Shutdown();
    return (int) msg.wParam;
}

void App::ProcessPendingMessages() {
    MSG msg = {0};
    while (User32::W32_PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (!PreTranslateMessage(&msg)) {
            User32::W32_TranslateMessage(&msg);
            User32::W32_DispatchMessage(&msg);
        }
    }
}

bool App::PreTranslateMessage(void *msgVoid) {
    MSG *msg = MSG_CAST(msgVoid);
    if (!msg->hwnd) return false;

    // 1. 通过 HWND 查找绑定的 C++ 对象
    ControlBase *ctrl = WindowRegistry::Get(msg->hwnd);
    if (!ctrl) return false;

    // 2. 处理内部跨线程调用 (Invoke)
    // 对应 ControlBase 中定义的 WM_BX_INVOKE (WM_USER + 1001)
    if (msg->message == (WM_USER + 1001)) {
        ctrl->ProcessInvokeQueue();
        return true; // 消息已处理，不再下发
    }

    bool processed = false;

    // 3. 过滤键盘和鼠标消息范围
    if ((msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST) || (msg->message >= WM_MOUSEFIRST && msg->message <= WM_MOUSELAST)) {
        // 4. 特殊处理 WM_KEYDOWN (同步 Go 代码逻辑)
        if (msg->message == WM_KEYDOWN && ctrl->OnKeyDown) {
            KeyEventData keyData;
            keyData.VKey = (int)msg->wParam;
            keyData.ScanCode = (int)((msg->lParam >> 16) & 0xFF);
            ctrl->OnKeyDown.Fire(Event(ctrl, keyData));
        }

        // 5. 消息冒泡预处理 (Parent Chain)
        // 这一步非常关键，用于处理快捷键、Tab 键切换焦点等。
        // 如果子控件不处理，就问父窗口，直到最顶层。
        for (ControlBase *p = ctrl; p != nullptr; p = p->GetParent()) {
            if (p->PreTranslateMessage(msg)) {
                processed = true;
                break;
            }
        }
    }

    return processed;
}

#endif // BXSCRIPT_IMPLEMENTATION
