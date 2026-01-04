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
 * @brief    ComDlg32
 */
#ifndef BXSCRIPT_COMDLG32_H
#define BXSCRIPT_COMDLG32_H
#include <windows.h>
#include <commdlg.h>

class ComDlg32 {
public:
    static bool W32_GetOpenFileName(LPOPENFILENAMEW ofn) {
        return GetOpenFileNameW(ofn);
    }

    static bool W32_GetSaveFileName(LPOPENFILENAMEW ofn) {
        return GetSaveFileNameW(ofn);
    }

    static DWORD W32_CommDlgExtendedError() {
        return CommDlgExtendedError();
    }
};


#endif //BXSCRIPT_COMDLG32_H
