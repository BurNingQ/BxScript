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
 * @brief    Brush
 */
#ifndef BXSCRIPT_BRUSH_H
#define BXSCRIPT_BRUSH_H

#include "Color.h"
#include <utility>


// 阴影线样式 (对应 HS_HORIZONTAL 等)
enum class HatchStyle {
    Horizontal = 0, // HS_HORIZONTAL
    Vertical = 1, // HS_VERTICAL
    FDiagonal = 2, // HS_FDIAGONAL
    BDiagonal = 3, // HS_BDIAGONAL
    Cross = 4, // HS_CROSS
    DiagCross = 5 // HS_DIAGCROSS
};

// 系统颜色索引 (对应 COLOR_BTNFACE 等)
enum class SysColorIndex {
    Scrollbar = 0, Background = 1, ActiveCaption = 2, InactiveCaption = 3,
    Menu = 4, Window = 5, WindowFrame = 6, MenuText = 7, WindowText = 8,
    CaptionText = 9, ActiveBorder = 10, InactiveBorder = 11, AppWorkspace = 12,
    Highlight = 13, HighlightText = 14, BtnFace = 15, BtnShadow = 16,
    GrayText = 17, BtnText = 18, InactiveCaptionText = 19, BtnHighlight = 20,

    Desktop = 1, ThreeDFace = 15, ThreeDShadow = 16, ThreeDHighlight = 20
};

class Brush {
    void *m_hBrush = nullptr; // HBRUSH

public:
    Brush(const Brush &) = delete;

    Brush &operator=(const Brush &) = delete;

    // 允许移动构造/赋值
    Brush(Brush &&other) noexcept : m_hBrush(std::exchange(other.m_hBrush, nullptr)) {
    }

    Brush &operator=(Brush &&other) noexcept {
        if (this != &other) {
            Dispose();
            m_hBrush = std::exchange(other.m_hBrush, nullptr);
        }
        return *this;
    }

    // 默认构造
    Brush() = default;

    // 析构函数
    ~Brush();

    // ======================== 工厂方法 ========================

    /**
     * 创建实心颜色画刷。
     */
    static Brush *NewSolid(Color color);

    /**
     * 创建系统颜色画刷 (如按钮面颜色)。
     */
    static Brush *NewSystem(int colorIndex);

    static Brush *NewSystem(SysColorIndex index);

    /**
     * 创建阴影线画刷 (网格、斜线等)。
     */
    static Brush *NewHatched(Color color, HatchStyle style);

    /**
     * 创建空画刷 (透明)。
     */
    static Brush *NewNull();

    /**
     * 获取默认背景画刷 (通常是 COLOR_BTNFACE)。
     */
    static Brush *DefaultBackground();

    // ======================== 方法 ========================

    void *GetHandle() const { return m_hBrush; }

    // 销毁资源
    void Dispose();

    // 判断是否有效
    bool IsValid() const { return m_hBrush != nullptr; }
};

#endif // BXSCRIPT_BRUSH_H


// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/Gdi32.h"
#include "internal/User32.h"

// 辅助宏
#define HBRUSH_CAST(ptr) static_cast<HBRUSH>(ptr)

inline Brush::~Brush() {
    Dispose();
}

inline void Brush::Dispose() {
    if (m_hBrush) {
        Gdi32::W32_DeleteObject(HBRUSH_CAST(m_hBrush));
        m_hBrush = nullptr;
    }
}

inline Brush *Brush::NewSolid(Color color) {
    const HBRUSH h = Gdi32::W32_CreateSolidBrush(color.Value());
    if (!h) return nullptr;
    const auto b = new Brush();
    b->m_hBrush = h;
    return b;
}

inline Brush *Brush::NewSystem(int colorIndex) {
    const HBRUSH h = GetSysColorBrush(colorIndex);
    if (!h) return nullptr;
    const auto b = new Brush();
    b->m_hBrush = h;
    return b;
}

inline Brush *Brush::NewSystem(SysColorIndex index) {
    return NewSystem(static_cast<int>(index));
}

inline Brush *Brush::NewHatched(Color color, HatchStyle style) {
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

inline Brush *Brush::NewNull() {
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

inline Brush *Brush::DefaultBackground() {
    return NewSystem(static_cast<int>(SysColorIndex::BtnFace));
}

#endif // BXSCRIPT_IMPLEMENTATION
