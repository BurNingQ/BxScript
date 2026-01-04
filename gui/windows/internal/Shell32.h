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
 * @brief    Shell32
 */
#ifndef BXSCRIPT_SHELL32_H
#define BXSCRIPT_SHELL32_H

#include <windows.h>
#include <shellapi.h> // 必须包含
#include <shlobj.h>   // SHBrowseForFolder, CSIDL 定义
#include <string>

class Shell32 {
public:
    // =========================================================================================
    // Group 1: 外部程序执行 (Execute)
    // =========================================================================================

    /**
     * 运行外部程序、打开网页或打印文件。
     * @param lpOperation 操作类型："open" (打开/运行), "print" (打印), "explore" (资源管理器), "edit", "runas" (管理员)。
     * @param lpFile 文件路径、EXE名或 URL。
     * @return
     *      > 32: 成功。
     *      <= 32: 失败，返回值为错误码 (如 ERROR_FILE_NOT_FOUND, SE_ERR_ACCESSDENIED)。
     */
    static HINSTANCE W32_ShellExecute(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd) {
        return ShellExecuteW(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
    }


    // =========================================================================================
    // Group 2: 文件拖放 (Drag & Drop)
    // @usage 实现将文件从桌面拖入窗口的功能。
    // =========================================================================================

    /**
     * 注册/取消注册窗口接受文件拖放。
     * @note 注册后，窗口会收到 WM_DROPFILES 消息。
     */
    static void W32_DragAcceptFiles(HWND hWnd, BOOL fAccept) {
        DragAcceptFiles(hWnd, fAccept);
    }

    /**
     * 获取拖放的文件数量。
     * @param hDrop 消息 wParam 传来的句柄。
     */
    static UINT W32_DragQueryFileCount(HDROP hDrop) {
        return DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
    }

    /**
     * 获取拖放的文件路径。
     * @param iFile 文件索引 (0 ~ Count-1)。
     * @param lpszFile [输出] 接收文件名的缓冲区 (大小建议为 MAX_PATH)。
     * @return 复制的字符数。
     */
    static UINT W32_DragQueryFilePath(HDROP hDrop, UINT iFile, LPWSTR lpszFile, UINT cch) {
        return DragQueryFileW(hDrop, iFile, lpszFile, cch);
    }

    /**
     * 获取拖放时的鼠标位置。
     * @return 如果落在客户区返回 TRUE，否则 FALSE。
     */
    static BOOL W32_DragQueryPoint(HDROP hDrop, LPPOINT lppt) {
        return DragQueryPoint(hDrop, lppt);
    }

    /**
     * 结束拖放操作，释放内存。
     * @warning 处理完 WM_DROPFILES 后必须调用。
     */
    static void W32_DragFinish(HDROP hDrop) {
        DragFinish(hDrop);
    }


    // =========================================================================================
    // Group 3: 文件夹选择对话框 (Folder Browser)
    // =========================================================================================

    /**
     * 弹出“浏览文件夹”对话框。
     * @return 返回 PIDL (Item ID List)，需要用 SHGetPathFromIDList 转换为字符串，且必须用 CoTaskMemFree 释放。
     */
    static LPITEMIDLIST W32_SHBrowseForFolder(LPBROWSEINFOW lpbi) {
        return SHBrowseForFolderW(lpbi);
    }

    /**
     * 将 PIDL 转换为文件系统路径。
     */
    static BOOL W32_SHGetPathFromIDList(LPCITEMIDLIST pidl, LPWSTR pszPath) {
        return SHGetPathFromIDListW(pidl, pszPath);
    }


    // =========================================================================================
    // Group 4: 特殊路径与图标 (Special Paths & Icons)
    // =========================================================================================

    /**
     * 获取特殊文件夹路径 (如 桌面、我的文档、AppData)。
     * @param csidl 文件夹ID (如 CSIDL_DESKTOP, CSIDL_PERSONAL)。
     * @param fCreate 如果不存在是否创建。
     * @note 虽然该 API 已过时，但兼容性最好 (Win95+)。
     */
    static BOOL W32_SHGetSpecialFolderPath(HWND hwnd, LPWSTR pszPath, int csidl, BOOL fCreate) {
        return SHGetSpecialFolderPathW(hwnd, pszPath, csidl, fCreate);
    }

    /**
     * 从 EXE 或 DLL 中提取图标。
     * @param nIconIndex 图标索引。
     * @return 图标句柄，用完需 DestroyIcon。
     */
    static HICON W32_ExtractIcon(HINSTANCE hInst, LPCWSTR lpszExeFileName, UINT nIconIndex) {
        return ExtractIconW(hInst, lpszExeFileName, nIconIndex);
    }
};

#endif //BXSCRIPT_SHELL32_H
