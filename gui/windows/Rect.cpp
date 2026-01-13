/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/13
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Rect
 */
#include "Rect.h"
#include <windows.h>
#include "internal/User32.h"

Rect *Rect::NewEmptyRect() {
    auto newRect = new Rect();
    User32::W32_SetRect((LPRECT) newRect, 0, 0, 0, 0);
    return newRect;
}

Rect *Rect::NewRect(int left, int top, int right, int bottom) {
    auto newRect = new Rect();
    newRect->Set(left, top, right, bottom);
    return newRect;
}

void Rect::Data(int32_t &left, int32_t &top, int32_t &right, int32_t &bottom) const {
    left = Left;
    top = Top;
    right = Right;
    bottom = Bottom;
}

int Rect::Width() const {
    return Right - Left;
}

int Rect::Height() const {
    return Bottom - Top;
}

void *Rect::GetW32Rect() {
    return this;
}

void Rect::Set(int left, int top, int right, int bottom) {
    User32::W32_SetRect(reinterpret_cast<LPRECT>(this), left, top, right, bottom);
}

bool Rect::IsEqual(Rect *rect) {
    return User32::W32_EqualRect(reinterpret_cast<LPRECT>(this), reinterpret_cast<LPRECT>(rect)) != 0;
}

void Rect::Inflate(int x, int y) {
    ::InflateRect(reinterpret_cast<LPRECT>(this), x, y);
}

void Rect::Intersect(Rect *src) {
    User32::W32_IntersectRect(reinterpret_cast<LPRECT>(this), reinterpret_cast<LPRECT>(this), reinterpret_cast<LPRECT>(src));
}

bool Rect::IsEmpty() {
    return User32::W32_IsRectEmpty(reinterpret_cast<LPRECT>(this)) != 0;
}

void Rect::Offset(int x, int y) {
    ::OffsetRect(reinterpret_cast<LPRECT>(this), x, y);
}

bool Rect::IsPointIn(int x, int y) {
    POINT pt = {static_cast<LONG>(x), static_cast<LONG>(y)};
    return User32::W32_PtInRect(reinterpret_cast<LPRECT>(this), pt) != 0;
}

void Rect::SubsTract(Rect *src) {
    User32::W32_SubtractRect(reinterpret_cast<LPRECT>(this), reinterpret_cast<const RECT *>(this), reinterpret_cast<const RECT *>(src));
}

void Rect::Union(Rect *src) {
    User32::W32_UnionRect(reinterpret_cast<LPRECT>(this), reinterpret_cast<const RECT *>(this), reinterpret_cast<const RECT *>(src));
}