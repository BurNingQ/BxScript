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
 * @brief    GlobalVars
 */
#pragma once

#ifndef BXSCRIPT_GLOBAL_VARS_H
#define BXSCRIPT_GLOBAL_VARS_H

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

inline void *G_AppInstance = nullptr;
inline std::unordered_map<void *, Controller *> G_ControllerRegistry;
inline std::vector<std::wstring> G_RegisteredClasses;
inline uint32_t WM_InvokeCallback = 0;
typedef intptr_t (__stdcall *W32_WNDPROC)(void *, unsigned int, uintptr_t, uintptr_t);

inline W32_WNDPROC GeneralWndProcCallBack = nullptr;
inline Font *DefaultFont = nullptr;

intptr_t __stdcall GeneralWndProc(void *hwnd, unsigned int msg, uintptr_t wparam, uintptr_t lparam);

#endif
