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

    // handle w32.HICON
    mutable void *m_handle = nullptr;

public:
    Icon() = default;

    ~Icon() = default;

    static Icon *NewIconFromFile(const std::wstring &path);

    static Icon *NewIconFromImageFile(const std::wstring &path);

    static Icon *NewIconFromResource(void *instance, int resId);

    static Icon const *ExtractIconX(const std::wstring &fileName, int index);

    bool Destroy() const;

    void *Handle() const { return m_handle; }
};

#endif // BXSCRIPT_ICON_H