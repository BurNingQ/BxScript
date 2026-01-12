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
 * @brief    Pen
 */
#include "Pen.h"
#include <windows.h>
#include "Brush.h"
#include "internal/Gdi32.h"

inline Pen::~Pen() {
    Dispose();
}

inline Pen *Pen::New(unsigned int style, unsigned int width, Brush *brush) {
    if (brush == nullptr) {
        // panic("Brush cannot be nil")
        exit(1);
    }

    // 需要从 Brush 获取 LOGBRUSH 信息
    // 由于 Brush.h 隐藏了 Windows 类型，我们在实现部分获取
    LOGBRUSH lb = {0};
    if (brush->GetHandle()) {
        GetObjectW(brush->GetHandle(), sizeof(LOGBRUSH), &lb);
    }

    HPEN hPen = Gdi32::W32_ExtCreatePen(style, width, &lb, 0, nullptr);
    if (!hPen) {
        exit(1);
    }

    Pen *pen = new Pen();
    pen->m_hPen = static_cast<void *>(hPen);
    pen->m_style = style;
    pen->m_brush = brush;
    return pen;
}

inline Pen *Pen::NewNull() {
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

inline void Pen::Dispose() {
    if (m_hPen) {
        Gdi32::W32_DeleteObject(static_cast<HGDIOBJ>(m_hPen));
        m_hPen = nullptr;
    }
}
