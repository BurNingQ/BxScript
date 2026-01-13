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

    void Data(int32_t &left, int32_t &top, int32_t &right, int32_t &bottom) const;

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
