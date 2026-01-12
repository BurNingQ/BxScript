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

    void SubsTract(Rect *src);

    void Union(Rect *src);
};

#endif // BXSCRIPT_RECT_H

// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION
#ifndef BXSCRIPT_RECT_IMPL
#define BXSCRIPT_RECT_IMPL

#include <windows.h>
#include "internal/User32.h"

inline Rect *Rect::NewEmptyRect() {
    auto newRect = new Rect();
    User32::W32_SetRect((LPRECT) newRect, 0, 0, 0, 0);
    return newRect;
}

inline Rect *Rect::NewRect(int left, int top, int right, int bottom) {
    auto newRect = new Rect();
    newRect->Set(left, top, right, bottom);
    return newRect;
}

inline void Rect::Data(int32_t &left, int32_t &top, int32_t &right, int32_t &bottom) {
    left = Left;
    top = Top;
    right = Right;
    bottom = Bottom;
}

inline int Rect::Width() const {
    return Right - Left;
}

inline int Rect::Height() const {
    return Bottom - Top;
}

inline void *Rect::GetW32Rect() {
    return static_cast<void *>(this);
}

inline void Rect::Set(int left, int top, int right, int bottom) {
    User32::W32_SetRect(reinterpret_cast<LPRECT>(this), left, top, right, bottom);
}

inline bool Rect::IsEqual(Rect *rect) {
    return User32::W32_EqualRect(reinterpret_cast<LPRECT>(this), reinterpret_cast<LPRECT>(rect)) != 0;
}

inline void Rect::Inflate(int x, int y) {
    ::InflateRect(reinterpret_cast<LPRECT>(this), x, y);
}

inline void Rect::Intersect(Rect *src) {
    User32::W32_IntersectRect(reinterpret_cast<LPRECT>(this), reinterpret_cast<LPRECT>(this), reinterpret_cast<LPRECT>(src));
}

inline bool Rect::IsEmpty() {
    return User32::W32_IsRectEmpty(reinterpret_cast<LPRECT>(this)) != 0;
}

inline void Rect::Offset(int x, int y) {
    ::OffsetRect(reinterpret_cast<LPRECT>(this), x, y);
}

inline bool Rect::IsPointIn(int x, int y) {
    POINT pt = {static_cast<LONG>(x), static_cast<LONG>(y)};
    return User32::W32_PtInRect(reinterpret_cast<LPRECT>(this), pt) != 0;
}

inline void Rect::SubsTract(Rect *src) {
    User32::W32_SubtractRect(reinterpret_cast<LPRECT>(this), reinterpret_cast<const RECT *>(this), reinterpret_cast<const RECT *>(src));
}

inline void Rect::Union(Rect *src) {
    User32::W32_UnionRect(reinterpret_cast<LPRECT>(this), reinterpret_cast<const RECT *>(this), reinterpret_cast<const RECT *>(src));
}

#endif // BXSCRIPT_RECT_IMPL
#endif // BXSCRIPT_IMPLEMENTATION
