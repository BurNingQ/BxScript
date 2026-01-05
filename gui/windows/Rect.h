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
 * @brief    Rect
 */
#ifndef BXSCRIPT_RECT_H
#define BXSCRIPT_RECT_H

#include <cstdint>

class Rect {
public:

    int32_t Left, Top, Right, Bottom;

    Rect() : Left(0), Top(0), Right(0), Bottom(0) {
    }

    Rect(int left, int top, int right, int bottom) : Left(left), Top(top), Right(right), Bottom(bottom) {
    }

    static Rect *NewEmptyRect();

    static Rect *NewRect(int left, int top, int right, int bottom);

    void Data(int32_t &left, int32_t &top, int32_t &right, int32_t &bottom);

    int Width() const;

    int Height() const;

    // 获取内部 w32.RECT 指针 (在实现部分会强转)
    void *GetW32Rect();

    void Set(int left, int top, int right, int bottom);

    bool IsEqual(Rect *rect);

    void Inflate(int x, int y);

    void Intersect(Rect *src);

    bool IsEmpty();

    void Offset(int x, int y);

    bool IsPointIn(int x, int y);

    void Substract(Rect *src);

    void Union(Rect *src);
};

#endif // BXSCRIPT_RECT_H

// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/User32.h"

Rect *Rect::NewEmptyRect() {
    Rect *newRect = new Rect();
    User32::W32_SetRect((LPRECT) newRect, 0, 0, 0, 0);
    return newRect;
}

Rect *Rect::NewRect(int left, int top, int right, int bottom) {
    Rect *newRect = new Rect();
    newRect->Set(left, top, right, bottom);
    return newRect;
}

void Rect::Data(int32_t &left, int32_t &top, int32_t &right, int32_t &bottom) {
    left = Left;
    top = Top;
    right = Right;
    bottom = Bottom;
}

int Rect::Width() const {
    return (int) (Right - Left);
}

int Rect::Height() const {
    return (int) (Bottom - Top);
}

void *Rect::GetW32Rect() {
    return static_cast<void *>(this);
}

void Rect::Set(int left, int top, int right, int bottom) {
    User32::W32_SetRect((LPRECT) this, left, top, right, bottom);
}

bool Rect::IsEqual(Rect *rect) {
    return User32::W32_EqualRect((LPRECT) this, (LPRECT) rect) != 0;
}

void Rect::Inflate(int x, int y) {
    // 调用 User32 里的 InflateRect
    ::InflateRect((LPRECT) this, x, y);
}

void Rect::Intersect(Rect *src) {
    User32::W32_IntersectRect((LPRECT) this, (LPRECT) this, (LPRECT) src);
}

bool Rect::IsEmpty() {
    return User32::W32_IsRectEmpty((LPRECT) this) != 0;
}

void Rect::Offset(int x, int y) {
    ::OffsetRect((LPRECT) this, x, y);
}

bool Rect::IsPointIn(int x, int y) {
    POINT pt = {(LONG) x, (LONG) y};
    return User32::W32_PtInRect((LPRECT) this, pt) != 0;
}

void Rect::Substract(Rect *src) {
    ::SubtractRect((LPRECT) this, (LPRECT) this, (LPRECT) src);
}

void Rect::Union(Rect *src) {
    // 调用 User32 里的 UnionRect 封装（如果之前没写，直接调用系统API）
    ::UnionRect((LPRECT) this, (LPRECT) this, (LPRECT) src);
}

#endif // BXSCRIPT_IMPLEMENTATION
