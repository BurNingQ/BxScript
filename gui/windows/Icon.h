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
 * @brief    Icon
 */
#ifndef BXSCRIPT_ICON_H
#define BXSCRIPT_ICON_H

#include <string>

class Icon {

    void *m_handle = nullptr; // handle w32.HICON

public:
    Icon() = default;

    ~Icon() = default;

    // Static Factory Methods (Return nullptr on error, matching Go's error != nil)
    static Icon *NewIconFromFile(const std::wstring &path);

    static Icon *NewIconFromResource(void *instance, int resId);

    static Icon *ExtractIcon(const std::wstring &fileName, int index);

    bool Destroy();

    void *Handle() const { return m_handle; }
};

#endif // BXSCRIPT_ICON_H

// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/User32.h"
#include "internal/Shell32.h"

Icon *Icon::NewIconFromFile(const std::wstring &path) {
    Icon *ico = new Icon();
    // 对照 w32.LoadIcon(0, syscall.StringToUTF16Ptr(path))
    ico->m_handle = (void *) User32::W32_W32_LoadIcon(nullptr, path.c_str());
    if (ico->m_handle == nullptr) {
        delete ico;
        return nullptr;
    }
    return ico;
}

Icon *Icon::NewIconFromResource(void *instance, int resId) {
    Icon *ico = new Icon();
    // 对照 w32.LoadIconWithResourceID(instance, resId)
    // 注意：LoadIconWithResourceID 内部是对 LoadIconW(instance, (LPCWSTR)resId) 的调用
    ico->m_handle = (void *) LoadIconW((HINSTANCE) instance, MAKEINTRESOURCEW(resId));
    if (ico->m_handle == nullptr) {
        delete ico;
        return nullptr;
    }
    return ico;
}

Icon *Icon::ExtractIcon(const std::wstring &fileName, int index) {
    Icon *ico = new Icon();
    // 对照 w32.ExtractIcon(fileName, index)
    // 底层调用 Shell32 的 ExtractIconW，hInst 传 0 是因为 Go 封装里写死传 0
    ico->m_handle = (void *) Shell32::W32_ExtractIcon(nullptr, fileName.c_str(), (UINT) index);

    // Win32 ExtractIcon 失败返回 0, 1 或 其它特殊值
    if (ico->m_handle == nullptr || ico->m_handle == (void *) 1) {
        delete ico;
        return nullptr;
    }
    return ico;
}

bool Icon::Destroy() {
    if (m_handle) {
        // 对照 w32.DestroyIcon(ic.handle)
        BOOL res = User32::W32_DestroyIcon((HICON) m_handle);
        if (res) m_handle = nullptr;
        return res != 0;
    }
    return false;
}

#endif // BXSCRIPT_IMPLEMENTATION