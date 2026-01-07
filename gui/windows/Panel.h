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
 * @brief    Panel
 */
#ifndef BXSCRIPT_PANEL_H
#define BXSCRIPT_PANEL_H

#include "ControlBase.h"
#include "Form.h"
#include "Rect.h"
#include "Pen.h"
#include "Brush.h"
#include "Canvas.h"
#include <vector>
#include "Utils.h"

class Panel : public ControlBase {
protected:
    LayoutManager *m_layoutMng = nullptr;

public:
    Panel() = default;

    ~Panel() override = default;

    static Panel *New(Controller *parent);

    void SetLayout(LayoutManager *mng);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

class ErrorPanel final : public ControlBase {
protected:
    Pen *m_pen = nullptr;
    int m_margin = 0;

public:
    ErrorPanel() = default;

    virtual ~ErrorPanel() = default;

    static ErrorPanel *New(Controller *parent);

    void SetMargin(int margin);

    void Printf(const std::wstring &format, ...);

    void Errorf(const std::wstring &format, ...);

    void ShowAsError(bool show);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;

private:
    void init(Controller *parent);
};

class MultiPanel : public ControlBase {
protected:
    int m_current = -1;
    std::vector<Panel *> m_panels;

public:
    MultiPanel() = default;

    ~MultiPanel() override = default;

    static MultiPanel *New(Controller *parent);

    int Count() const;

    void AddPanel(Panel *panel);

    void ReplacePanel(int index, Panel *panel);

    void DeletePanel(int index);

    int Current() const;

    void SetCurrent(int index);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include <cwchar>
#include <cstdarg>
#include "internal/User32.h"
#include "GlobalVars.h"

// Static pens for ErrorPanel logic
inline static Pen *errorPanelPen = nullptr;
inline static Pen *errorPanelOkPen = nullptr;

namespace {
    void InitPens() {
        if (!errorPanelPen) {
            errorPanelPen = Pen::New(0x00010000, 2, Brush::NewSolid(Color::FromRGB(255, 128, 128)));
        }
        if (!errorPanelOkPen) {
            errorPanelOkPen = Pen::New(0x00010000, 2, Brush::NewSolid(Color::FromRGB(220, 220, 220)));
        }
    }
}

// --- Panel Implementation ---

Panel *Panel::New(Controller *parent) {
    Panel *pa = new Panel();
    RegClassOnlyOnce(L"BxPanel");

    pa->InitWindow(L"BxPanel", parent, WS_EX_CONTROLPARENT, WS_CHILD | WS_VISIBLE | WS_BORDER);
    pa->SetParent(parent);

    pa->SetFont(DefaultFont);
    pa->SetText(L"");
    pa->SetSize(200, 65);
    return pa;
}

void Panel::SetLayout(LayoutManager *mng) {
    m_layoutMng = mng;
}

uintptr_t Panel::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_SIZE:
        case WM_PAINT:
            if (m_layoutMng) {
                m_layoutMng->Update();
            }
            break;
    }
    return User32::W32_DefWindowProc((HWND) m_hwnd, msg, wparam, lparam);
}

// --- ErrorPanel Implementation ---

ErrorPanel *ErrorPanel::New(Controller *parent) {
    InitPens();
    ErrorPanel *f = new ErrorPanel();
    f->init(parent);

    f->SetFont(DefaultFont);
    f->SetText(L"No errors");
    f->SetSize(200, 65);
    f->m_margin = 5;
    f->m_pen = errorPanelOkPen;
    return f;
}

void ErrorPanel::init(Controller *parent) {
    RegClassOnlyOnce(L"BxErrorPanel");
    m_hwnd = User32::W32_CreateWindowEx(WS_EX_CONTROLPARENT, L"BxErrorPanel", L"", WS_CHILD | WS_VISIBLE,
                                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                        parent ? (HWND) parent->Handle() : nullptr, nullptr, (HINSTANCE) gAppInstance, nullptr);
    m_parent = parent;
    WindowRegistry::Register(m_hwnd, this);
}

void ErrorPanel::SetMargin(int margin) {
    m_margin = margin;
}

void ErrorPanel::Printf(const std::wstring &format, ...) {
    wchar_t buf[1024];
    va_list args;
    va_start(args, format);
    vswprintf(buf, 1024, format.c_str(), args);
    va_end(args);
    this->SetText(buf);
    this->ShowAsError(false);
}

void ErrorPanel::Errorf(const std::wstring &format, ...) {
    wchar_t buf[1024];
    va_list args;
    va_start(args, format);
    vswprintf(buf, 1024, format.c_str(), args);
    va_end(args);
    this->SetText(buf);
    this->ShowAsError(true);
}

void ErrorPanel::ShowAsError(bool show) {
    if (show) {
        m_pen = errorPanelPen;
    } else {
        m_pen = errorPanelOkPen;
    }
    this->Invalidate(true);
}

uintptr_t ErrorPanel::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_ERASEBKGND: {
            Canvas *canvas = Canvas::FromHDC((HDC) wparam);
            Rect *r = this->Bounds();
            r->Left += m_margin;
            r->Right -= m_margin;
            r->Top += m_margin;
            r->Bottom -= m_margin;

            Brush *winBrush = Brush::NewSystem(5); // COLOR_WINDOW
            canvas->DrawFillRect(*r, m_pen, winBrush);

            r->Left += 5;
            canvas->DrawText(this->Text(), *r, 0, this->GetFont(), Color::FromRGB(0, 0, 0));

            delete winBrush;
            delete r;
            delete canvas;
            return 1;
        }
    }
    return User32::W32_DefWindowProc((HWND) m_hwnd, msg, wparam, lparam);
}

// --- MultiPanel Implementation ---

MultiPanel *MultiPanel::New(Controller *parent) {
    MultiPanel *mpa = new MultiPanel();
    RegClassOnlyOnce(L"BxMultiPanel");
    mpa->InitWindow(L"BxMultiPanel", parent, WS_EX_CONTROLPARENT, WS_CHILD | WS_VISIBLE | WS_BORDER);
    mpa->SetParent(parent);

    mpa->SetFont(DefaultFont);
    mpa->SetText(L"");
    mpa->SetSize(300, 200);
    mpa->m_current = -1;
    return mpa;
}

int MultiPanel::Count() const {
    return (int) m_panels.size();
}

void MultiPanel::AddPanel(Panel *panel) {
    if (!m_panels.empty()) {
        panel->Hide();
    }
    m_current = 0;
    m_panels.push_back(panel);
}

void MultiPanel::ReplacePanel(int index, Panel *panel) {
    m_panels[index] = panel;
}

void MultiPanel::DeletePanel(int index) {
    m_panels.erase(m_panels.begin() + index);
}

int MultiPanel::Current() const {
    return m_current;
}

void MultiPanel::SetCurrent(int index) {
    if (index >= (int) m_panels.size()) {
        exit(1); // panic
    }
    if (m_current == -1) {
        exit(1); // panic
    }
    for (size_t i = 0; i < m_panels.size(); ++i) {
        if ((int) i != index) {
            m_panels[i]->Hide();
            m_panels[i]->Invalidate(true);
        }
    }
    m_panels[index]->Show();
    m_panels[index]->Invalidate(true);
    m_current = index;
}

uintptr_t MultiPanel::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_SIZE: {
            int w, h;
            this->Size(w, h);
            for (auto *p: m_panels) {
                p->SetPos(0, 0);
                p->SetSize(w, h);
            }
            break;
        }
    }
    return User32::W32_DefWindowProc((HWND) m_hwnd, msg, wparam, lparam);
}

#endif
