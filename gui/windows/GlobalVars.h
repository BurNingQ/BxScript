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


// --- Private global variables ---
inline void *gAppInstance = nullptr;
inline std::unordered_map<void *, Controller *> gControllerRegistry;
inline std::vector<std::wstring> gRegisteredClasses;
inline uint32_t wmInvokeCallback = 0;

// --- Public global variables ---
typedef intptr_t (__stdcall *W32_WNDPROC)(void *, unsigned int, uintptr_t, uintptr_t);

inline W32_WNDPROC GeneralWndProcCallBack = nullptr;
inline Font *DefaultFont = nullptr;

// 函数声明
intptr_t __stdcall generalWndProc(void *hwnd, unsigned int msg, uintptr_t wparam, uintptr_t lparam);

#endif // BXSCRIPT_GLOBAL_VARS_H

// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#ifndef BXSCRIPT_GLOBAL_VARS_IMPL
#define BXSCRIPT_GLOBAL_VARS_IMPL

#include <windows.h>
#include "Controller.h"
#include "ControlBase.h"
#include "EventData.h"
#include "Canvas.h"
#include "Menu.h"
#include "internal/User32.h"
#include "internal/Shell32.h"

namespace {

    void genPoint(uintptr_t p, int &x, int &y) {
        x = static_cast<int>(static_cast<short>(LOWORD(static_cast<uint32_t>(p))));
        y = static_cast<int>(static_cast<short>(HIWORD(static_cast<uint32_t>(p))));
    }

    MouseEventData *genMouseEventArg(uintptr_t wparam, uintptr_t lparam) {
        auto data = new MouseEventData();
        data->Button = static_cast<int>(wparam);
        genPoint(lparam, data->X, data->Y);
        return data;
    }

    DropFilesEventData *genDropFilesEventArg(uintptr_t wparam) {
        auto hDrop = reinterpret_cast<HDROP>(wparam);
        auto data = new DropFilesEventData();

        unsigned int fileCount = Shell32::W32_DragQueryFileCount(hDrop);
        for (unsigned int i = 0; i < fileCount; i++) {
            wchar_t buf[MAX_PATH];
            Shell32::W32_DragQueryFilePath(hDrop, i, buf, MAX_PATH);
            data->Files.push_back(buf);
        }

        POINT pt;
        Shell32::W32_DragQueryPoint(hDrop, &pt);
        data->X = pt.x;
        data->Y = pt.y;

        Shell32::W32_DragFinish(hDrop);
        return data;
    }
}

inline intptr_t __stdcall generalWndProc(void *hwnd, unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    // 基础消息预处理
    switch (msg) {
        case WM_HSCROLL: break;
        case WM_VSCROLL: break;
    }

    auto it = gControllerRegistry.find(hwnd);
    if (it != gControllerRegistry.end()) {
        Controller *controller = it->second;
        // 执行对象自身的 WndProc
        uintptr_t ret = controller->WndProc(msg, wparam, lparam);
        switch (msg) {
            case WM_NOTIFY: {
                const auto nm = (NMHDR *) lparam;
                auto itChild = gControllerRegistry.find(nm->hwndFrom);
                if (itChild != gControllerRegistry.end()) {
                    uintptr_t childRet = itChild->second->WndProc(msg, wparam, lparam);
                    if (childRet != 0) {
                        SetWindowLongPtrW((HWND) hwnd, DWLP_MSGRESULT, (LONG_PTR) childRet);
                        return TRUE;
                    }
                }
                break;
            }
            case WM_COMMAND: {
                if (lparam != 0) {
                    auto itChild = gControllerRegistry.find((void *) lparam);
                    if (itChild != gControllerRegistry.end()) {
                        uintptr_t childRet = itChild->second->WndProc(msg, wparam, lparam);
                        if (childRet != 0) {
                            SetWindowLongPtrW((HWND) hwnd, DWLP_MSGRESULT, (LONG_PTR) childRet);
                            return TRUE;
                        }
                    }
                }
                break;
            }
            case WM_CLOSE:
                controller->OnClose().Fire(Event(static_cast<ControlBase *>(controller), nullptr));
                break;
            case WM_KILLFOCUS:
                controller->OnKillFocus().Fire(Event(static_cast<ControlBase *>(controller), nullptr));
                break;
            case WM_SETFOCUS:
                controller->OnSetFocus().Fire(Event(static_cast<ControlBase *>(controller), nullptr));
                break;
            case WM_DROPFILES:
                controller->OnDropFiles().Fire(Event(static_cast<ControlBase *>(controller), genDropFilesEventArg(wparam)));
                break;
            case WM_CONTEXTMENU: {
                if (wparam != 0) {
                    auto itCtx = gControllerRegistry.find((void *) wparam);
                    if (itCtx != gControllerRegistry.end()) {
                        MenuItem *contextMenu = itCtx->second->ContextMenu();
                        int x, y;
                        genPoint(lparam, x, y);
                        if (contextMenu != nullptr) {
                            uint32_t id = static_cast<uint32_t>(User32::W32_TrackPopupMenuEx(contextMenu->hSubMenu, TPM_NOANIMATION | TPM_RETURNCMD, x, y,
                                                                                             static_cast<HWND>(itCtx->second->Handle()), nullptr));
                            MenuItem *item = actionsByID[id];
                            if (item != nullptr) {
                                item->OnClick().Fire(Event(static_cast<ControlBase *>(itCtx->second), genMouseEventArg(wparam, lparam)));
                            }
                            return 0;
                        }
                    }
                }
                break;
            }
            case WM_LBUTTONDOWN:
                controller->OnLBDown().Fire(Event(static_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_LBUTTONUP:
                controller->OnLBUp().Fire(Event(static_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_LBUTTONDBLCLK:
                controller->OnLBDbl().Fire(Event(static_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_MBUTTONDOWN:
                controller->OnMBDown().Fire(Event(static_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_MBUTTONUP:
                controller->OnMBUp().Fire(Event(static_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_RBUTTONDOWN:
                controller->OnRBDown().Fire(Event(static_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_RBUTTONUP:
                controller->OnRBUp().Fire(Event(static_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_RBUTTONDBLCLK:
                controller->OnRBDbl().Fire(Event(static_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_MOUSEMOVE:
                controller->OnMouseMove().Fire(Event(static_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_PAINT: {
                Canvas *canvas = Canvas::FromHwnd(hwnd);
                PaintEventData data;
                data.pCanvas = canvas;
                controller->OnPaint().Fire(Event(static_cast<ControlBase *>(controller), &data));
                delete canvas; // 对照 defer canvas.Dispose()
                break;
            }
            case WM_KEYUP: {
                KeyUpEventData data;
                data.VKey = static_cast<int>(wparam);
                data.Code = static_cast<int>(lparam);
                controller->OnKeyUp().Fire(Event(static_cast<ControlBase *>(controller), &data));
                break;
            }
            case WM_SIZE: {
                int x, y;
                genPoint(lparam, x, y);
                SizeEventData data;
                data.Type = static_cast<unsigned int>(wparam);
                data.Width = x;
                data.Height = y;
                controller->OnSize().Fire(Event(static_cast<ControlBase *>(controller), &data));
                break;
            }
            default:
                if (wmInvokeCallback != 0 && msg == wmInvokeCallback) {
                    controller->invokeCallbacks();
                }
                break;
        }
        return static_cast<intptr_t>(ret);
    }

    return DefWindowProcW(static_cast<HWND>(hwnd), msg, wparam, static_cast<LPARAM>(lparam));
}

struct WndProcInitializer {
    WndProcInitializer() {
        GeneralWndProcCallBack = static_cast<W32_WNDPROC>(generalWndProc);
        wmInvokeCallback = ::RegisterWindowMessageW(L"WincV0.InvokeCallback");
    }
};
static WndProcInitializer _wndproc_init;

#endif // BXSCRIPT_GLOBAL_VARS_IMPL

#endif
