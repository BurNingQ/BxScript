/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/12
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    App
 */

#include <windows.h>

// 引入底层封装
#include "App.h"

#include "internal/Kernel32.h"
#include "internal/User32.h"
#include "internal/ComCtl32.h"
#include "internal/GdiPlus.h"
#include "internal/Ole32.h"
#include "internal/ShCore.h"
#include "Font.h"
#include "ControlBase.h"
#include "WindowRegistry.h"
#include "EventData.h"

// 宏转换辅助
#define HINSTANCE_CAST(ptr) static_cast<HINSTANCE>(ptr)
#define MSG_CAST(ptr) static_cast<MSG*>(ptr)

inline void App::Init() {
    HINSTANCE hInst = Kernel32::W32_GetModuleHandle(nullptr);
    if (!hInst) {
        MessageBoxW(nullptr, L"Failed to get application instance.", L"Error", MB_ICONERROR);
        exit(1);
    }
    gAppInstance = Kernel32::W32_GetModuleHandle(nullptr);

    Ole32::W32_CoInitializeEx(COINIT_APARTMENTTHREADED);

    if (ShCore::IsAvailable()) {
        ShCore::W32_SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    }

    INITCOMMONCONTROLSEX icex = {0};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES |
                 ICC_PROGRESS_CLASS | ICC_TAB_CLASSES | ICC_BAR_CLASSES |
                 ICC_USEREX_CLASSES | ICC_STANDARD_CLASSES;
    ComCtl32::InitCommonControls(&icex);

    ULONG_PTR token;
    GdiPlus::W32_GdiplusStartup(&token);
    g_gdiplusToken = token;

    if (DefaultFont == nullptr) {
        DefaultFont = new Font(L"MS Shell Dlg 2", 8, static_cast<uint8_t>(0));
    }
}

inline void *App::GetInstance() {
    return gAppInstance;
}

inline void App::Shutdown() {
    if (g_gdiplusToken != 0) {
        GdiPlus::W32_GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    Ole32::W32_CoUninitialize();
}

inline void App::Exit(int exitCode) {
    User32::W32_PostQuitMessage(exitCode);
}

inline bool App::PollEvents() {
    MSG msg = {0};
    while (User32::W32_PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        if (!PreTranslateMessage(&msg)) {
            User32::W32_TranslateMessage(&msg);
            User32::W32_DispatchMessage(&msg);
        }
    }
    return true;
}

inline void App::WaitEvents(int timeoutMs) {
    MsgWaitForMultipleObjectsEx(0, nullptr, timeoutMs, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
}

inline int App::Run() {
    MSG msg = {0};
    if (DefaultFont) {
        delete DefaultFont;
        DefaultFont = nullptr;
    }
    Shutdown();
    return static_cast<int>(msg.wParam);
}

inline bool App::PreTranslateMessage(void *msgVoid) {
    const auto msg = MSG_CAST(msgVoid);
    if (!msg->hwnd) return false;

    ControlBase *ctrl = WindowRegistry::Get(msg->hwnd);
    if (!ctrl) return false;

    if (msg->message == (WM_USER + 1001)) {
        ctrl->invokeCallbacks();
        return true;
    }

    bool processed = false;

    if ((msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST) || (msg->message >= WM_MOUSEFIRST && msg->message <= WM_MOUSELAST)) {
        if (msg->message == WM_KEYDOWN && ctrl->OnKeyDown) {
            KeyEventData keyData{};
            keyData.VKey = static_cast<int>(msg->wParam);
            keyData.ScanCode = static_cast<int>((msg->lParam >> 16) & 0xFF);
            ctrl->OnKeyDown.Fire(Event(ctrl, keyData));
        }

        for (ControlBase *p = ctrl; p != nullptr; p = p->GetParent()) {
            if (p->PreTranslateMessage(msg)) {
                processed = true;
                break;
            }
        }
    }

    return processed;
}