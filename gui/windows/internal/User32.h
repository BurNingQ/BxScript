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
 * @brief    User32
 */
#ifndef BXSCRIPT_USER32_H
#define BXSCRIPT_USER32_H

#include <windows.h>
#include <string>

class User32 {
    // 内部动态加载器：专门处理 Win10+ 的 User32 API，保证 Win7 不崩溃
    struct DynamicLoader {
        HMODULE hModule = nullptr;

        typedef UINT (STDAPICALLTYPE *PFN_GetDpiForWindow)(HWND);

        typedef UINT (STDAPICALLTYPE *PFN_GetDpiForSystem)();

        PFN_GetDpiForWindow pGetDpiForWindow = nullptr;
        PFN_GetDpiForSystem pGetDpiForSystem = nullptr;

        DynamicLoader() {
            hModule = GetModuleHandleW(L"user32.dll");
            if (hModule) {
                pGetDpiForWindow = reinterpret_cast<PFN_GetDpiForWindow>(GetProcAddress(hModule, "GetDpiForWindow"));
                pGetDpiForSystem = reinterpret_cast<PFN_GetDpiForSystem>(GetProcAddress(hModule, "GetDpiForSystem"));
            }
        }
    };

    static DynamicLoader &Get() {
        static DynamicLoader loader;
        return loader;
    }

public:
    // =========================================================================================
    // Group 1: 窗口类与创建 (Window Class & Creation)
    // =========================================================================================

    static ATOM W32_RegisterClassEx(CONST WNDCLASSEXW *unnamedParam1) {
        return RegisterClassExW(unnamedParam1);
    }

    /**
     * 创建窗口 (核心函数)。
     * @note 建议使用 Ex 版本以支持更多样式。
     */
    static HWND W32_CreateWindowEx(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                   int X, int Y, int nWidth, int nHeight,
                                   HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
        return CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }

    static bool W32_DestroyWindow(HWND hWnd) {
        return DestroyWindow(hWnd);
    }

    static HWND W32_FindWindow(LPCWSTR lpClassName, LPCWSTR lpWindowName) {
        return FindWindowW(lpClassName, lpWindowName);
    }


    // =========================================================================================
    // Group 2: 消息循环 (Message Loop)
    // =========================================================================================

    static BOOL W32_GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax) {
        return GetMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    }

    static BOOL W32_PeekMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
        return PeekMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
    }

    static BOOL W32_TranslateMessage(CONST MSG *lpMsg) {
        return TranslateMessage(lpMsg);
    }

    static LRESULT W32_DispatchMessage(CONST MSG *lpMsg) {
        return DispatchMessageW(lpMsg);
    }

    static void W32_PostQuitMessage(int nExitCode) {
        PostQuitMessage(nExitCode);
    }

    static LRESULT W32_SendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
        return SendMessageW(hWnd, Msg, wParam, lParam);
    }

    static BOOL W32_PostMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
        return PostMessageW(hWnd, Msg, wParam, lParam);
    }

    static BOOL W32_WaitMessage() {
        return WaitMessage();
    }


    // =========================================================================================
    // Group 3: 窗口状态与几何 (State & Geometry)
    // =========================================================================================

    static BOOL W32_ShowWindow(HWND hWnd, int nCmdShow) {
        return ShowWindow(hWnd, nCmdShow);
    }

    static BOOL W32_UpdateWindow(HWND hWnd) {
        return UpdateWindow(hWnd);
    }

    static BOOL W32_MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint) {
        return MoveWindow(hWnd, X, Y, nWidth, nHeight, bRepaint);
    }

    static BOOL W32_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
        return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    static BOOL W32_GetWindowRect(HWND hWnd, LPRECT lpRect) {
        return GetWindowRect(hWnd, lpRect);
    }

    static BOOL W32_GetClientRect(HWND hWnd, LPRECT lpRect) {
        return GetClientRect(hWnd, lpRect);
    }

    static BOOL W32_SetWindowText(HWND hWnd, LPCWSTR lpString) {
        return SetWindowTextW(hWnd, lpString);
    }

    static int W32_GetWindowTextLength(HWND hWnd) {
        return GetWindowTextLengthW(hWnd);
    }

    static int W32_GetWindowText(HWND hWnd, LPWSTR lpString, int nMaxCount) {
        return GetWindowTextW(hWnd, lpString, nMaxCount);
    }

    static BOOL W32_EnableWindow(HWND hWnd, BOOL bEnable) {
        return EnableWindow(hWnd, bEnable);
    }

    static BOOL W32_IsWindowEnabled(HWND hWnd) {
        return IsWindowEnabled(hWnd);
    }

    static BOOL W32_IsWindowVisible(HWND hWnd) {
        return IsWindowVisible(hWnd);
    }

    static BOOL W32_ScreenToClient(HWND hWnd, LPPOINT lpPoint) {
        return ScreenToClient(hWnd, lpPoint);
    }

    static BOOL W32_ClientToScreen(HWND hWnd, LPPOINT lpPoint) {
        return ClientToScreen(hWnd, lpPoint);
    }

    // =========================================================================================
    // Group 4: 窗口过程与子类化 (Window Proc & Subclassing)
    // =========================================================================================

    static LRESULT W32_DefWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
        return DefWindowProcW(hWnd, Msg, wParam, lParam);
    }

    static LRESULT W32_CallWindowProc(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
        return CallWindowProcW(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
    }

    /**
     * 设置窗口长整型数据 (x64 兼容)。
     * @usage 用于子类化 (Subclassing) 或绑定用户数据 (GWLP_USERDATA)。
     */
    static LONG_PTR W32_SetWindowLongPtr(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
#ifdef _WIN64
        return SetWindowLongPtrW(hWnd, nIndex, dwNewLong);
#else
        return SetWindowLongW(hWnd, nIndex, dwNewLong);
#endif
    }

    static LONG_PTR W32_GetWindowLongPtr(HWND hWnd, int nIndex) {
#ifdef _WIN64
        return GetWindowLongPtrW(hWnd, nIndex);
#else
        return GetWindowLongW(hWnd, nIndex);
#endif
    }


    // =========================================================================================
    // Group 5: 绘图支持 (Painting)
    // @note 实际绘图用 Gdi32，但获取 DC 和 BeginPaint 在 User32。
    // =========================================================================================

    static HDC W32_GetDC(HWND hWnd) {
        return GetDC(hWnd);
    }

    static int W32_ReleaseDC(HWND hWnd, HDC hDC) {
        return ReleaseDC(hWnd, hDC);
    }

    static HDC W32_BeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint) {
        return BeginPaint(hWnd, lpPaint);
    }

    static BOOL W32_EndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint) {
        return EndPaint(hWnd, lpPaint);
    }

    static BOOL W32_InvalidateRect(HWND hWnd, CONST RECT *lpRect, BOOL bErase) {
        return InvalidateRect(hWnd, lpRect, bErase);
    }

    static int W32_FillRect(HDC hDC, CONST RECT *lprc, HBRUSH hbr) {
        return FillRect(hDC, lprc, hbr);
    }

    static int W32_DrawText(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
        return DrawTextW(hdc, lpchText, cchText, lprc, format);
    }


    // =========================================================================================
    // Group 6: 输入：鼠标与键盘 (Input)
    // =========================================================================================

    static HWND W32_SetFocus(HWND hWnd) {
        return SetFocus(hWnd);
    }

    static HWND W32_GetFocus() {
        return GetFocus();
    }

    static HWND W32_SetCapture(HWND hWnd) {
        return SetCapture(hWnd);
    }

    static BOOL W32_ReleaseCapture() {
        return ReleaseCapture();
    }

    static BOOL W32_GetCursorPos(LPPOINT lpPoint) {
        return GetCursorPos(lpPoint);
    }

    static BOOL W32_SetCursorPos(int X, int Y) {
        return SetCursorPos(X, Y);
    }

    static HCURSOR W32_SetCursor(HCURSOR hCursor) {
        return SetCursor(hCursor);
    }

    static SHORT W32_GetAsyncKeyState(int vKey) {
        return GetAsyncKeyState(vKey);
    }

    static SHORT W32_GetKeyState(int nVirtKey) {
        return GetKeyState(nVirtKey);
    }


    // =========================================================================================
    // Group 7: 剪贴板 (Clipboard)
    // =========================================================================================

    static BOOL W32_OpenClipboard(HWND hWndNewOwner) {
        return OpenClipboard(hWndNewOwner);
    }

    static BOOL W32_CloseClipboard() {
        return CloseClipboard();
    }

    static BOOL W32_EmptyClipboard() {
        return EmptyClipboard();
    }

    static HANDLE W32_GetClipboardData(UINT uFormat) {
        return GetClipboardData(uFormat);
    }

    static HANDLE W32_SetClipboardData(UINT uFormat, HANDLE hMem) {
        return SetClipboardData(uFormat, hMem);
    }

    static BOOL W32_IsClipboardFormatAvailable(UINT format) {
        return IsClipboardFormatAvailable(format);
    }


    // =========================================================================================
    // Group 8: 菜单 (Menus)
    // =========================================================================================

    static HMENU W32_CreateMenu() {
        return CreateMenu();
    }

    static HMENU W32_CreatePopupMenu() {
        return CreatePopupMenu();
    }

    static BOOL W32_DestroyMenu(HMENU hMenu) {
        return DestroyMenu(hMenu);
    }

    static BOOL W32_InsertMenuItem(HMENU hMenu, UINT uItem, BOOL fByPosition, LPCMENUITEMINFOW lpmii) {
        return InsertMenuItemW(hMenu, uItem, fByPosition, lpmii);
    }

    static BOOL W32_TrackPopupMenuEx(HMENU hMenu, UINT uFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm) {
        return TrackPopupMenuEx(hMenu, uFlags, x, y, hwnd, lptpm);
    }


    // =========================================================================================
    // Group 9: 对话框 (Dialogs)
    // =========================================================================================

    static HWND W32_GetDlgItem(HWND hDlg, int nIDDlgItem) {
        return GetDlgItem(hDlg, nIDDlgItem);
    }

    static int W32_MessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
        return MessageBoxW(hWnd, lpText, lpCaption, uType);
    }


    // =========================================================================================
    // Group 10: 监视器与 DPI (Monitors & DPI)
    // @note 包含兼容 Win7 的动态加载处理。
    // =========================================================================================

    static HMONITOR W32_MonitorFromWindow(HWND hwnd, DWORD dwFlags) {
        return MonitorFromWindow(hwnd, dwFlags);
    }

    static BOOL W32_GetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpmi) {
        return GetMonitorInfoW(hMonitor, lpmi);
    }

    /**
     * 获取窗口 DPI (Win10 1607+)。
     * @note 兼容性封装：Win7 下返回 96。
     */
    static UINT W32_GetDpiForWindow(HWND hwnd) {
        auto &api = Get();
        if (api.pGetDpiForWindow) {
            return api.pGetDpiForWindow(hwnd);
        }
        return 96; // 默认 96 DPI
    }

    /**
     * 获取系统 DPI (Win10 1607+)。
     * @note 兼容性封装：Win7 下返回 96。
     */
    static UINT W32_GetDpiForSystem() {
        auto &api = Get();
        if (api.pGetDpiForSystem) {
            return api.pGetDpiForSystem();
        }
        return 96;
    }


    // =========================================================================================
    // Group 11: 矩形工具 (Rect Utilities)
    // @note 虽然 Win32 有这些 API，但通常建议直接操作 RECT 结构体。封装出来给脚本用很方便。
    // =========================================================================================

    static BOOL W32_SetRect(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom) {
        return SetRect(lprc, xLeft, yTop, xRight, yBottom);
    }

    static BOOL W32_CopyRect(LPRECT lprcDst, CONST RECT *lprcSrc) {
        return CopyRect(lprcDst, lprcSrc);
    }

    static BOOL W32_IsRectEmpty(CONST RECT *lprc) {
        return IsRectEmpty(lprc);
    }

    static BOOL W32_PtInRect(CONST RECT *lprc, POINT pt) {
        return PtInRect(lprc, pt);
    }

    static int W32_GetSystemMetrics(int nIndex) {
        return GetSystemMetrics(nIndex);
    }
};

#endif //BXSCRIPT_USER32_H
