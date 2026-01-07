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
 * @brief    Form
 */
#ifndef BXSCRIPT_FORM_H
#define BXSCRIPT_FORM_H

#include "ControlBase.h"
#include <vector>
#include "Utils.h"
#include "Menu.h"
#include "Icon.h"
#include "Utils.h"

class LayoutManager {
public:
    virtual ~LayoutManager() = default;

    virtual void Update() = 0;
};

class Form : public ControlBase {
    LayoutManager *m_layoutMng = nullptr;

    // Fullscreen / Unfullscreen state
    bool m_isFullscreen = false;
    unsigned int m_previousWindowStyle = 0;
    unsigned int m_previousWindowExStyle = 0;
    // 使用 void* 占位 WINDOWPLACEMENT 以保持头文件纯净
    uint8_t m_previousWindowPlacement[44]; // WINDOWPLACEMENT 大小通常为 44 字节

public:
    Form() = default;

    ~Form() override = default;

    // Static Factory Methods
    static Form *NewCustom(Controller *parent, int exStyle, unsigned int dwStyle);

    static Form *New(Controller *parent);

    Form *SetLayout(LayoutManager *mng);

    Form *UpdateLayout();

    Menu *NewMenu();

    void DisableIcon();

    void Maximise();

    void Minimise();

    void Restore();

    void Center();

    void Fullscreen();

    void UnFullscreen();

    bool IsFullScreen() const { return m_isFullscreen; }

    void SetIcon(int iconType, Icon *icon);

    void EnableMaxButton(bool b);

    void EnableMinButton(bool b);

    void EnableSizable(bool b);

    void EnableDragMove(bool b);

    void EnableTopMost(bool b);

    // Override WndProc from ControlBase
    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif // BXSCRIPT_FORM_H

// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/User32.h"
#include "internal/Kernel32.h"
#include "WindowRegistry.h"
#include "App.h"

Form *Form::NewCustom(Controller *parent, int exStyle, unsigned int dwStyle) {
    Form *fm = new Form();
    RegClassOnlyOnce(L"BxForm");

    fm->SetIsForm(true);

    if (exStyle == 0) {
        exStyle = WS_EX_CONTROLPARENT | WS_EX_APPWINDOW;
    }
    if (dwStyle == 0) {
        dwStyle = WS_OVERLAPPEDWINDOW;
    }

    HWND parentHwnd = parent ? (HWND) parent->Handle() : nullptr;
    fm->SetHandle(CreateWindowExW(exStyle, L"BxForm", L"", dwStyle,
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                  parentHwnd, nullptr, (HINSTANCE) App::GetInstance(), nullptr));
    fm->SetParent(parent);

    Icon *ico = Icon::NewIconFromResource(App::GetInstance(), 3);
    if (ico) {
        fm->SetIcon(0, ico);
    }

    User32::W32_SendMessage((HWND) fm->Handle(), WM_CHANGEUISTATE, 1 /* UIS_INITIALIZE */, 0);
    WindowRegistry::Register(fm->Handle(), fm);

    fm->SetFont(Font::DefaultFont);
    fm->SetText(L"Form");
    return fm;
}

Form *Form::New(Controller *parent) {
    Form *fm = new Form();
    RegClassOnlyOnce(L"BxForm");
    fm->SetIsForm(true);

    DWORD exStyle = WS_EX_CONTROLPARENT | WS_EX_APPWINDOW | WS_EX_CLIENTEDGE;
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;

    HWND parentHwnd = parent ? (HWND) parent->Handle() : nullptr;
    fm->SetHandle(CreateWindowExW(exStyle, L"BxForm", L"", dwStyle,
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                  parentHwnd, nullptr, (HINSTANCE) App::GetInstance(), nullptr));
    fm->SetParent(parent);

    Icon *ico = Icon::NewIconFromResource(App::GetInstance(), 1);
    if (ico) {
        fm->SetIcon(0, ico);
    }

    User32::W32_SendMessage((HWND) fm->Handle(), WM_CHANGEUISTATE, 1 /* UIS_INITIALIZE */, 0);
    WindowRegistry::Register(fm->Handle(), fm);

    fm->SetFont(Font::DefaultFont);
    fm->SetText(L"Form");
    return fm;
}

Form *Form::SetLayout(LayoutManager *mng) {
    m_layoutMng = mng;
    return this;
}

Form *Form::UpdateLayout() {
    if (m_layoutMng) m_layoutMng->Update();
    return this;
}

Menu *Form::NewMenu() {
    HMENU hMenu = CreateMenu();
    if (hMenu == 0) exit(1);
    Menu *m = new Menu();
    m->hMenu = hMenu;
    m->hwnd = m_hwnd;
    if (!SetMenu((HWND) m_hwnd, hMenu)) {
        exit(1);
    }
    return m;
}

void Form::DisableIcon() {
    // 获取 WindowInfo 判断 IsPopup
    WINDOWINFO wi = {sizeof(WINDOWINFO)};
    GetWindowInfo((HWND) m_hwnd, &wi);
    if ((wi.dwStyle & WS_POPUP) != 0) return;

    LONG exStyle = GetWindowLongW((HWND) m_hwnd, GWL_EXSTYLE);
    SetWindowLongW((HWND) m_hwnd, GWL_EXSTYLE, exStyle | WS_EX_DLGMODALFRAME);
    SetWindowPos((HWND) m_hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

void Form::Maximise() { ShowWindow((HWND) m_hwnd, SW_MAXIMIZE); }
void Form::Minimise() { ShowWindow((HWND) m_hwnd, SW_MINIMIZE); }

void Form::Restore() {
    const int SC_RESTORE = 0xF120;
    User32::W32_SendMessage((HWND) m_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    ShowWindow((HWND) m_hwnd, SW_RESTORE);
}

void Form::Center() {
    WINDOWINFO wi = {sizeof(WINDOWINFO)};
    GetWindowInfo((HWND) m_hwnd, &wi);
    bool frameless = (wi.dwStyle & WS_POPUP) != 0;

    HMONITOR hMonitor = MonitorFromWindow((HWND) m_hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(MONITORINFO)};
    GetMonitorInfoW(hMonitor, &mi);

    RECT workRect = mi.rcWork;
    int screenMiddleW = workRect.left + (workRect.right - workRect.left) / 2;
    int screenMiddleH = workRect.top + (workRect.bottom - workRect.top) / 2;

    RECT winRect;
    if (!frameless) GetWindowRect((HWND) m_hwnd, &winRect);
    else GetClientRect((HWND) m_hwnd, &winRect);

    int winWidth = winRect.right - winRect.left;
    int winHeight = winRect.bottom - winRect.top;
    int windowX = screenMiddleW - (winWidth / 2);
    int windowY = screenMiddleH - (winHeight / 2);

    SetWindowPos((HWND) m_hwnd, HWND_TOP, windowX, windowY, winWidth, winHeight, SWP_NOSIZE);
}

void Form::Fullscreen() {
    if (m_isFullscreen) return;

    m_previousWindowStyle = GetWindowLongW((HWND) m_hwnd, GWL_STYLE);
    m_previousWindowExStyle = GetWindowLongW((HWND) m_hwnd, GWL_EXSTYLE);

    HMONITOR monitor = MonitorFromWindow((HWND) m_hwnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(MONITORINFO)};
    if (!GetMonitorInfoW(monitor, &mi)) return;
    if (!GetWindowPlacement((HWND) m_hwnd, (WINDOWPLACEMENT *) m_previousWindowPlacement)) return;

    SetWindowLongW((HWND) m_hwnd, GWL_STYLE, m_previousWindowStyle & ~WS_OVERLAPPEDWINDOW | (WS_POPUP | WS_VISIBLE));
    SetWindowLongW((HWND) m_hwnd, GWL_EXSTYLE, m_previousWindowExStyle & ~WS_EX_DLGMODALFRAME);

    m_isFullscreen = true;
    SetWindowPos((HWND) m_hwnd, HWND_TOP,
                 mi.rcMonitor.left, mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left,
                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

void Form::UnFullscreen() {
    if (!m_isFullscreen) return;
    SetWindowLongW((HWND) m_hwnd, GWL_STYLE, m_previousWindowStyle);
    SetWindowLongW((HWND) m_hwnd, GWL_EXSTYLE, m_previousWindowExStyle);
    SetWindowPlacement((HWND) m_hwnd, (WINDOWPLACEMENT *) m_previousWindowPlacement);
    m_isFullscreen = false;
    SetWindowPos((HWND) m_hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

void Form::SetIcon(int iconType, Icon *icon) {
    if (iconType > 1) exit(1);
    if (icon) {
        User32::W32_SendMessage((HWND) m_hwnd, WM_SETICON, (WPARAM) iconType, (LPARAM) icon->Handle());
    }
}

void Form::EnableMaxButton(bool b) { SetStyle((HWND) m_hwnd, b, WS_MAXIMIZEBOX); }
void Form::EnableMinButton(bool b) { SetStyle((HWND) m_hwnd, b, WS_MINIMIZEBOX); }
void Form::EnableSizable(bool b) { SetStyle((HWND) m_hwnd, b, WS_THICKFRAME); }
void Form::EnableDragMove(bool b) {
    /* fm.isDragMove = b */
    (void) b;
}

void Form::EnableTopMost(bool b) {
    SetWindowPos((HWND) m_hwnd, b ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

uintptr_t Form::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_COMMAND:
            if (lparam == 0 && HIWORD((uint32_t) wparam) == 0) {
                uint16_t actionID = (uint16_t) LOWORD((uint32_t) wparam);
                if (actionsByID.count(actionID)) {
                    actionsByID[actionID]->onClick.Fire(Event(this, nullptr));
                }
            }
            break;
        case WM_KEYDOWN:
            Shortcut shortcut = {ModifiersDown(), (Key) wparam};
            if (((uint32_t) lparam >> 30) == 0) {
                if (shortcut2Action.count(shortcut)) {
                    MenuItem *action = shortcut2Action[shortcut];
                    if (action->Enabled()) {
                        action->onClick.Fire(Event(this, nullptr));
                    }
                }
            }
            break;
        case WM_CLOSE:
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
        case WM_PAINT:
            if (m_layoutMng) m_layoutMng->Update();
            break;
        case WM_GETMINMAXINFO: {
            MINMAXINFO *mmi = (MINMAXINFO *) lparam;
            bool hasConstraints = false;
            int minW = m_minWidth, minH = m_minHeight;
            int maxW = m_maxWidth, maxH = m_maxHeight;

            if (minW > 0 || minH > 0) {
                hasConstraints = true;
                this->scaleWithWindowDPI(minW, minH);
                if (minW > 0) mmi->ptMinTrackSize.x = minW;
                if (minH > 0) mmi->ptMinTrackSize.y = minH;
            }
            if (maxW > 0 || maxH > 0) {
                hasConstraints = true;
                this->scaleWithWindowDPI(maxW, maxH);
                if (maxW > 0) mmi->ptMaxTrackSize.x = maxW;
                if (maxH > 0) mmi->ptMaxTrackSize.y = maxH;
            }
            if (hasConstraints) return 0;
            break;
        }
    }
    return DefWindowProcW((HWND) m_hwnd, msg, wparam, lparam);
}

#endif
