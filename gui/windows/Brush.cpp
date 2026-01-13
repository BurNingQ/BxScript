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
 * @brief    Brush
 */
#include <windows.h>
#include "internal/Gdi32.h"
#include "internal/User32.h"
#include "Brush.h"

// 辅助宏
#define HBRUSH_CAST(ptr) static_cast<HBRUSH>(ptr)

Brush::~Brush() {
    Dispose();
}

void Brush::Dispose() {
    if (m_hBrush) {
        Gdi32::W32_DeleteObject(HBRUSH_CAST(m_hBrush));
        m_hBrush = nullptr;
    }
}

Brush *Brush::NewSolid(Color color) {
    const HBRUSH h = Gdi32::W32_CreateSolidBrush(color.Value());
    if (!h) return nullptr;
    const auto b = new Brush();
    b->m_hBrush = h;
    return b;
}

Brush *Brush::NewSystem(int colorIndex) {
    const HBRUSH h = GetSysColorBrush(colorIndex);
    if (!h) return nullptr;
    const auto b = new Brush();
    b->m_hBrush = h;
    return b;
}

Brush *Brush::NewSystem(SysColorIndex index) {
    return NewSystem(static_cast<int>(index));
}

Brush *Brush::NewHatched(Color color, HatchStyle style) {
    LOGBRUSH lb;
    lb.lbStyle = BS_HATCHED;
    lb.lbColor = static_cast<COLORREF>(color.Value());
    lb.lbHatch = static_cast<ULONG_PTR>(style);
    const HBRUSH h = Gdi32::W32_CreateBrushIndirect(&lb);
    if (!h) return nullptr;
    const auto b = new Brush();
    b->m_hBrush = h;
    return b;
}

Brush *Brush::NewNull() {
    LOGBRUSH lb;
    lb.lbStyle = BS_NULL;
    lb.lbColor = 0;
    lb.lbHatch = 0;
    const HBRUSH h = Gdi32::W32_CreateBrushIndirect(&lb);
    if (!h) return nullptr;
    const auto b = new Brush();
    b->m_hBrush = h;
    return b;
}

Brush *Brush::DefaultBackground() {
    return NewSystem(static_cast<int>(SysColorIndex::BtnFace));
}