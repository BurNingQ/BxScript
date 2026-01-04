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
 * @brief    UxTheme
 */
#ifndef BXSCRIPT_UXTHEME_H
#define BXSCRIPT_UXTHEME_H

#include <windows.h>
#include <uxtheme.h>
#include <vsstyle.h>

class UxTheme {
public:
    // =========================================================================================
    // Group 1: 主题数据生命周期 (Lifecycle)
    // =========================================================================================

    /**
     * 打开主题数据句柄。
     * @param hwnd 控件句柄。
     * @param pszClassList 控件类名，如 L"Button", L"TreeView", L"Explorer"。
     * @return 主题句柄 (HTHEME)。用完必须调用 CloseThemeData。
     */
    static HTHEME W32_OpenThemeData(HWND hwnd, LPCWSTR pszClassList) {
        return OpenThemeData(hwnd, pszClassList);
    }

    /**
     * 关闭主题数据句柄。
     */
    static HRESULT W32_CloseThemeData(HTHEME hTheme) {
        return CloseThemeData(hTheme);
    }

    /**
     * 强制设置窗口的主题风格。
     * @usage 这是个“魔法函数”。
     *        比如给 ListView 调用 SetWindowTheme(hwnd, L"Explorer", NULL)，
     *        列表就会变成资源管理器那种漂亮的样式（行距变大、选中高亮变蓝）。
     */
    static HRESULT W32_SetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList) {
        return SetWindowTheme(hwnd, pszSubAppName, pszSubIdList);
    }


    // =========================================================================================
    // Group 2: 绘制 (Drawing)
    // =========================================================================================

    /**
     * 绘制主题背景。
     * @param hTheme 主题句柄。
     * @param hdc 绘图上下文。
     * @param iPartId 部件 ID (如 BP_PUSHBUTTON)。
     * @param iStateId 状态 ID (如 PBS_NORMAL, PBS_PRESSED)。
     * @param pRect 绘制区域。
     * @param pClipRect 裁剪区域 (可选，传 NULL)。
     */
    static HRESULT W32_DrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, CONST RECT *pRect, CONST RECT *pClipRect) {
        return DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
    }

    /**
     * 绘制主题文本。
     * @note 会根据当前主题自动选择字体颜色和阴影。
     */
    static HRESULT W32_DrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags,
                                     DWORD dwTextFlags2, CONST RECT *pRect) {
        return DrawThemeText(hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, dwTextFlags2, pRect);
    }


    // =========================================================================================
    // Group 3: 测量与检测 (Measurement & Status)
    // =========================================================================================

    /**
     * 计算主题文本的大小。
     * @usage 用于自动布局，确保文字不会被截断。
     */
    static HRESULT W32_GetThemeTextExtent(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags,
                                          CONST RECT *pBoundingRect, LPRECT pExtentRect) {
        return GetThemeTextExtent(hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, pBoundingRect, pExtentRect);
    }

    /**
     * 检查系统是否开启了视觉样式。
     * @return 如果用户切回了“Windows 经典”模式，返回 FALSE。
     */
    static BOOL W32_IsThemeActive() {
        return IsThemeActive();
    }

    /**
     * 检查当前应用是否被主题化。
     */
    static BOOL W32_IsAppThemed() {
        return IsAppThemed();
    }
};

#endif //BXSCRIPT_UXTHEME_H
