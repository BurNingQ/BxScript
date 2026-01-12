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
