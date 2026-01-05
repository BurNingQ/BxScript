/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/5
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Utils
 */
#ifndef BXSCRIPT_UTILS_H
#define BXSCRIPT_UTILS_H

#include <string>
#include <vector>
#include <cstdint>
#include "Controller.h"
#include "Rect.h"

void internalTrackMouseEvent(void *hwnd);

void SetStyle(void *hwnd, bool b, int style);

void SetExStyle(void *hwnd, bool b, int style);

void *CreateWindow(const std::wstring &className, Controller *parent, unsigned int exStyle, unsigned int style);

void RegisterClass(const std::wstring &className, uintptr_t wndproc);

uint32_t RegisterWindowMessage(const std::wstring &name);

// Internal info helpers
void *getMonitorInfo(void *hwnd); // Returns MONITORINFO* as void*
void *getWindowInfo(void *hwnd); // Returns WINDOWINFO* as void*

void RegClassOnlyOnce(const std::wstring &className);

Rect *ScreenToClientRect(void *hwnd, void *rect); // rect is w32.RECT*

int ScaleWithDPI(int pixels, unsigned int dpi);

int ScaleToDefaultDPI(int pixels, unsigned int dpi);

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

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
    tme.hwndTrack = (HWND) hwnd;
    tme.dwHoverTime = HOVER_DEFAULT;

    Comctl32::InitTrackMouseEvent(&tme);
}

void SetStyle(void *hwnd, bool b, int style) {
    LONG_PTR originalStyle = User32::W32_GetWindowLongPtr((HWND) hwnd, GWL_STYLE);
    if (originalStyle != 0) {
        if (b) {
            originalStyle |= style;
        } else {
            originalStyle &= ~style;
        }
        User32::W32_SetWindowLongPtr((HWND) hwnd, GWL_STYLE, originalStyle);
    }
}

void SetExStyle(void *hwnd, bool b, int style) {
    LONG_PTR originalStyle = User32::W32_GetWindowLongPtr((HWND) hwnd, GWL_EXSTYLE);
    if (originalStyle != 0) {
        if (b) {
            originalStyle |= style;
        } else {
            originalStyle &= ~style;
        }
        User32::W32_SetWindowLongPtr((HWND) hwnd, GWL_EXSTYLE, originalStyle);
    }
}

void *CreateWindow(const std::wstring &className, Controller *parent, unsigned int exStyle, unsigned int style) {
    void *instance = gAppInstance;
    HWND parentHwnd = nullptr;
    if (parent != nullptr) {
        parentHwnd = (HWND) parent->Handle();
    }

    HWND hwnd = User32::W32_CreateWindowEx(
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
        (HINSTANCE) instance,
        nullptr);

    if (hwnd == 0) {
        exit(1); // Equivalent to panic
    }

    return (void *) hwnd;
}

void RegisterClass(const std::wstring &className, uintptr_t wndproc) {
    HINSTANCE instance = (HINSTANCE) gAppInstance;
    HICON icon = LoadIconW(instance, MAKEINTRESOURCEW(3));

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) wndproc;
    wc.hInstance = instance;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hIcon = icon;
    wc.hCursor = LoadCursorW(nullptr, (LPCWSTR) IDC_ARROW);
    wc.lpszClassName = className.c_str();
    wc.hIconSm = icon;

    if (RegisterClassExW(&wc) == 0) {
        exit(1); // Equivalent to panic
    }
}

uint32_t RegisterWindowMessage(const std::wstring &name) {
    UINT ret = ::RegisterWindowMessageW(name.c_str());
    if (ret == 0) {
        exit(1); // Equivalent to panic
    }
    return (uint32_t) ret;
}

void *getMonitorInfo(void *hwnd) {
    HMONITOR currentMonitor = MonitorFromWindow((HWND) hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO *info = new MONITORINFO();
    info->cbSize = sizeof(MONITORINFO);
    GetMonitorInfoW(currentMonitor, info);
    return (void *) info;
}

void *getWindowInfo(void *hwnd) {
    WINDOWINFO *info = new WINDOWINFO();
    info->cbSize = sizeof(WINDOWINFO);
    GetWindowInfo((HWND) hwnd, info);
    return (void *) info;
}

void RegClassOnlyOnce(const std::wstring &className) {
    bool isExists = false;
    for (const auto &cls: gRegisteredClasses) {
        if (cls == className) {
            isExists = true;
            break;
        }
    }

    if (!isExists) {
        RegisterClass(className, (uintptr_t) GeneralWndProcCallBack);
        gRegisteredClasses.push_back(className);
    }
}

Rect *ScreenToClientRect(void *hwnd, void *rectPtr) {
    RECT *rect = (RECT *) rectPtr;
    POINT p1 = {rect->left, rect->top};
    POINT p2 = {rect->right, rect->bottom};

    ::ScreenToClient((HWND) hwnd, &p1);
    ::ScreenToClient((HWND) hwnd, &p2);

    return Rect::NewRect(p1.x, p1.y, p2.x, p2.y);
}

int ScaleWithDPI(int pixels, unsigned int dpi) {
    return (pixels * (int) dpi) / 96;
}

int ScaleToDefaultDPI(int pixels, unsigned int dpi) {
    return (pixels * 96) / (int) dpi;
}

#endif
