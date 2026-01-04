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

#include <cstdint>
#include <utility> // for std::exchange

// 简单的颜色定义，对应 COLORREF (0x00BBGGRR)
using BrushColor = uint32_t;

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
    // ... 其他常用索引
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
    static Brush *NewSolid(BrushColor color);

    /**
     * 创建系统颜色画刷 (如按钮面颜色)。
     */
    static Brush *NewSystem(int colorIndex);

    static Brush *NewSystem(SysColorIndex index);

    /**
     * 创建阴影线画刷 (网格、斜线等)。
     */
    static Brush *NewHatched(BrushColor color, HatchStyle style);

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

// 内部构造函数 (不暴露给外部)
static Brush *CreateBrushWrapper(HBRUSH hBrush) {
    if (!hBrush) return nullptr;
    Brush *b = new Brush();
    // 这是一个 hack 方式访问私有成员，或者你可以把构造函数设为 public 但不建议直接用
    // 为了保持单头文件简洁，这里利用友元或者直接内存操作，
    // 但最标准的是在类里加个 private constructor 并且 friend 这个工厂函数。
    // 这里简单起见，假设类定义里加个 SetHandle 或者 friend。
    // 为了不修改上面声明太复杂，我们直接 memcpy 进去或者改用 public 构造。
    // 修正：我们在下面直接操作成员，因为我们在同一个文件里实现。

    // 实际上，因为是同一个文件，我们不能直接访问私有成员除非是成员函数。
    // 让我们把上面的 new 改成 new + 赋值，但 m_hBrush 是私有的。
    // 最简单的办法：实现代码写成成员函数。
    return nullptr; // 占位，下面的工厂方法是静态成员，可以访问私有变量
}

Brush::~Brush() {
    Dispose();
}

void Brush::Dispose() {
    if (m_hBrush) {
        Gdi32::W32_DeleteObject(HBRUSH_CAST(m_hBrush));
        m_hBrush = nullptr;
    }
}

Brush *Brush::NewSolid(BrushColor color) {
    // Go代码用的是 CreateBrushIndirect，但 CreateSolidBrush 等价且更简单
    HBRUSH h = Gdi32::W32_CreateSolidBrush(static_cast<COLORREF>(color));
    if (!h) return nullptr;

    Brush *b = new Brush();
    b->m_hBrush = h;
    return b;
}

Brush *Brush::NewSystem(int colorIndex) {
    // GetSysColorBrush 是 User32 的函数
    HBRUSH h = GetSysColorBrush(colorIndex);
    if (!h) return nullptr;

    Brush *b = new Brush();
    b->m_hBrush = h;
    return b;
}

Brush *Brush::NewSystem(SysColorIndex index) {
    return NewSystem(static_cast<int>(index));
}

Brush *Brush::NewHatched(BrushColor color, HatchStyle style) {
    LOGBRUSH lb;
    lb.lbStyle = BS_HATCHED;
    lb.lbColor = static_cast<COLORREF>(color);
    lb.lbHatch = static_cast<ULONG_PTR>(style);

    HBRUSH h = Gdi32::W32_CreateBrushIndirect(&lb);
    if (!h) return nullptr;

    Brush *b = new Brush();
    b->m_hBrush = h;
    return b;
}

Brush *Brush::NewNull() {
    LOGBRUSH lb;
    lb.lbStyle = BS_NULL;
    lb.lbColor = 0;
    lb.lbHatch = 0;

    HBRUSH h = Gdi32::W32_CreateBrushIndirect(&lb);
    // 或者直接用 GetStockObject(NULL_BRUSH) 也可以，但 CreateBrushIndirect 更符合 Go 原逻辑
    if (!h) return nullptr;

    Brush *b = new Brush();
    b->m_hBrush = h;
    return b;
}

Brush *Brush::DefaultBackground() {
    return NewSystem(static_cast<int>(SysColorIndex::BtnFace));
}

#endif // BXSCRIPT_IMPLEMENTATION
