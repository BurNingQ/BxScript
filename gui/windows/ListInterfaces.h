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
 * @brief    ListInterfaces
 */
#ifndef BXSCRIPT_LIST_INTERFACES_H
#define BXSCRIPT_LIST_INTERFACES_H

#include <vector>
#include <string>

class ListItem {
public:
    virtual ~ListItem() = default;
    virtual std::vector<std::wstring> Text() = 0;
    virtual int ImageIndex() = 0;
};

class ListItemChecker {
public:
    virtual ~ListItemChecker() = default;
    virtual bool Checked() = 0;
    virtual void SetChecked(bool checked) = 0;
};

class ListItemSetter {
public:
    virtual ~ListItemSetter() = default;
    virtual void SetText(const std::wstring &s) = 0;
};

#endif // BXSCRIPT_LIST_INTERFACES_H