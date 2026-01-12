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
#include "Color.h"

class Bitmap {
public:
    void *m_hBitmap = nullptr;
    int m_width = 0;
    int m_height = 0;

    Bitmap() = default;

    Bitmap(const Bitmap &) = delete;

    Bitmap &operator=(const Bitmap &) = delete;

    // 允许移动
    Bitmap(Bitmap &&other) noexcept;

    Bitmap &operator=(Bitmap &&other) noexcept;

    ~Bitmap();

    // ======================== 工厂方法 ========================

    /**
     * 从本地文件加载 (支持 PNG, JPG, BMP 等)。
     * @param path
     * @param bg 转换透明通道时的背景色 (默认白色)。
     */
    static Bitmap *FromFile(const std::wstring &path, Color bg = Color::White());

    /**
     * 从内存数据加载。
     */
    static Bitmap *FromMemory(const void *data, size_t size, Color bg = Color::White());

    /**
     * 从 URL 下载并加载 (阻塞式)。
     * @note 依赖 WinINet。
     */
    static Bitmap *FromUrl(const std::wstring &url, Color bg = Color::White());

    // ======================== 属性 ========================

    void Dispose();

    void *GetHandle() const { return m_hBitmap; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    void GetSize(int &w, int &h) const {
        w = m_width;
        h = m_height;
    }

    bool IsValid() const { return m_hBitmap != nullptr; }
};

#endif // BXSCRIPT_BITMAP_H
