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
#include <cstdint>
#include <memory>
#include <thread>

#include "Controller.h"
#include "evaluator/Value.h"
#include "internal/User32.h"

void internalTrackMouseEvent(void *hwnd);

void SetStyle(void *hwnd, bool b, int style);

void SetExStyle(void *hwnd, bool b, int style);

void *CreateWindowX(const std::wstring &className, Controller *parent, unsigned int exStyle, unsigned int style);

void RegisterClassX(const std::wstring &className, uintptr_t wndproc);

uint32_t RegisterWindowMessageX(const std::wstring &name);

// Internal info helpers
void *getMonitorInfo(void *hwnd); // Returns MONITORINFO* as void*
void *getWindowInfo(void *hwnd); // Returns WINDOWINFO* as void*

void RegClassOnlyOnce(const std::wstring &className);

Rect *ScreenToClientRect(void *hwnd, void *rect); // rect is w32.RECT*

int ScaleWithDPI(int pixels, unsigned int dpi);

int ScaleToDefaultDPI(int pixels, unsigned int dpi);

void doMax(void *hwnd);

void doMin(void *hwnd);

void doCap(void *hwnd);

void doExit(void *hwnd);

void doTray(void *hwnd, const std::wstring &iconPath, const std::wstring &tooltip, void *&trayIconHandle, bool &hasTray);

void removeTray(void *hwnd, void *&trayIconHandle, bool &hasTray);

struct Mouse {
    static void move(int x, int y) {
        User32::W32_SendMouseEvent(x, y, MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE);
    }

    static void click(int x, int y) {
        move(x, y);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        down(x, y, "left");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        up(x, y, "left");
    }

    static void up(int x, int y, std::string tag) {
        DWORD flag = MOUSEEVENTF_LEFTUP;
        if (tag == "right") flag = MOUSEEVENTF_RIGHTUP;
        else if (tag == "middle") flag = MOUSEEVENTF_MIDDLEUP;
        User32::W32_SendMouseEvent(x, y, flag);
    }

    static void down(int x, int y, const std::string &tag) {
        DWORD flag = MOUSEEVENTF_RIGHTDOWN;
        if (tag == "right") flag = MOUSEEVENTF_RIGHTDOWN;
        else if (tag == "middle") flag = MOUSEEVENTF_MIDDLEDOWN;
        User32::W32_SendMouseEvent(x, y, flag);
    }

    static void scroll(int delta = -120) {
        User32::W32_SendMouseEvent(0, 0,MOUSEEVENTF_WHEEL, delta);
    }

    static std::pair<int, int> pos() {
        auto [x, y] = User32::W32_GetMouseCursorPos();
        return std::make_pair(x, y);
    }
};

void showConsole();

void hideConsole();

void setConsoleTitle(const std::wstring &title);

#endif
