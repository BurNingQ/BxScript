/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/29
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    comctl32
 */
#ifndef BXSCRIPT_COMCTL32_H
#define BXSCRIPT_COMCTL32_H
#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

class ComCtl32 {

    struct AutoInitializer {
        AutoInitializer() {
            INITCOMMONCONTROLSEX icex;
            icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icex.dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_USEREX_CLASSES | ICC_COOL_CLASSES;
            InitCommonControlsEx(&icex);
        }
    };

    static AutoInitializer _initializer;

public:

    static void InitCommonControls(const tagINITCOMMONCONTROLSEX *params) {
        InitCommonControlsEx(params);
    }

    static void InitTrackMouseEvent(LPTRACKMOUSEEVENT params) {
        TrackMouseEvent(params);
    }
};

#endif //BXSCRIPT_COMCTL32_H