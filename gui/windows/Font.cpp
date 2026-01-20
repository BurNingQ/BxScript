/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/12
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Font
 */
#include <windows.h>
#include "internal/Gdi32.h"
#include "internal/User32.h"

Font::Font(const std::wstring &family, int pointSize, uint8_t style)
    : family(family), pointSize(pointSize), style(style) {
    if (style > (FontBold | FontItalic | FontUnderline | FontStrikeOut)) {
        exit(1);
    }

    const HDC hDC = User32::W32_GetDC(nullptr);
    int screenDPIY = Gdi32::W32_GetDeviceCaps(hDC, LOGPIXELSY);
    User32::W32_ReleaseDC(nullptr, hDC);

    m_hfont = createForDPI(screenDPIY);
    if (!m_hfont) {
        exit(1);
    }
}

Font::~Font() {
    Dispose();
}

void *Font::createForDPI(int dpi) const {
    LOGFONTW lf = {0};

    lf.lfHeight = -MulDiv(pointSize, dpi, 72);
    if (style & FontBold) {
        lf.lfWeight = FW_BOLD;
    } else {
        lf.lfWeight = FW_NORMAL;
    }

    if (style & FontItalic) lf.lfItalic = 1;
    if (style & FontUnderline) lf.lfUnderline = 1;
    if (style & FontStrikeOut) lf.lfStrikeOut = 1;

    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_TT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = CLEARTYPE_QUALITY;
    lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;

    wcsncpy(lf.lfFaceName, family.c_str(), LF_FACESIZE);

    return Gdi32::W32_CreateFontIndirect(lf);
}

void Font::Dispose() {
    if (m_hfont) {
        Gdi32::W32_DeleteObject(m_hfont);
        m_hfont = nullptr;
    }
}
