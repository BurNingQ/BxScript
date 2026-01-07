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

const uint8_t FontBold = 0x01;
const uint8_t FontItalic = 0x02;
const uint8_t FontUnderline = 0x04;
const uint8_t FontStrikeOut = 0x08;

class Font {
    // w32.HFONT
    void *m_hfont = nullptr;
    std::wstring m_family;
    int m_pointSize;
    uint8_t m_style;

public:

    Font(const std::wstring &family, int pointSize, uint8_t style);

    ~Font();

    void *GetHFONT() const { return m_hfont; }
    bool Bold() const { return (m_style & FontBold) > 0; }

    void Dispose();

    std::wstring Family() const { return m_family; }
    bool Italic() const { return (m_style & FontItalic) > 0; }
    bool StrikeOut() const { return (m_style & FontStrikeOut) > 0; }
    bool Underline() const { return (m_style & FontUnderline) > 0; }
    uint8_t Style() const { return m_style; }

private:
    void *createForDPI(int dpi) const;
};

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/Gdi32.h"
#include "internal/User32.h"

inline Font::Font(const std::wstring &family, int pointSize, uint8_t style)
    : m_family(family), m_pointSize(pointSize), m_style(style) {
    if (style > (FontBold | FontItalic | FontUnderline | FontStrikeOut)) {
        exit(1);
    }

    HDC hDC = User32::W32_GetDC(nullptr);
    int screenDPIY = Gdi32::W32_GetDeviceCaps(hDC, LOGPIXELSY);
    User32::W32_ReleaseDC(nullptr, hDC);

    m_hfont = createForDPI(screenDPIY);
    if (!m_hfont) {
        exit(1);
    }
}

inline Font::~Font() {
    Dispose();
}

inline void *Font::createForDPI(int dpi) const {
    LOGFONTW lf = {0};

    lf.lfHeight = -MulDiv(m_pointSize, dpi, 72);
    if (m_style & FontBold) {
        lf.lfWeight = FW_BOLD;
    } else {
        lf.lfWeight = FW_NORMAL;
    }

    if (m_style & FontItalic) lf.lfItalic = 1;
    if (m_style & FontUnderline) lf.lfUnderline = 1;
    if (m_style & FontStrikeOut) lf.lfStrikeOut = 1;

    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_TT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = CLEARTYPE_QUALITY;
    lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;

    wcsncpy(lf.lfFaceName, m_family.c_str(), LF_FACESIZE);

    return Gdi32::W32_CreateFontIndirect(lf);
}

inline void Font::Dispose() {
    if (m_hfont) {
        Gdi32::W32_DeleteObject(static_cast<HGDIOBJ>(m_hfont));
        m_hfont = nullptr;
    }
}

#endif