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
 * @brief    Form
 */

#include "Form.h"
#include <windows.h>
#include "internal/User32.h"
#include "WindowRegistry.h"
#include "App.h"

Form *Form::NewCustom(Controller *parent, int exStyle, unsigned int dwStyle) {
    const auto fm = new Form();
    RegClassOnlyOnce(L"BxForm");
    fm->SetIsForm(true);
    if (exStyle == 0) {
        exStyle = WS_EX_CONTROLPARENT | WS_EX_APPWINDOW;
    }
    if (dwStyle == 0) {
        dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
    }
    const HWND parentHwnd = parent ? static_cast<HWND>(parent->Handle()) : nullptr;
    fm->SetHandle(CreateWindowExW(exStyle, L"BxForm", L"", dwStyle,
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                  parentHwnd, nullptr, static_cast<HINSTANCE>(App::GetInstance()), nullptr));
    fm->SetParent(parent);
    Icon *ico = Icon::NewIconFromResource(App::GetInstance(), 3);
    if (ico) {
        fm->SetIcon(0, ico);
    }
    User32::W32_SendMessage(static_cast<HWND>(fm->Handle()), WM_CHANGEUISTATE, 1 /* UIS_INITIALIZE */, 0);
    WindowRegistry::Register(fm->Handle(), fm);
    fm->SetFont(DefaultFont);
    fm->SetText(L"Form");
    return fm;
}

Form *Form::New(Controller *parent) {
    auto fm = new Form();
    RegClassOnlyOnce(L"BxForm");
    fm->SetIsForm(true);

    constexpr DWORD exStyle = WS_EX_CONTROLPARENT | WS_EX_APPWINDOW | WS_EX_CLIENTEDGE;
    constexpr DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;

    const HWND parentHwnd = parent ? static_cast<HWND>(parent->Handle()) : nullptr;
    fm->SetHandle(CreateWindowExW(exStyle, L"BxForm", L"", dwStyle,
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                  parentHwnd, nullptr, static_cast<HINSTANCE>(App::GetInstance()), nullptr));
    fm->SetParent(parent);

    Icon *ico = Icon::NewIconFromResource(App::GetInstance(), 1);
    if (ico) {
        fm->SetIcon(0, ico);
    }

    User32::W32_SendMessage(static_cast<HWND>(fm->Handle()), WM_CHANGEUISTATE, 1 /* UIS_INITIALIZE */, 0);
    WindowRegistry::Register(fm->Handle(), fm);

    fm->SetFont(DefaultFont);
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

Menu *Form::NewMenu() const {
    const HMENU hMenu = CreateMenu();
    if (hMenu == nullptr) exit(1);
    const auto m = new Menu();
    m->hMenu = hMenu;
    m->hwnd = m_hwnd;
    if (!SetMenu(static_cast<HWND>(m_hwnd), hMenu)) {
        exit(1);
    }
    return m;
}

void Form::DisableIcon() const {
    // 获取 WindowInfo 判断 IsPopup
    WINDOWINFO wi = {sizeof(WINDOWINFO)};
    GetWindowInfo(static_cast<HWND>(m_hwnd), &wi);
    if ((wi.dwStyle & WS_POPUP) != 0) return;

    const LONG exStyle = GetWindowLongW(static_cast<HWND>(m_hwnd), GWL_EXSTYLE);
    SetWindowLongW(static_cast<HWND>(m_hwnd), GWL_EXSTYLE, exStyle | WS_EX_DLGMODALFRAME);
    SetWindowPos(static_cast<HWND>(m_hwnd), nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

void Form::Maximise() const { ShowWindow(static_cast<HWND>(m_hwnd), SW_MAXIMIZE); }
void Form::Minimise() const { ShowWindow(static_cast<HWND>(m_hwnd), SW_MINIMIZE); }

void Form::Restore() const {
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), WM_SYSCOMMAND, 0xF120, 0);
    ShowWindow(static_cast<HWND>(m_hwnd), SW_RESTORE);
}

void Form::Center() const {
    WINDOWINFO wi = {sizeof(WINDOWINFO)};
    GetWindowInfo(static_cast<HWND>(m_hwnd), &wi);
    const bool frameless = (wi.dwStyle & WS_POPUP) != 0;

    const HMONITOR hMonitor = MonitorFromWindow(static_cast<HWND>(m_hwnd), MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(MONITORINFO)};
    GetMonitorInfoW(hMonitor, &mi);

    auto [left, top, right, bottom] = mi.rcWork;
    const int screenMiddleW = left + (right - left) / 2;
    const int screenMiddleH = top + (bottom - top) / 2;

    RECT winRect;
    if (!frameless) GetWindowRect(static_cast<HWND>(m_hwnd), &winRect);
    else GetClientRect(static_cast<HWND>(m_hwnd), &winRect);

    const int winWidth = winRect.right - winRect.left;
    const int winHeight = winRect.bottom - winRect.top;
    const int windowX = screenMiddleW - (winWidth / 2);
    const int windowY = screenMiddleH - (winHeight / 2);

    SetWindowPos(static_cast<HWND>(m_hwnd), HWND_TOP, windowX, windowY, winWidth, winHeight, SWP_NOSIZE);
}

void Form::Fullscreen() {
    if (m_isFullscreen) return;

    m_previousWindowStyle = GetWindowLongW(static_cast<HWND>(m_hwnd), GWL_STYLE);
    m_previousWindowExStyle = GetWindowLongW(static_cast<HWND>(m_hwnd), GWL_EXSTYLE);

    HMONITOR monitor = MonitorFromWindow(static_cast<HWND>(m_hwnd), MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(MONITORINFO)};
    if (!GetMonitorInfoW(monitor, &mi)) return;
    if (!GetWindowPlacement(static_cast<HWND>(m_hwnd), reinterpret_cast<WINDOWPLACEMENT *>(m_previousWindowPlacement))) return;

    SetWindowLongW(static_cast<HWND>(m_hwnd), GWL_STYLE, m_previousWindowStyle & ~WS_OVERLAPPEDWINDOW | (WS_POPUP | WS_VISIBLE));
    SetWindowLongW(static_cast<HWND>(m_hwnd), GWL_EXSTYLE, m_previousWindowExStyle & ~WS_EX_DLGMODALFRAME);

    m_isFullscreen = true;
    SetWindowPos(static_cast<HWND>(m_hwnd), HWND_TOP,
                 mi.rcMonitor.left, mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left,
                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

void Form::UnFullscreen() {
    if (!m_isFullscreen) return;
    SetWindowLongW(static_cast<HWND>(m_hwnd), GWL_STYLE, m_previousWindowStyle);
    SetWindowLongW(static_cast<HWND>(m_hwnd), GWL_EXSTYLE, m_previousWindowExStyle);
    SetWindowPlacement(static_cast<HWND>(m_hwnd), reinterpret_cast<WINDOWPLACEMENT *>(m_previousWindowPlacement));
    m_isFullscreen = false;
    SetWindowPos(static_cast<HWND>(m_hwnd), nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

void Form::SetIcon(int iconType, const Icon *icon) const {
    if (iconType > 1) exit(1);
    if (icon) {
        User32::W32_SendMessage(static_cast<HWND>(m_hwnd), WM_SETICON, static_cast<WPARAM>(iconType), reinterpret_cast<LPARAM>(icon->Handle()));
    }
}

void Form::EnableMaxButton(bool b) const { SetStyle((HWND) m_hwnd, b, WS_MAXIMIZEBOX); }
void Form::EnableMinButton(bool b) const { SetStyle((HWND) m_hwnd, b, WS_MINIMIZEBOX); }
void Form::EnableSizable(bool b) const { SetStyle((HWND) m_hwnd, b, WS_THICKFRAME); }

void Form::EnableDragMove(bool b) {
    /* fm.isDragMove = b */
    (void) b;
}

void Form::EnableTopMost(bool b) const {
    SetWindowPos(static_cast<HWND>(m_hwnd), b ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void Form::SetTrayIcon(const std::wstring &iconPath, const std::wstring &tooltip) {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = static_cast<HWND>(m_hwnd);
    nid.uID = 1; // ID
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;

    if (!iconPath.empty()) {
        HANDLE hIcon = LoadImageW(nullptr, iconPath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        if (hIcon) {
            nid.hIcon = static_cast<HICON>(hIcon);
            trayIconHandle = hIcon;
        } else {
            nid.hIcon = reinterpret_cast<HICON>(SendMessage(static_cast<HWND>(m_hwnd), WM_GETICON, ICON_BIG, 0));
        }
    }

    if (!tooltip.empty()) {
        wcsncpy(nid.szTip, tooltip.c_str(), 127);
    } else {
        wcsncpy(nid.szTip, L"BxScript App", 127);
    }

    if (hasTray) {
        Shell_NotifyIconW(NIM_MODIFY, &nid);
    } else {
        Shell_NotifyIconW(NIM_ADD, &nid);
        hasTray = true;
    }
}

void Form::RemoveTrayIcon() {
    if (!hasTray) return;
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = static_cast<HWND>(m_hwnd);
    nid.uID = 1;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    hasTray = false;
    if (trayIconHandle) {
        DestroyIcon(static_cast<HICON>(trayIconHandle));
        trayIconHandle = nullptr;
    }
}

void Form::ShowTrayBalloon(const std::wstring &title, const std::wstring &msg) const {
    if (!hasTray) return;
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = static_cast<HWND>(m_hwnd);
    nid.uID = 1;
    nid.uFlags = NIF_INFO;
    wcsncpy(nid.szInfo, msg.c_str(), 255);
    wcsncpy(nid.szInfoTitle, title.c_str(), 63);
    nid.dwInfoFlags = NIIF_INFO;
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}


uintptr_t Form::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_COMMAND:
            if (lparam == 0 && HIWORD(static_cast<uint32_t>(wparam)) == 0) {
                uint16_t actionID = LOWORD(static_cast<uint32_t>(wparam));
                if (actionsByID.count(actionID)) {
                    actionsByID[actionID]->onClick.Fire(Event(this, nullptr));
                }
            }
            break;
        case WM_TRAYICON:
            if (lparam == WM_LBUTTONUP || lparam == WM_RBUTTONUP) {
                this->Show();
                this->Restore();
                SetForegroundWindow(static_cast<HWND>(m_hwnd));
                onTrayClick.Fire(Event(this, nullptr));
            }
            break;
        case WM_KEYDOWN: {
            const Shortcut shortcut = {ModifiersDown(), static_cast<Key>(wparam)};
            if ((static_cast<uint32_t>(lparam) >> 30) == 0) {
                if (shortcut2Action.count(shortcut)) {
                    MenuItem *action = shortcut2Action[shortcut];
                    if (action->Enabled()) {
                        action->onClick.Fire(Event(this, nullptr));
                    }
                }
            }
        }
        break;
        case WM_CLOSE:
            break;
        case WM_DESTROY:
            RemoveTrayIcon();
            App::Exit(0);
            return 0;
        case WM_SIZE:
        case WM_PAINT:
            if (m_layoutMng) m_layoutMng->Update();
            break;
        case WM_GETMINMAXINFO: {
            const auto mmi = reinterpret_cast<MINMAXINFO *>(lparam);
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
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORLISTBOX: {
            const auto hdc = reinterpret_cast<HDC>(wparam);
            const auto hChild = reinterpret_cast<HWND>(lparam);
            if (const auto *ctrl = WindowRegistry::Get(hChild)) {
                if (auto brush = ctrl->HandleCtlColor(hdc, msg)) {
                    return reinterpret_cast<uintptr_t>(brush);
                }
            }
            break;
        }
        case WM_ERASEBKGND: {
            if (this->defineBackgroundColor && this->formBrush) {
                HDC hdc = reinterpret_cast<HDC>(wparam);
                RECT rc;
                GetClientRect(static_cast<HWND>(m_hwnd), &rc);
                FillRect(hdc, &rc, static_cast<HBRUSH>(this->formBrush));
                return 1;
            }
            break;
            break;
        }
        default: ;
    }
    return DefWindowProcW(static_cast<HWND>(m_hwnd), msg, wparam, lparam);
}
