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
 * @brief    Shlwapi
 */
#ifndef BXSCRIPT_SHLWAPI_H
#define BXSCRIPT_SHLWAPI_H

#include <windows.h>
#include <shlwapi.h>
#include <objidl.h>

class Shlwapi {
public:
    // =========================================================================================
    // Group 1: 内存流 (Memory Streams)
    // =========================================================================================

    /**
     * 创建基于内存的 IStream。
     * @usage 这是 GDI+ 加载图片最简单的方法。比 CreateStreamOnHGlobal 更快，不需要 GlobalAlloc。
     * @param pInit 初始化数据指针（如果不传数据，给 NULL）。
     * @param cbInit 数据大小。
     * @return IStream 指针，用完记得 pStream->Release()。
     */
    static IStream *W32_SHCreateMemStream(const BYTE *pInit, UINT cbInit) {
        return SHCreateMemStream(pInit, cbInit);
    }


    // =========================================================================================
    // Group 2: 路径操作 (Path Utilities)
    // @note 脚本语言通常需要处理文件路径，这些原生 API 处理了 Windows 复杂的路径规则。
    // =========================================================================================

    /**
     * 判断文件或目录是否存在。
     * @return TRUE 存在，FALSE 不存在。
     */
    static BOOL W32_PathFileExists(LPCWSTR pszPath) {
        return PathFileExistsW(pszPath);
    }

    /**
     * 连接两个路径 (自动处理中间的斜杠)。
     * @usage "C:\Folder" + "file.txt" -> "C:\Folder\file.txt"
     * @param pszPathOut [输出] 缓冲区，大小至少为 MAX_PATH。
     */
    static LPWSTR W32_PathCombine(LPWSTR pszPathOut, LPCWSTR pszDir, LPCWSTR pszFile) {
        return PathCombineW(pszPathOut, pszDir, pszFile);
    }

    /**
     * 获取文件扩展名指针。
     * @usage "C:\file.txt" -> ".txt"
     */
    static LPWSTR W32_PathFindExtension(LPCWSTR pszPath) {
        return PathFindExtensionW(pszPath);
    }

    /**
     * 获取文件名指针 (不含目录)。
     * @usage "C:\Dir\file.txt" -> "file.txt"
     */
    static LPWSTR W32_PathFindFileName(LPCWSTR pszPath) {
        return PathFindFileNameW(pszPath);
    }

    /**
     * 去除文件名，只保留目录。
     * @usage "C:\Dir\file.txt" -> "C:\Dir"
     */
    static BOOL W32_PathRemoveFileSpec(LPWSTR pszPath) {
        return PathRemoveFileSpecW(pszPath);
    }

    /**
     * 规范化路径 (处理 .. 和 .)。
     * @usage "C:\Dir\Sub\..\file.txt" -> "C:\Dir\file.txt"
     */
    static BOOL W32_PathCanonicalize(LPWSTR pszBuf, LPCWSTR pszPath) {
        return PathCanonicalizeW(pszBuf, pszPath);
    }
};

#endif //BXSCRIPT_SHLWAPI_H
