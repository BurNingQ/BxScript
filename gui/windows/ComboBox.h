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
 * @brief    ComboBox
 */
#ifndef BXSCRIPT_COMBOBOX_H
#define BXSCRIPT_COMBOBOX_H

#include "ControlBase.h"

class ComboBox : public ControlBase {
    int m_itemCount = 0;

public:
    // 事件回调：当用户更改了下拉框的选择时触发
    EventManager OnSelectedChange;

    ComboBox() = default;

    ~ComboBox() override = default;

    /**
     * 工厂方法：创建一个下拉列表样式的组合框。
     */
    static ComboBox *Create(ControlBase *parent, int x = 0, int y = 0, int w = 200, int h = 150);

    /**
     * 清空所有项。
     */
    bool DeleteAllItems();

    /**
     * 在指定索引处插入项。
     */
    bool InsertItem(int index, const std::wstring &str);

    /**
     * 在末尾追加项。
     */
    bool AddItem(const std::wstring &str);

    /**
     * 删除指定索引的项。
     */
    bool DeleteItem(int index);

    /**
     * 获取当前选中项的索引。
     * @return 成功返回索引(0起)，未选中返回 -1。
     */
    int SelectedItem() const;

    /**
     * 设置当前选中项。
     */
    bool SetSelectedItem(int index) const;

    /**
     * 获取项的总数。
     */
    int GetItemCount() const { return m_itemCount; }

    uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override ;
};

#endif // BXSCRIPT_COMBOBOX_H


// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION



#endif // BXSCRIPT_IMPLEMENTATION
