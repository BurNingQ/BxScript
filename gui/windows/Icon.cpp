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
 * @brief    Icon
 */
#include "Icon.h"
#include <windows.h>
#include "internal/User32.h"
#include "internal/Shell32.h"

inline Icon *Icon::NewIconFromFile(const std::wstring &path) {
    const auto ico = new Icon();
    ico->m_handle = static_cast<void *>(::LoadIconW(nullptr, path.c_str()));
    if (ico->m_handle == nullptr) {
        delete ico;
        return nullptr;
    }
    return ico;
}

inline const Icon *Icon::NewIconFromResource(void *instance, int resId) {
    const auto *ico = new Icon();
    ico->m_handle = static_cast<void *>(LoadIconW(static_cast<HINSTANCE>(instance), MAKEINTRESOURCEW(resId)));
    if (ico->m_handle == nullptr) {
        delete ico;
        return nullptr;
    }
    return ico;
}

inline Icon const *Icon::ExtractIconX(const std::wstring &fileName, int index) {
    auto const *ico = new Icon();
    // 底层调用 Shell32 的 ExtractIconW，hInst 传 0 是因为 Go 封装里写死传 0
    ico->m_handle = static_cast<void *>(Shell32::W32_ExtractIcon(nullptr, fileName.c_str(), (UINT) index));
    // Win32 ExtractIcon 失败返回 0, 1 或 其它特殊值
    if (ico->m_handle == nullptr || ico->m_handle == reinterpret_cast<void *>(1)) {
        delete ico;
        return nullptr;
    }
    return ico;
}

inline bool Icon::Destroy() const {
    if (m_handle) {
        const BOOL res = User32::W32_DestroyIcon(static_cast<HICON>(m_handle));
        if (res) m_handle = nullptr;
        return res != 0;
    }
    return false;
}
