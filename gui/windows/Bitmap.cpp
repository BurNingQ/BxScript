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
 * @brief    Bitmap
 */
#include <windows.h>
#include <wininet.h>
#include "Bitmap.h"

#include <vector>

#include "internal/Gdi32.h"
#include "internal/GdiPlus.h"
#include "internal/Shlwapi.h"

// GDI+ Bitmap 提取 HBITMAP 并封装
static Bitmap *AssembleBitmapFromGDIPlus(Gdiplus::GpBitmap *gpBitmap, Color bg) {
    if (!gpBitmap) return nullptr;
    HBITMAP hBitmap = nullptr;
    // GDI+ 里的 Color 是 ARGB，GDI 是 RGB。注意顺序。
    // Gdiplus::Color(A, R, G, B)
    const Gdiplus::ARGB backColor = Gdiplus::Color::MakeARGB(255, bg.R(), bg.G(), bg.B());
    GdiPlus::W32_GdipCreateHBITMAPFromBitmap(gpBitmap, &hBitmap, backColor);
    GdiPlus::W32_GdipDisposeImage(gpBitmap);
    if (!hBitmap) return nullptr;
    // 获取尺寸信息
    BITMAP bm;
    if (Gdi32::W32_GetObject(hBitmap, sizeof(BITMAP), &bm) == 0) {
        Gdi32::W32_DeleteObject(hBitmap);
        return nullptr;
    }
    auto *wrapper = new Bitmap();
    wrapper->m_hBitmap = hBitmap;
    wrapper->m_width = bm.bmWidth;
    wrapper->m_height = bm.bmHeight;
    return wrapper;
}

Bitmap::Bitmap(Bitmap &&other) noexcept {
    m_hBitmap = other.m_hBitmap;
    m_width = other.m_width;
    m_height = other.m_height;
    other.m_hBitmap = nullptr;
    other.m_width = 0;
    other.m_height = 0;
}

Bitmap &Bitmap::operator=(Bitmap &&other) noexcept {
    if (this != &other) {
        Dispose();
        m_hBitmap = other.m_hBitmap;
        m_width = other.m_width;
        m_height = other.m_height;
        other.m_hBitmap = nullptr;
        other.m_width = 0;
        other.m_height = 0;
    }
    return *this;
}

Bitmap::~Bitmap() {
    Dispose();
}

void Bitmap::Dispose() {
    if (m_hBitmap) {
        Gdi32::W32_DeleteObject(m_hBitmap);
        m_hBitmap = nullptr;
    }
}

Bitmap *Bitmap::FromFile(const std::wstring &path, const Color bg) {
    Gdiplus::GpBitmap *gpBitmap = nullptr;
    auto status = GdiPlus::W32_GdipCreateBitmapFromFile(path.c_str(), &gpBitmap);
    if (status != Gdiplus::Ok || !gpBitmap) {
        return nullptr;
    }
    return AssembleBitmapFromGDIPlus(gpBitmap, bg);
}

Bitmap *Bitmap::FromMemory(const void *data, const size_t size, const Color bg) {
    if (!data || size == 0) return nullptr;
    // 1. 创建内存流 (Shlwapi)
    IStream *pStream = Shlwapi::W32_SHCreateMemStream(static_cast<const BYTE *>(data), static_cast<UINT>(size));
    if (!pStream) return nullptr;
    // 2. GDI+ 加载
    Gdiplus::GpBitmap *gpBitmap = nullptr;
    auto status = GdiPlus::W32_GdipCreateBitmapFromStream(pStream, &gpBitmap);
    // GDI+ 加载完后，释放流
    pStream->Release();
    if (status != Gdiplus::Ok) return nullptr;
    return AssembleBitmapFromGDIPlus(gpBitmap, bg);
}

Bitmap *Bitmap::FromUrl(const std::wstring &url, const Color bg) {
    const HINTERNET hNet = InternetOpenW(L"BxScript/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hNet) return nullptr;
    const HINTERNET hFile = InternetOpenUrlW(hNet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hFile) {
        InternetCloseHandle(hNet);
        return nullptr;
    }
    std::vector<BYTE> buffer;
    DWORD bytesRead = 0;
    BYTE temp[4096];
    do {
        if (InternetReadFile(hFile, temp, sizeof(temp), &bytesRead)) {
            if (bytesRead > 0) {
                buffer.insert(buffer.end(), temp, temp + bytesRead);
            }
        } else {
            break;
        }
    } while (bytesRead > 0);
    InternetCloseHandle(hFile);
    InternetCloseHandle(hNet);
    if (buffer.empty()) return nullptr;
    return FromMemory(buffer.data(), buffer.size(), bg);
}
