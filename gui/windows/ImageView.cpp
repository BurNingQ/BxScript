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
 * @brief    ImageView
 */
#include "ImageView.h"
#include <windows.h>
#include "internal/User32.h"
#include "Canvas.h"
#include "GlobalVars.h"

ImageView *ImageView::New(Controller *parent) {
    const auto iv = new ImageView();
    RegClassOnlyOnce(L"BxScriptImageView");
    iv->InitWindow(L"BxScriptImageView", parent, WS_EX_CONTROLPARENT, WS_CHILD | WS_VISIBLE);
    iv->SetFont(DefaultFont);
    iv->SetText(L"");
    iv->SetSize(0, 0);
    return iv;
}

bool ImageView::DrawImageFile(const std::wstring &filepath) {
    // 注意：RGB(255, 255, 0) 是黄色
    Bitmap *bmp = Bitmap::FromFile(filepath, Color::FromRGB(255, 255, 0));
    if (!bmp) {
        return false;
    }
    m_bmp = bmp;
    User32::W32_PostMessage(static_cast<HWND>(m_hwnd), WM_PAINT, 0, 0);
    return true;
}

bool ImageView::DrawImageUrl(const std::wstring &url) {
    Bitmap *bmp = Bitmap::FromUrl(url);
    if (!bmp) {
        return false;
    }
    m_bmp = bmp;
    User32::W32_PostMessage(static_cast<HWND>(m_hwnd), WM_PAINT, 0, 0);
    return true;
}

void ImageView::DrawImage(Bitmap *bmp) {
    m_bmp = bmp;
    User32::W32_PostMessage(static_cast<HWND>(m_hwnd), WM_PAINT, 0, 0);
}

uintptr_t ImageView::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    const auto hwnd = static_cast<HWND>(m_hwnd);
    switch (msg) {
        case WM_SIZE:
        case WM_SIZING:
            this->Invalidate(true);
            break;

        case WM_ERASEBKGND:
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            const auto hdc = BeginPaint(hwnd, &ps);
            if (m_bmp != nullptr) {
                Canvas *canvas = Canvas::FromHDC(hdc);
                RECT rc;
                GetClientRect(hwnd, &rc);
                canvas->DrawBitmap(m_bmp, 0, 0, rc.right, rc.bottom);
                delete canvas;
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        default: ;
    }

    return User32::W32_DefWindowProc(static_cast<HWND>(m_hwnd), msg, wparam, lparam);
}
