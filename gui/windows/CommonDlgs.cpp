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

// Windows 消息框的文本硬上限：65535 个字符。刚好 65535 能弹，65536 就静默返回 0，
// 窗口根本不创建，GetLastError 也是 0 —— 外部表现就是"alert 没反应"。
static constexpr size_t kMsgBoxTextLimit = 65535;
// 留一点余量给截断提示本身，避免拼接后又踩到上限。
static constexpr size_t kMsgBoxKeepChars = 60000;

std::wstring CommonDlgs::TruncateMsgBoxText(const std::wstring &text) {
    if (text.size() <= kMsgBoxTextLimit) return text;

    size_t keep = kMsgBoxKeepChars;
    // 不要从代理对中间切断，否则尾部会留半个字符（显示成方块）
    if (const wchar_t last = text[keep - 1]; last >= 0xD800 && last <= 0xDBFF) {
        --keep;
    }
    std::wstring result = text.substr(0, keep);
    result += L"\r\n\r\n……（内容过长，已截断；原始长度 " + std::to_wstring(text.size()) + L" 字符）";
    return result;
}

int CommonDlgs::MsgBox(ControlBase *parent, const std::wstring &text, const std::wstring &title, unsigned int flags) {
    HWND hwnd = nullptr;
    if (parent) {
        hwnd = static_cast<HWND>(parent->Handle());
    } else {
        hwnd = ::GetActiveWindow();
    }
    const std::wstring safeText = TruncateMsgBoxText(text);
    return User32::W32_MessageBox(hwnd, safeText.c_str(), title.c_str(), flags);
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
