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
 * @brief    CommonDlgs
 */
#ifndef BXSCRIPT_COMMON_DLGS_H
#define BXSCRIPT_COMMON_DLGS_H

#include <string>
#include <vector>
#include "ControlBase.h"

class CommonDlgs {
public:
    /**
     * 显示打开文件对话框。
     * @param filter 格式如 "Text Files|*.txt|All Files|*.*"
     */
    static std::wstring ShowOpenFileDlg(ControlBase *parent,
                                        const std::wstring &title = L"Open File",
                                        const std::wstring &filter = L"All Files|*.*",
                                        unsigned int filterIndex = 1,
                                        const std::wstring &initialDir = L"");

    /**
     * 显示保存文件对话框。
     */
    static std::wstring ShowSaveFileDlg(ControlBase *parent,
                                        const std::wstring &title = L"Save File",
                                        const std::wstring &filter = L"All Files|*.*",
                                        unsigned int filterIndex = 1,
                                        const std::wstring &initialDir = L"");

    /**
     * 显示文件夹选择对话框。
     */
    static std::wstring ShowBrowseFolderDlg(ControlBase *parent, const std::wstring &title = L"Select Folder");

    /**
     * 标准消息框封装。
     * @return IDOK, IDCANCEL, IDYES, IDNO 等。
     */
    static int MsgBox(ControlBase *parent, const std::wstring &text, const std::wstring &title, unsigned int flags);

    static bool MsgBoxConfirm(ControlBase *parent, const std::wstring &text, const std::wstring &title = L"Confirm");

    static bool MsgBoxYesNo(ControlBase *parent, const std::wstring &text, const std::wstring &title = L"Question");

    static void MsgBoxInfo(ControlBase *parent, const std::wstring &text, const std::wstring &title = L"Information");

    static void MsgBoxWarn(ControlBase *parent, const std::wstring &text, const std::wstring &title = L"Warning");

    static void MsgBoxError(ControlBase *parent, const std::wstring &text, const std::wstring &title = L"Error");
};

#endif // BXSCRIPT_COMMON_DLGS_H
