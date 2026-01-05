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
 * @brief    Pen
 */
#ifndef BXSCRIPT_PEN_H
#define BXSCRIPT_PEN_H

#include <cstdint>

class Brush;

class Pen {

    void *m_hPen = nullptr;
    unsigned int m_style = 0;
    Brush *m_brush = nullptr;

public:
    Pen() = default;

    ~Pen();

    static Pen *New(unsigned int style, unsigned int width, Brush *brush);

    static Pen *NewNull();

    unsigned int Style() const { return m_style; }
    Brush *GetBrush() const { return m_brush; }
    void *GetHandle() const { return m_hPen; }

    void Dispose();
};

#endif // BXSCRIPT_PEN_H

// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "Brush.h"
#include "internal/Gdi32.h"

Pen::~Pen() {
    Dispose();
}

Pen *Pen::New(unsigned int style, unsigned int width, Brush *brush) {
    if (brush == nullptr) {
        // panic("Brush cannot be nil")
        exit(1);
    }

    // 需要从 Brush 获取 LOGBRUSH 信息
    // 由于 Brush.h 隐藏了 Windows 类型，我们在实现部分获取
    LOGBRUSH lb = {0};
    if (brush->GetHandle()) {
        GetObjectW(static_cast<HGDIOBJ>(brush->GetHandle()), sizeof(LOGBRUSH), &lb);
    }

    HPEN hPen = Gdi32::W32_ExtCreatePen(style, width, &lb, 0, nullptr);
    if (!hPen) {
        // panic("Failed to create pen")
        exit(1);
    }

    Pen *pen = new Pen();
    pen->m_hPen = static_cast<void *>(hPen);
    pen->m_style = style;
    pen->m_brush = brush;
    return pen;
}

Pen *Pen::NewNull() {
    LOGBRUSH lb = {0};
    lb.lbStyle = BS_NULL;

    // w32.PS_COSMETIC | w32.PS_NULL = 0x00000000 | 0x00000005
    HPEN hPen = Gdi32::W32_ExtCreatePen(PS_COSMETIC | PS_NULL, 1, &lb, 0, nullptr);
    if (!hPen) {
        // panic("failed to create null brush")
        exit(1);
    }

    Pen *pen = new Pen();
    pen->m_hPen = static_cast<void *>(hPen);
    pen->m_style = PS_NULL;
    return pen;
}

void Pen::Dispose() {
    if (m_hPen) {
        Gdi32::W32_DeleteObject(static_cast<HGDIOBJ>(m_hPen));
        m_hPen = nullptr;
    }
}

#endif // BXSCRIPT_IMPLEMENTATION
