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
 * @brief    Controller
 */
#ifndef BXSCRIPT_CONTROLLER_H
#define BXSCRIPT_CONTROLLER_H

#include <string>
#include <functional>

#include "Font.h"

class ControlBase;
class Rect;
class MenuItem;
class EventManager;

class Controller {
public:
    virtual ~Controller() = default;

    virtual std::wstring Text() = 0;

    virtual bool Enabled() = 0;
    virtual void SetFocus() = 0;

    virtual void* Handle() = 0;
    virtual void Invalidate(bool erase) = 0;
    virtual Controller* Parent() = 0;

    virtual void Pos(int& x, int& y) = 0;
    virtual void Size(int& w, int& h) = 0;
    virtual int Height() = 0;
    virtual int Width() = 0;
    virtual bool Visible() = 0;
    virtual Rect* Bounds() = 0;
    virtual Rect* ClientRect() = 0;

    virtual ControlBase* SetText(const std::wstring& s) = 0;
    virtual ControlBase* SetEnabled(bool b) = 0;
    virtual ControlBase* SetPos(int x, int y) = 0;
    virtual ControlBase* SetSize(int w, int h) = 0;
    virtual ControlBase* EnableDragAcceptFiles(bool b) = 0;
    virtual ControlBase* Show() = 0;
    virtual ControlBase* Hide() = 0;

    virtual MenuItem* ContextMenu() = 0;
    virtual void SetContextMenu(MenuItem* menu) = 0;

    virtual Font* GetFont() = 0;
    virtual void SetFont(Font* font) = 0;
    virtual bool InvokeRequired() = 0;
    virtual void Invoke(std::function<void()>) = 0;
    virtual bool PreTranslateMessage(void* msg) = 0;
    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) = 0;

    // General events
    virtual EventManager& OnCreate() = 0;
    virtual EventManager& OnClose() = 0;

    // Focus events
    virtual EventManager& OnKillFocus() = 0;
    virtual EventManager& OnSetFocus() = 0;

    // Drag and drop events
    virtual EventManager& OnDropFiles() = 0;

    // Mouse events
    virtual EventManager& OnLBDown() = 0;
    virtual EventManager& OnLBUp() = 0;
    virtual EventManager& OnLBDbl() = 0;
    virtual EventManager& OnMBDown() = 0;
    virtual EventManager& OnMBUp() = 0;
    virtual EventManager& OnRBDown() = 0;
    virtual EventManager& OnRBUp() = 0;
    virtual EventManager& OnRBDbl() = 0;
    virtual EventManager& OnMouseMove() = 0;
    virtual EventManager& OnMouseWheel() = 0;

    virtual EventManager& OnMouseHover() = 0;
    virtual EventManager& OnMouseLeave() = 0;

    // Keyboard events
    virtual EventManager& OnKeyUp() = 0;
    virtual EventManager& OnKeyDown() = 0;

    // Paint events
    virtual EventManager& OnPaint() = 0;
    virtual EventManager& OnSize() = 0;

    virtual void invokeCallbacks() = 0;
};

#endif