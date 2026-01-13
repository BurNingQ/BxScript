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
#include "Utils.h"
#include "Menu.h"
#include "Icon.h"

class LayoutManager {
public:
    virtual ~LayoutManager() = default;

    virtual void Update() = 0;
};

class Form final : public ControlBase {
    LayoutManager *m_layoutMng = nullptr;

    // Fullscreen / Unfullscreen state
    bool m_isFullscreen = false;
    unsigned int m_previousWindowStyle = 0;
    unsigned int m_previousWindowExStyle = 0;
    // WINDOWPLACEMENT 大小通常为 44 字节
    uint8_t m_previousWindowPlacement[44]{};

public:
    Form() = default;

    ~Form() override = default;

    // Static Factory Methods
    static Form *NewCustom(Controller *parent, int exStyle, unsigned int dwStyle);

    static Form *New(Controller *parent);

    Form *SetLayout(LayoutManager *mng);

    Form *UpdateLayout();

    Menu *NewMenu() const;

    void DisableIcon() const;

    void Maximise() const;

    void Minimise() const;

    void Restore() const;

    void Center() const;

    void Fullscreen();

    void UnFullscreen();

    bool IsFullScreen() const { return m_isFullscreen; }

    void SetIcon(int iconType, const Icon *icon) const;

    void EnableMaxButton(bool b) const;

    void EnableMinButton(bool b) const;

    void EnableSizable(bool b) const;

    static void EnableDragMove(bool b);

    void EnableTopMost(bool b) const;

    // Override WndProc from ControlBase
    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif // BXSCRIPT_FORM_H
