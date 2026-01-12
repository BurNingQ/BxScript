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
    Canvas *DrawTextX(const std::wstring &text, const Rect &rect, unsigned int format, Font *font, Color textColor);
};

#endif // BXSCRIPT_CANVAS_H
