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
    // ======================== 文件对话框 ========================

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

    // ======================== 文件夹对话框 ========================

    /**
     * 显示文件夹选择对话框。
     */
    static std::wstring ShowBrowseFolderDlg(ControlBase *parent, const std::wstring &title = L"Select Folder");

    // ======================== 消息框 (MessageBox) ========================

    /**
     * 标准消息框封装。
     * @return IDOK, IDCANCEL, IDYES, IDNO 等。
     */
    static int MsgBox(ControlBase *parent, const std::wstring &text, const std::wstring &title, unsigned int flags);

    static bool MsgBoxConfirm(ControlBase *parent, const std::wstring &text, const std::wstring &title = L"Confirm"); // OK/Cancel
    static bool MsgBoxYesNo(ControlBase *parent, const std::wstring &text, const std::wstring &title = L"Question");

    static void MsgBoxInfo(ControlBase *parent, const std::wstring &text, const std::wstring &title = L"Information");

    static void MsgBoxWarn(ControlBase *parent, const std::wstring &text, const std::wstring &title = L"Warning");

    static void MsgBoxError(ControlBase *parent, const std::wstring &text, const std::wstring &title = L"Error");
};

#endif // BXSCRIPT_COMMON_DLGS_H

// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include "internal/User32.h"
#include "internal/ComDlg32.h"
#include "internal/Shell32.h"
#include "internal/Ole32.h"

// 内部辅助：处理 Win32 要求的双 Null 结尾 filter 字符串
static std::vector<wchar_t> PrepareFilterString(const std::wstring &filter) {
    std::vector<wchar_t> buf;
    for (wchar_t c: filter) {
        if (c == L'|') buf.push_back(L'\0');
        else buf.push_back(c);
    }
    buf.push_back(L'\0'); // 结尾第一个 \0
    buf.push_back(L'\0'); // 结尾第二个 \0 (Win32 规范)
    return buf;
}

std::wstring CommonDlgs::ShowOpenFileDlg(ControlBase *parent, const std::wstring &title, const std::wstring &filter, unsigned int filterIndex,
                                         const std::wstring &initialDir) {
    OPENFILENAMEW ofn = {0};
    wchar_t szFile[1024] = {0};
    auto filterBuf = PrepareFilterString(filter);

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = parent ? static_cast<HWND>(parent->GetHandle()) : nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = filterBuf.data();
    ofn.nFilterIndex = filterIndex;
    ofn.lpstrTitle = title.empty() ? nullptr : title.c_str();
    ofn.lpstrInitialDir = initialDir.empty() ? nullptr : initialDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (ComDlg32::W32_GetOpenFileName(&ofn)) {
        return std::wstring(szFile);
    }
    return L"";
}

std::wstring CommonDlgs::ShowSaveFileDlg(ControlBase *parent, const std::wstring &title, const std::wstring &filter, unsigned int filterIndex,
                                         const std::wstring &initialDir) {
    OPENFILENAMEW ofn = {0};
    wchar_t szFile[1024] = {0};
    auto filterBuf = PrepareFilterString(filter);

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = parent ? static_cast<HWND>(parent->GetHandle()) : nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = filterBuf.data();
    ofn.nFilterIndex = filterIndex;
    ofn.lpstrTitle = title.empty() ? nullptr : title.c_str();
    ofn.lpstrInitialDir = initialDir.empty() ? nullptr : initialDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (ComDlg32::W32_GetSaveFileName(&ofn)) {
        return std::wstring(szFile);
    }
    return L"";
}

std::wstring CommonDlgs::ShowBrowseFolderDlg(ControlBase *parent, const std::wstring &title) {
    BROWSEINFOW bi = {0};
    bi.hwndOwner = parent ? static_cast<HWND>(parent->GetHandle()) : nullptr;
    bi.lpszTitle = title.c_str();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_USENEWUI;

    LPITEMIDLIST pidl = Shell32::W32_SHBrowseForFolder(&bi);
    if (pidl != nullptr) {
        wchar_t path[MAX_PATH];
        if (Shell32::W32_SHGetPathFromIDList(pidl, path)) {
            Ole32::W32_CoTaskMemFree(pidl);
            return std::wstring(path);
        }
        Ole32::W32_CoTaskMemFree(pidl);
    }
    return L"";
}

int CommonDlgs::MsgBox(ControlBase *parent, const std::wstring &text, const std::wstring &title, unsigned int flags) {
    HWND hwnd = parent ? static_cast<HWND>(parent->GetHandle()) : nullptr;
    return User32::W32_MessageBox(hwnd, text.c_str(), title.c_str(), flags);
}

bool CommonDlgs::MsgBoxConfirm(ControlBase *parent, const std::wstring &text, const std::wstring &title) {
    return MsgBox(parent, text, title, MB_ICONQUESTION | MB_OKCANCEL) == IDOK;
}

bool CommonDlgs::MsgBoxYesNo(ControlBase *parent, const std::wstring &text, const std::wstring &title) {
    return MsgBox(parent, text, title, MB_ICONQUESTION | MB_YESNO) == IDYES;
}

void CommonDlgs::MsgBoxInfo(ControlBase *parent, const std::wstring &text, const std::wstring &title) {
    MsgBox(parent, text, title, MB_ICONINFORMATION | MB_OK);
}

void CommonDlgs::MsgBoxWarn(ControlBase *parent, const std::wstring &text, const std::wstring &title) {
    MsgBox(parent, text, title, MB_ICONWARNING | MB_OK);
}

void CommonDlgs::MsgBoxError(ControlBase *parent, const std::wstring &text, const std::wstring &title) {
    MsgBox(parent, text, title, MB_ICONERROR | MB_OK);
}

#endif // BXSCRIPT_IMPLEMENTATION
