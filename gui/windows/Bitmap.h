/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/4
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Bitmap
 */
#ifndef BXSCRIPT_BITMAP_H
#define BXSCRIPT_BITMAP_H

#include <string>
#include <vector>
#include <cstdint>

class Bitmap;

struct BitmapColor {
    uint8_t R, G, B;
    static BitmapColor White() { return {255, 255, 255}; }
};

class Bitmap {
    void* m_hBitmap = nullptr;
    int m_width = 0;
    int m_height = 0;

public:
    Bitmap() = default;
    Bitmap(const Bitmap&) = delete;
    Bitmap& operator=(const Bitmap&) = delete;

    // 允许移动
    Bitmap(Bitmap&& other) noexcept;
    Bitmap& operator=(Bitmap&& other) noexcept;

    ~Bitmap();

    // ======================== 工厂方法 ========================

    /**
     * 从本地文件加载 (支持 PNG, JPG, BMP 等)。
     * @param path
     * @param bg 转换透明通道时的背景色 (默认白色)。
     */
    static Bitmap* FromFile(const std::wstring& path, BitmapColor bg = BitmapColor::White());

    /**
     * 从内存数据加载。
     */
    static Bitmap* FromMemory(const void* data, size_t size, BitmapColor bg = BitmapColor::White());

    /**
     * 从 URL 下载并加载 (阻塞式)。
     * @note 依赖 WinINet。
     */
    static Bitmap* FromUrl(const std::wstring& url, BitmapColor bg = BitmapColor::White());

    // ======================== 属性 ========================

    void Dispose();
    void* GetHandle() const { return m_hBitmap; } // 返回 HBITMAP
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    void GetSize(int& w, int& h) const { w = m_width; h = m_height; }

    // 检查位图是否有效
    bool IsValid() const { return m_hBitmap != nullptr; }
};

#endif // BXSCRIPT_BITMAP_H


// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include <wininet.h> // HTTP 下载需要
#include <gdiplus.h>

#include "internal/Gdi32.h"
#include "internal/GdiPlus.h"
#include "internal/Shlwapi.h"

// 链接 WinINet 库 (如果 CMake 没配)
#pragma comment(lib, "wininet.lib")

// 辅助：从 GDI+ Bitmap 提取 HBITMAP 并封装
static Bitmap* AssembleBitmapFromGDIPlus(Gdiplus::GpBitmap* gpBitmap, BitmapColor bg) {
    if (!gpBitmap) return nullptr;

    HBITMAP hBitmap = nullptr;
    // GDI+ 里的 Color 是 ARGB，GDI 是 RGB。注意顺序。
    // Gdiplus::Color(A, R, G, B)
    Gdiplus::ARGB backColor = Gdiplus::Color::MakeARGB(255, bg.R, bg.G, bg.B);

    GdiPlus::W32_GdipCreateHBITMAPFromBitmap(gpBitmap, &hBitmap, backColor);

    // 我们已经拿到了 HBITMAP，GDI+ 的对象可以释放了
    GdiPlus::W32_GdipDisposeImage(gpBitmap);

    if (!hBitmap) return nullptr;

    // 获取尺寸信息
    BITMAP bm;
    if (Gdi32::W32_GetObject(hBitmap, sizeof(BITMAP), &bm) == 0) {
        Gdi32::W32_DeleteObject(hBitmap);
        return nullptr;
    }

    Bitmap* wrapper = new Bitmap();
    wrapper->m_hBitmap = hBitmap;
    wrapper->m_width = bm.bmWidth;
    wrapper->m_height = bm.bmHeight;
    return wrapper;
}

// --- Implementation of Bitmap ---

Bitmap::Bitmap(Bitmap&& other) noexcept {
    m_hBitmap = other.m_hBitmap;
    m_width = other.m_width;
    m_height = other.m_height;
    other.m_hBitmap = nullptr;
    other.m_width = 0;
    other.m_height = 0;
}

Bitmap& Bitmap::operator=(Bitmap&& other) noexcept {
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
        Gdi32::W32_DeleteObject(static_cast<HGDIOBJ>(m_hBitmap));
        m_hBitmap = nullptr;
    }
}

Bitmap* Bitmap::FromFile(const std::wstring& path, BitmapColor bg) {
    Gdiplus::GpBitmap* gpBitmap = nullptr;
    auto status = GdiPlus::W32_GdipCreateBitmapFromFile(path.c_str(), &gpBitmap);

    if (status != Gdiplus::Ok || !gpBitmap) {
        return nullptr;
    }

    return AssembleBitmapFromGDIPlus(gpBitmap, bg);
}

Bitmap* Bitmap::FromMemory(const void* data, size_t size, BitmapColor bg) {
    if (!data || size == 0) return nullptr;

    // 1. 创建内存流 (Shlwapi)
    IStream* pStream = Shlwapi::W32_SHCreateMemStream((const BYTE*)data, (UINT)size);
    if (!pStream) return nullptr;

    // 2. GDI+ 加载
    Gdiplus::GpBitmap* gpBitmap = nullptr;
    auto status = GdiPlus::W32_GdipCreateBitmapFromStream(pStream, &gpBitmap);

    pStream->Release(); // GDI+ 加载完后，释放流

    if (status != Gdiplus::Ok) return nullptr;

    return AssembleBitmapFromGDIPlus(gpBitmap, bg);
}

Bitmap* Bitmap::FromUrl(const std::wstring& url, BitmapColor bg) {
    // 简单的 WinINet 下载实现
    HINTERNET hNet = InternetOpenW(L"BxScript/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hNet) return nullptr;

    HINTERNET hFile = InternetOpenUrlW(hNet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
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
            break; // Error
        }
    } while (bytesRead > 0);

    InternetCloseHandle(hFile);
    InternetCloseHandle(hNet);

    if (buffer.empty()) return nullptr;

    return FromMemory(buffer.data(), buffer.size(), bg);
}

#endif // BXSCRIPT_IMPLEMENTATION