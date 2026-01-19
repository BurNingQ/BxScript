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
 * @brief    GlobalVars
 */

#include <windows.h>
#include "Controller.h"
#include "ControlBase.h"
#include "EventData.h"
#include "Canvas.h"
#include "Menu.h"
#include "internal/User32.h"
#include "internal/Shell32.h"
#include "GlobalVars.h"

void genPoint(uintptr_t p, int &x, int &y) {
    x = static_cast<int>(static_cast<short>(LOWORD(static_cast<uint32_t>(p))));
    y = static_cast<int>(static_cast<short>(HIWORD(static_cast<uint32_t>(p))));
}

MouseEventData *genMouseEventArg(uintptr_t wparam, uintptr_t lparam) {
    const auto data = new MouseEventData();
    data->Button = static_cast<int>(wparam);
    genPoint(lparam, data->X, data->Y);
    return data;
}

DropFilesEventData *genDropFilesEventArg(uintptr_t wparam) {
    const auto hDrop = reinterpret_cast<HDROP>(wparam);
    const auto data = new DropFilesEventData();

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

intptr_t __stdcall generalWndProc(void *hwnd, unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    // 基础消息预处理
    switch (msg) {
        case WM_HSCROLL: break;
        case WM_VSCROLL: break;
    }

    auto it = G_ControllerRegistry.find(hwnd);
    if (it != G_ControllerRegistry.end()) {
        Controller *controller = it->second;
        // 执行对象自身的 WndProc
        const uintptr_t ret = controller->WndProc(msg, wparam, lparam);
        switch (msg) {
            case WM_NOTIFY: {
                const auto nm = reinterpret_cast<NMHDR *>(lparam);
                auto itChild = G_ControllerRegistry.find(nm->hwndFrom);
                if (itChild != G_ControllerRegistry.end()) {
                    uintptr_t childRet = itChild->second->WndProc(msg, wparam, lparam);
                    if (childRet != 0) {
                        SetWindowLongPtrW(static_cast<HWND>(hwnd), DWLP_MSGRESULT, static_cast<LONG_PTR>(childRet));
                        return TRUE;
                    }
                }
                break;
            }
            case WM_COMMAND: {
                if (lparam != 0) {
                    auto itChild = G_ControllerRegistry.find(reinterpret_cast<void *>(lparam));
                    if (itChild != G_ControllerRegistry.end()) {
                        uintptr_t childRet = itChild->second->WndProc(msg, wparam, lparam);
                        if (childRet != 0) {
                            SetWindowLongPtrW(static_cast<HWND>(hwnd), DWLP_MSGRESULT, static_cast<LONG_PTR>(childRet));
                            return TRUE;
                        }
                    }
                }
                break;
            }
            case WM_CLOSE:
                controller->OnClose().Fire(Event(dynamic_cast<ControlBase *>(controller), nullptr));
                break;
            case WM_KILLFOCUS:
                controller->OnKillFocus().Fire(Event(dynamic_cast<ControlBase *>(controller), nullptr));
                break;
            case WM_SETFOCUS:
                controller->OnSetFocus().Fire(Event(dynamic_cast<ControlBase *>(controller), nullptr));
                break;
            case WM_DROPFILES:
                controller->OnDropFiles().Fire(Event(dynamic_cast<ControlBase *>(controller), genDropFilesEventArg(wparam)));
                break;
            case WM_CONTEXTMENU: {
                if (wparam != 0) {
                    auto itCtx = G_ControllerRegistry.find(reinterpret_cast<void *>(wparam));
                    if (itCtx != G_ControllerRegistry.end()) {
                        MenuItem *contextMenu = itCtx->second->ContextMenu();
                        int x, y;
                        genPoint(lparam, x, y);
                        if (contextMenu != nullptr) {
                            const auto id = static_cast<uint32_t>(User32::W32_TrackPopupMenuEx(
                                contextMenu->hSubMenu, TPM_NOANIMATION | TPM_RETURNCMD, x, y,
                                static_cast<HWND>(itCtx->second->Handle()), nullptr));
                            MenuItem *item = actionsByID[id];
                            if (item != nullptr) {
                                item->OnClick().Fire(Event(dynamic_cast<ControlBase *>(itCtx->second), genMouseEventArg(wparam, lparam)));
                            }
                            return 0;
                        }
                    }
                }
                break;
            }
            case WM_LBUTTONDOWN:
                controller->OnLBDown().Fire(Event(dynamic_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_LBUTTONUP:
                controller->OnLBUp().Fire(Event(dynamic_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_LBUTTONDBLCLK:
                controller->OnLBDbl().Fire(Event(dynamic_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_MBUTTONDOWN:
                controller->OnMBDown().Fire(Event(dynamic_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_MBUTTONUP:
                controller->OnMBUp().Fire(Event(dynamic_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_RBUTTONDOWN:
                controller->OnRBDown().Fire(Event(dynamic_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_RBUTTONUP:
                controller->OnRBUp().Fire(Event(dynamic_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_RBUTTONDBLCLK:
                controller->OnRBDbl().Fire(Event(dynamic_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_MOUSEMOVE:
                controller->OnMouseMove().Fire(Event(dynamic_cast<ControlBase *>(controller), genMouseEventArg(wparam, lparam)));
                break;
            case WM_PAINT: {
                Canvas *canvas = Canvas::FromHwnd(hwnd);
                PaintEventData data{};
                data.pCanvas = canvas;
                controller->OnPaint().Fire(Event(dynamic_cast<ControlBase *>(controller), &data));
                delete canvas; // 对照 defer canvas.Dispose()
                break;
            }
            case WM_KEYUP: {
                KeyUpEventData data{};
                data.VKey = static_cast<int>(wparam);
                data.Code = static_cast<int>(lparam);
                controller->OnKeyUp().Fire(Event(dynamic_cast<ControlBase *>(controller), &data));
                break;
            }
            case WM_MOUSEWHEEL: {
                const short delta = GET_WHEEL_DELTA_WPARAM(wparam);
                MouseEventData *data = genMouseEventArg(wparam, lparam);
                data->Wheel = delta;
                controller->OnMouseWheel().Fire(Event(dynamic_cast<ControlBase *>(controller), data));
                break;
            }
            case WM_SIZE: {
                int x, y;
                genPoint(lparam, x, y);
                SizeEventData data{};
                data.Type = static_cast<unsigned int>(wparam);
                data.Width = x;
                data.Height = y;
                controller->OnSize().Fire(Event(dynamic_cast<ControlBase *>(controller), &data));
                break;
            }
            default:
                if (WM_InvokeCallback != 0 && msg == WM_InvokeCallback) {
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
        WM_InvokeCallback = ::RegisterWindowMessageW(L"WincV0.InvokeCallback");
    }
};

static WndProcInitializer _wndproc_init;
