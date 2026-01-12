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
 * @brief    Pen
 */
#ifndef BXSCRIPT_PEN_H
#define BXSCRIPT_PEN_H

class Brush;

class Pen {

    void *m_hPen = nullptr;
    unsigned int m_style = 0;
    Brush *m_brush = nullptr;

public:
    Pen() = default;

    ~Pen();

    static Pen *New(unsigned int style, unsigned int width, Brush *brush);

    static Pen *NewNull();

    unsigned int Style() const { return m_style; }
    Brush *GetBrush() const { return m_brush; }
    void *GetHandle() const { return m_hPen; }

    void Dispose();
};

#endif // BXSCRIPT_PEN_H
