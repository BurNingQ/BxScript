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
 * @brief    Font
 */
#ifndef BXSCRIPT_FONT_H
#define BXSCRIPT_FONT_H

#include <string>
#include <cstdint>

constexpr uint8_t FontBold = 0x01;
constexpr uint8_t FontItalic = 0x02;
constexpr uint8_t FontUnderline = 0x04;
constexpr uint8_t FontStrikeOut = 0x08;

class Font {
    // w32.HFONT
    void *m_hfont = nullptr;
    std::wstring family;
    int pointSize;
    uint8_t style;

public:

    Font(const std::wstring &family, int pointSize, uint8_t style);

    ~Font();

    void *GetHFONT() const { return m_hfont; }
    bool Bold() const { return (style & FontBold) > 0; }

    void Dispose();

    std::wstring Family() const { return family; }
    uint8_t Style() const { return style; }
    int PointSize() const { return pointSize; }
    bool Italic() const { return (style & FontItalic) > 0; }
    bool StrikeOut() const { return (style & FontStrikeOut) > 0; }
    bool Underline() const { return (style & FontUnderline) > 0; }

private:
    void *createForDPI(int dpi) const;
};

#endif
