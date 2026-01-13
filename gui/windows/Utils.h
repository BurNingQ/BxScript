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
#include "Controller.h"

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

#endif
