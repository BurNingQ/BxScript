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
 * @brief    ControlBase
 */
#ifndef BXSCRIPT_CONTROL_BASE_H
#define BXSCRIPT_CONTROL_BASE_H

#include "Controller.h"
#include "EventManager.h"
#include <vector>
#include <mutex>
#include <windef.h>

#include "Color.h"

class ControlBase : public Controller {
protected:
    void *m_hwnd = nullptr;
    Font *m_font = nullptr;
    Controller *parentHwnd = nullptr;
    MenuItem *contextMenu = nullptr;

    bool isForm = false;

    int m_minWidth = 0, m_minHeight = 0;
    int m_maxWidth = 0, m_maxHeight = 0;

    // Events
    EventManager onCreate;
    EventManager onClose;
    EventManager onKillFocus;
    EventManager onSetFocus;
    EventManager onDropFiles;
    EventManager onLBDown;
    EventManager onLBUp;
    EventManager onLBDbl;
    EventManager onMBDown;
    EventManager onMBUp;
    EventManager onRBDown;
    EventManager onRBUp;
    EventManager onRBDbl;
    EventManager onMouseMove;
    EventManager onMouseHover;
    EventManager onMouseLeave;
    EventManager onKeyUp;
    EventManager onKeyDown;
    EventManager onPaint;
    EventManager onSize;
    EventManager onMouseWheel;

    std::mutex m_mutex;
    std::vector<std::function<void()> > uiThreadDispatch;

    Color fontColor = Color::Black();
    Color bgColor = Color::White();
    bool defineFontColor = false;
    bool defineBackgroundColor = false;
    void* formBrush = nullptr;

public:
    ~ControlBase() override;

    std::wstring Text() override;

    bool Enabled() override;

    void SetFocus() override;

    void *Handle() override { return m_hwnd; }

    void Invalidate(bool erase) override;

    Controller *Parent() override { return parentHwnd; }

    void SetParent(Controller *parent) { parentHwnd = parent; }

    void Pos(int &x, int &y) override;

    void Size(int &w, int &h) override;

    int Height() override;

    int Width() override;

    bool Visible() override;

    Rect *Bounds() override;

    Rect *ClientRect() override;

    ControlBase *SetText(const std::wstring &s) override;

    ControlBase *SetEnabled(bool b) override;

    ControlBase *SetPos(int x, int y) override;

    ControlBase *SetSize(int w, int h) override;

    ControlBase *EnableDragAcceptFiles(bool b) override;

    ControlBase *Show() override;

    ControlBase *Hide() override;

    ControlBase *SetTextColor(Color c);

    ControlBase *SetBackgroundColor(Color c);

    void* HandleCtlColor(void* hdc, unsigned int uMsg) const;

    MenuItem *ContextMenu() override { return contextMenu; }
    void SetContextMenu(MenuItem *menu) override { contextMenu = menu; }

    Font *GetFont() override { return m_font; }

    void SetFont(Font *font) override;

    bool InvokeRequired() override;

    void Invoke(std::function<void()> f) override;

    bool PreTranslateMessage(void *msg) override;

    uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;

    // Event Accessors
    EventManager &OnCreate() override { return onCreate; }
    EventManager &OnClose() override { return onClose; }
    EventManager &OnKillFocus() override { return onKillFocus; }
    EventManager &OnSetFocus() override { return onSetFocus; }
    EventManager &OnDropFiles() override { return onDropFiles; }
    EventManager &OnLBDown() override { return onLBDown; }
    EventManager &OnLBUp() override { return onLBUp; }
    EventManager &OnLBDbl() override { return onLBDbl; }
    EventManager &OnMBDown() override { return onMBDown; }
    EventManager &OnMBUp() override { return onMBUp; }
    EventManager &OnRBDown() override { return onRBDown; }
    EventManager &OnRBUp() override { return onRBUp; }
    EventManager &OnRBDbl() override { return onRBDbl; }
    EventManager &OnMouseMove() override { return onMouseMove; }
    EventManager &OnMouseHover() override { return onMouseHover; }
    EventManager &OnMouseLeave() override { return onMouseLeave; }
    EventManager &OnKeyUp() override { return onKeyUp; }
    EventManager &OnKeyDown() override { return onKeyDown; }
    EventManager &OnPaint() override { return onPaint; }
    EventManager &OnSize() override { return onSize; }
    EventManager &OnMouseWheel() override { return onMouseWheel; }

    void invokeCallbacks() override;

    // 初始化方法
    void InitControl(const std::wstring &className, Controller *parent, unsigned int exstyle, unsigned int style);

    void InitWindow(const std::wstring &className, Controller *parent, unsigned int exstyle, unsigned int style);

    void SetTheme(const std::wstring &appName) const;

    void SetHandle(void *hwnd) { m_hwnd = hwnd; }

    void GetWindowDPI(unsigned int &dpiX, unsigned int &dpiY) const;

    void SetAndClearStyleBits(unsigned int set, unsigned int clear) const;

    void SetIsForm(bool isform) { isForm = isform; }

    void SetTranslucentBackground();

    void SetMinSize(int width, int height);

    void SetMaxSize(int width, int height);

    void SetAlwaysOnTop(bool b) const;

    void ShowTop() const;

    bool ToggleVisible();

    void clampSize(int &width, int &height) const;

    void scaleWithWindowDPI(int &width, int &height) const;

    void scaleToDefaultDPI(int &width, int &height) const;
};

#endif
