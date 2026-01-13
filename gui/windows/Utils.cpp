/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/13
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Utils
 */
#include "Utils.h"
#include <windows.h>
#include "GlobalVars.h"
#include "Controller.h"
#include "Rect.h"
#include "internal/User32.h"
#include "internal/ComCtl32.h"

void internalTrackMouseEvent(void *hwnd) {
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = static_cast<HWND>(hwnd);
    tme.dwHoverTime = HOVER_DEFAULT;

    ComCtl32::InitTrackMouseEvent(&tme);
}

void SetStyle(void *hwnd, bool b, int style) {
    LONG_PTR originalStyle = User32::W32_GetWindowLongPtr(static_cast<HWND>(hwnd), GWL_STYLE);
    if (originalStyle != 0) {
        if (b) {
            originalStyle |= style;
        } else {
            originalStyle &= ~style;
        }
        User32::W32_SetWindowLongPtr(static_cast<HWND>(hwnd), GWL_STYLE, originalStyle);
    }
}

void SetExStyle(void *hwnd, bool b, int style) {
    LONG_PTR originalStyle = User32::W32_GetWindowLongPtr(static_cast<HWND>(hwnd), GWL_EXSTYLE);
    if (originalStyle != 0) {
        if (b) {
            originalStyle |= style;
        } else {
            originalStyle &= ~style;
        }
        User32::W32_SetWindowLongPtr(static_cast<HWND>(hwnd), GWL_EXSTYLE, originalStyle);
    }
}

void *CreateWindowX(const std::wstring &className, Controller *parent, unsigned int exStyle, unsigned int style) {
    void *instance = G_AppInstance;
    HWND parentHwnd = nullptr;
    if (parent != nullptr) {
        parentHwnd = static_cast<HWND>(parent->Handle());
    }

    const HWND hwnd = User32::W32_CreateWindowEx(
        exStyle,
        className.c_str(),
        nullptr,
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        parentHwnd,
        0,
        static_cast<HINSTANCE>(instance),
        nullptr);

    if (hwnd == 0) {
        exit(1); // Equivalent to panic
    }

    return (void *) hwnd;
}

void RegisterClassX(const std::wstring &className, uintptr_t wndproc) {
    const auto instance = static_cast<HINSTANCE>(G_AppInstance);
    const HICON icon = LoadIconW(instance, MAKEINTRESOURCEW(3));

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = reinterpret_cast<WNDPROC>(wndproc);
    wc.hInstance = instance;
    wc.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_BTNFACE + 1));
    wc.hIcon = icon;
    wc.hCursor = LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW));
    wc.lpszClassName = className.c_str();
    wc.hIconSm = icon;

    if (RegisterClassExW(&wc) == 0) {
        exit(1); // Equivalent to panic
    }
}

uint32_t RegisterWindowMessageX(const std::wstring &name) {
    const UINT ret = ::RegisterWindowMessageW(name.c_str());
    if (ret == 0) {
        exit(1); // Equivalent to panic
    }
    return ret;
}

void *getMonitorInfo(void *hwnd) {
    HMONITOR currentMonitor = MonitorFromWindow(static_cast<HWND>(hwnd), MONITOR_DEFAULTTONEAREST);
    auto *info = new MONITORINFO();
    info->cbSize = sizeof(MONITORINFO);
    GetMonitorInfoW(currentMonitor, info);
    return (void *) info;
}

void *getWindowInfo(void *hwnd) {
    auto *info = new WINDOWINFO();
    info->cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(static_cast<HWND>(hwnd), info);
    return (void *) info;
}

void RegClassOnlyOnce(const std::wstring &className) {
    bool isExists = false;
    for (const auto &cls: G_RegisteredClasses) {
        if (cls == className) {
            isExists = true;
            break;
        }
    }

    if (!isExists) {
        RegisterClassX(className, reinterpret_cast<uintptr_t>(GeneralWndProcCallBack));
        G_RegisteredClasses.push_back(className);
    }
}

Rect *ScreenToClientRect(void *hwnd, void *rectPtr) {
    const auto rect = static_cast<RECT *>(rectPtr);
    POINT p1 = {rect->left, rect->top};
    POINT p2 = {rect->right, rect->bottom};

    ::ScreenToClient(static_cast<HWND>(hwnd), &p1);
    ::ScreenToClient(static_cast<HWND>(hwnd), &p2);

    return Rect::NewRect(p1.x, p1.y, p2.x, p2.y);
}

int ScaleWithDPI(int pixels, unsigned int dpi) {
    return (pixels * static_cast<int>(dpi)) / 96;
}

int ScaleToDefaultDPI(int pixels, unsigned int dpi) {
    return (pixels * 96) / static_cast<int>(dpi);
}