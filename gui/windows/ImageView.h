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
 * @brief    ImageView
 */
#ifndef BXSCRIPT_IMAGEVIEW_H
#define BXSCRIPT_IMAGEVIEW_H

#include "ControlBase.h"
#include "Bitmap.h"
#include "Color.h"
#include "Utils.h"

class ImageView : public ControlBase {

    Bitmap *m_bmp = nullptr; // bmp *Bitmap

public:
    ImageView() = default;

    virtual ~ImageView() = default;

    // Static Factory Method
    static ImageView *New(Controller *parent);

    // Image loading methods
    bool DrawImageFile(const std::wstring &filepath);

    bool DrawImageUrl(const std::wstring &url);

    void DrawImage(Bitmap *bmp);

    // Override WndProc from ControlBase
    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif // BXSCRIPT_IMAGEVIEW_H

// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "WindowRegistry.h"
#include "internal/User32.h"
#include "Canvas.h"
#include "GlobalVars.h"

ImageView *ImageView::New(Controller *parent) {
    ImageView *iv = new ImageView();

    RegClassOnlyOnce(L"BxScriptImageView");

    iv->InitWindow(L"BxScriptImageView", parent, WS_EX_CONTROLPARENT, WS_CHILD | WS_VISIBLE);

    // WindowRegistry::Register 已经在 InitWindow 内部完成

    iv->SetFont(DefaultFont);

    iv->SetText(L"");

    iv->SetSize(0, 0);

    return iv;
}

bool ImageView::DrawImageFile(const std::wstring &filepath) {
    // 注意：RGB(255, 255, 0) 是黄色
    Bitmap *bmp = Bitmap::FromFile(filepath, Color::RGB(255, 255, 0));
    if (!bmp) {
        return false;
    }
    m_bmp = bmp;

    User32::W32_PostMessage((HWND) m_hwnd, WM_PAINT, 0, 0);
    return true;
}

bool ImageView::DrawImageUrl(const std::wstring &url) {
    Bitmap *bmp = Bitmap::FromUrl(url);
    if (!bmp) {
        return false;
    }
    m_bmp = bmp;

    User32::W32_PostMessage((HWND) m_hwnd, WM_PAINT, 0, 0);
    return true;
}

void ImageView::DrawImage(Bitmap *bmp) {
    m_bmp = bmp;
    User32::W32_PostMessage((HWND) m_hwnd, WM_PAINT, 0, 0);
}

uintptr_t ImageView::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_SIZE:
        case WM_SIZING:
            this->Invalidate(true);
            break;

        case WM_ERASEBKGND:
            // 对照 Go: return 1 // important
            return 1;

        case WM_PAINT:
            if (m_bmp != nullptr) {
                Canvas *canvas = Canvas::FromHwnd(m_hwnd);

                if (this->Width() <= 0 || this->Height() <= 0) {
                    this->SetSize(m_bmp->GetWidth(), m_bmp->GetHeight());
                }

                canvas->DrawBitmap(m_bmp, 0, 0, this->Width(), this->Height());

                delete canvas;
            }
            break;
    }

    return User32::W32_DefWindowProc((HWND) m_hwnd, msg, wparam, lparam);
}

#endif // BXSCRIPT_IMPLEMENTATION
