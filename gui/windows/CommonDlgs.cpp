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
 * @brief    CommonDlgs
 */
#include "CommonDlgs.h"
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
    // 结尾第一个 \0
    buf.push_back(L'\0');
    // 结尾第二个 \0 (Win32 规范)
    buf.push_back(L'\0');
    return buf;
}

std::wstring CommonDlgs::ShowOpenFileDlg(ControlBase *parent, const std::wstring &title, const std::wstring &filter, unsigned int filterIndex,
                                         const std::wstring &initialDir) {
    OPENFILENAMEW ofn = {0};
    wchar_t szFile[1024] = {0};
    auto filterBuf = PrepareFilterString(filter);
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = parent ? static_cast<HWND>(parent->Handle()) : nullptr;
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
    ofn.hwndOwner = parent ? static_cast<HWND>(parent->Handle()) : nullptr;
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
    bi.hwndOwner = parent ? static_cast<HWND>(parent->Handle()) : nullptr;
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
    HWND hwnd = parent ? static_cast<HWND>(parent->Handle()) : nullptr;
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
