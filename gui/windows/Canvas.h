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
 * @brief    Canvas
 */
#ifndef BXSCRIPT_CANVAS_H
#define BXSCRIPT_CANVAS_H

#include <string>
#include "Color.h"
#include "Font.h"
#include "Bitmap.h"
#include "Brush.h"
#include "Pen.h"
#include "Icon.h"
#include "Rect.h"

// ============================================================================
// 前置声明 (依赖的图形对象)
// ============================================================================

class Canvas {
    void *m_hdc = nullptr; // HDC
    void *m_hwnd = nullptr; // HWND
    bool m_shouldRelease = false; // 是否需要释放 DC

public:
    Canvas() = default;

    // 禁止拷贝 (HDC 是独占资源)
    Canvas(const Canvas &) = delete;

    Canvas &operator=(const Canvas &) = delete;

    // ======================== 构造与析构 ========================

    /**
     * 从窗口句柄创建画布。
     * @note 析构时会自动调用 ReleaseDC。
     */
    static Canvas *FromHwnd(void *hwnd);

    /**
     * 从现有 HDC 创建画布 (用于 WM_PAINT 或者打印)。
     * @note 析构时不会释放 HDC (由调用者管理)。
     */
    static Canvas *FromHDC(void *hdc);

    ~Canvas();

    void Dispose();

    void *GetHandle() const { return m_hdc; }

    /**
     * 绘制位图 (自动缩放)。
     * @param w, h 如果 <= 0，则使用图片原始尺寸。
     */
    Canvas *DrawBitmap(Bitmap *bmp, int x, int y, int w = 0, int h = 0);

    /**
     * 绘制图标。
     */
    bool DrawIcon(Icon *ico, int x, int y);

    /**
     * 绘制并填充矩形。
     */
    Canvas *DrawFillRect(const Rect &rect, Pen *pen, Brush *brush);

    /**
     * 仅绘制矩形边框 (内部透明)。
     */
    Canvas *DrawRect(const Rect &rect, Pen *pen);

    /**
     * 仅填充矩形 (无边框)。
     */
    Canvas *FillRect(const Rect &rect, Brush *brush);

    /**
     * 绘制椭圆 (空心)。
     */
    Canvas *DrawEllipse(const Rect &rect, Pen *pen);

    /**
     * 绘制并填充椭圆。
     */
    Canvas *DrawFillEllipse(const Rect &rect, Pen *pen, Brush *brush);

    /**
     * 画线。
     */
    Canvas *DrawLine(int x1, int y1, int x2, int y2, Pen *pen);

    /**
     * 绘制文本。
     * @param format DT_LEFT, DT_CENTER, DT_VCENTER 等 (参考 DrawTextW)。
     */
    Canvas *DrawText(const std::wstring &text, const Rect &rect, unsigned int format, Font *font, Color textColor);
};

#endif // BXSCRIPT_CANVAS_H


// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/Gdi32.h"
#include "internal/User32.h"
#include "Bitmap.h"
#include "Brush.h"
#include "Pen.h"
#include "Font.h"
#include "Icon.h"
#include "Rect.h"

// 辅助宏
#define HDC_CAST(ptr) static_cast<HDC>(ptr)
#define HWND_CAST(ptr) static_cast<HWND>(ptr)
#define HGDIOBJ_CAST(ptr) static_cast<HGDIOBJ>(ptr)

static RECT ToWinRect(const Rect &r) {
    return {static_cast<LONG>(r.Left), static_cast<LONG>(r.Top), static_cast<LONG>(r.Right), static_cast<LONG>(r.Bottom)};
}

// RAII 辅助：自动 SelectObject 和 恢复 OldObject
struct GdiSelector {
    HDC hdc;
    HGDIOBJ oldObj;

    GdiSelector(HDC _hdc, void *_obj) : hdc(_hdc) {
        if (_obj) {
            oldObj = Gdi32::W32_SelectObject(hdc, HGDIOBJ_CAST(_obj));
        } else {
            oldObj = nullptr;
        }
    }

    ~GdiSelector() {
        if (oldObj) {
            Gdi32::W32_SelectObject(hdc, oldObj);
        }
    }
};

// --- Canvas Implementation ---

inline Canvas *Canvas::FromHwnd(void *hwnd) {
    const HDC hdc = User32::W32_GetDC(HWND_CAST(hwnd));
    if (!hdc) return nullptr;

    const auto c = new Canvas();
    c->m_hdc = hdc;
    c->m_hwnd = hwnd;
    // 需要 ReleaseDC
    c->m_shouldRelease = true;
    return c;
}

inline Canvas *Canvas::FromHDC(void *hdc) {
    if (!hdc) return nullptr;
    const auto c = new Canvas();
    c->m_hdc = hdc;
    c->m_hwnd = nullptr;
    // 外部管理的 DC，不需要释放
    c->m_shouldRelease = false;
    return c;
}

inline Canvas::~Canvas() {
    Dispose();
}

inline void Canvas::Dispose() {
    if (m_hdc) {
        if (m_shouldRelease) {
            if (m_hwnd) {
                User32::W32_ReleaseDC(HWND_CAST(m_hwnd), HDC_CAST(m_hdc));
            } else {
                Gdi32::W32_DeleteDC(HDC_CAST(m_hdc));
            }
        }
        m_hdc = nullptr;
    }
}

inline Canvas *Canvas::DrawBitmap(Bitmap *bmp, int x, int y, int w, int h) {
    if (!bmp || !bmp->IsValid()) return this;

    HDC destDC = HDC_CAST(m_hdc);
    // 创建内存DC
    const HDC memDC = Gdi32::W32_CreateCompatibleDC(destDC);

    // 将 Bitmap 选入内存 DC
    HGDIOBJ oldBmp = Gdi32::W32_SelectObject(memDC, HGDIOBJ_CAST(bmp->GetHandle()));
    int ow = bmp->GetWidth();
    int oh = bmp->GetHeight();
    if (w <= 0) w = ow;
    if (h <= 0) h = oh;

    // 设置拉伸模式 (3 = COLORONCOLOR)
    int oldMode = Gdi32::W32_SetStretchBltMode(destDC, 3);
    // 绘制
    Gdi32::W32_StretchBlt(destDC, x, y, w, h, memDC, 0, 0, ow, oh, SRCCOPY);
    // 恢复状态并清理
    Gdi32::W32_SetStretchBltMode(destDC, oldMode);
    // 必须恢复旧对象才能删除 DC
    Gdi32::W32_SelectObject(memDC, oldBmp);
    Gdi32::W32_DeleteDC(memDC);

    return this;
}

inline bool Canvas::DrawIcon(Icon *ico, int x, int y) {
    if (!ico) return false;
    return ::DrawIcon((HDC) m_hdc, x, y, (HICON) ico->Handle());
}

inline Canvas *Canvas::DrawFillRect(const Rect &rect, Pen *pen, Brush *brush) {
    GdiSelector p(HDC_CAST(m_hdc), pen ? pen->GetHandle() : nullptr);
    GdiSelector b(HDC_CAST(m_hdc), brush ? brush->GetHandle() : nullptr);

    RECT rc = ToWinRect(rect);
    Gdi32::W32_Rectangle(HDC_CAST(m_hdc), rc.left, rc.top, rc.right, rc.bottom);
    return this;
}

inline Canvas *Canvas::DrawRect(const Rect &rect, Pen *pen) {
    GdiSelector p(HDC_CAST(m_hdc), pen ? pen->GetHandle() : nullptr);
    // 使用透明画刷 (NULL_BRUSH)
    GdiSelector b(HDC_CAST(m_hdc), Gdi32::W32_GetStockObject(NULL_BRUSH));
    RECT rc = ToWinRect(rect);
    Gdi32::W32_Rectangle(HDC_CAST(m_hdc), rc.left, rc.top, rc.right, rc.bottom);
    return this;
}

inline Canvas *Canvas::FillRect(const Rect &rect, Brush *brush) {
    RECT rc = ToWinRect(rect);
    User32::W32_FillRect(HDC_CAST(m_hdc), &rc, (HBRUSH) brush->GetHandle());
    return this;
}

inline Canvas *Canvas::DrawEllipse(const Rect &rect, Pen *pen) {
    GdiSelector p(HDC_CAST(m_hdc), pen ? pen->GetHandle() : nullptr);
    // 透明填充
    GdiSelector b(HDC_CAST(m_hdc), Gdi32::W32_GetStockObject(NULL_BRUSH));
    RECT rc = ToWinRect(rect);
    Gdi32::W32_Ellipse(HDC_CAST(m_hdc), rc.left, rc.top, rc.right, rc.bottom);
    return this;
}

inline Canvas *Canvas::DrawFillEllipse(const Rect &rect, Pen *pen, Brush *brush) {
    GdiSelector p(HDC_CAST(m_hdc), pen ? pen->GetHandle() : nullptr);
    GdiSelector b(HDC_CAST(m_hdc), brush ? brush->GetHandle() : nullptr);
    RECT rc = ToWinRect(rect);
    Gdi32::W32_Ellipse(HDC_CAST(m_hdc), rc.left, rc.top, rc.right, rc.bottom);
    return this;
}

inline Canvas *Canvas::DrawLine(int x1, int y1, int x2, int y2, Pen *pen) {
    GdiSelector p(HDC_CAST(m_hdc), pen ? pen->GetHandle() : nullptr);
    Gdi32::W32_MoveTo(HDC_CAST(m_hdc), x1, y1, nullptr);
    Gdi32::W32_LineTo(HDC_CAST(m_hdc), x2, y2);
    return this;
}

inline Canvas *Canvas::DrawText(const std::wstring &text, const Rect &rect, unsigned int format, Font *font, Color textColor) {
    const HDC hdc = HDC_CAST(m_hdc);
    // 1. 选入字体
    GdiSelector f(hdc, font ? font->GetHFONT() : nullptr);
    // 2. 设置背景模式 (透明)
    int oldBkMode = Gdi32::W32_SetBkMode(hdc, TRANSPARENT);
    // 3. 设置文字颜色
    COLORREF oldColor = Gdi32::W32_SetTextColor(hdc, static_cast<COLORREF>(textColor.Value()));
    // 4. 绘制
    RECT rc = ToWinRect(rect);
    User32::W32_DrawText(hdc, text.c_str(), static_cast<int>(text.length()), &rc, format);
    // 5. 恢复状态 (字体由 GdiSelector 自动恢复)
    Gdi32::W32_SetTextColor(hdc, oldColor);
    Gdi32::W32_SetBkMode(hdc, oldBkMode);
    return this;
}

#endif // BXSCRIPT_IMPLEMENTATION
