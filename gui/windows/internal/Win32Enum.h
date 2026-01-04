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
 * @brief    Win32Enum
 */
#ifndef BXSCRIPT_WIN32ENUM_H
#define BXSCRIPT_WIN32ENUM_H


enum Win32Enum {
    // InitCommonControlsEx flags
    ICC_LISTVIEW_CLASSES = 1,
    ICC_TREEVIEW_CLASSES = 2,
    ICC_BAR_CLASSES = 3,
    ICC_TAB_CLASSES = 8,
    ICC_UPDOWN_CLASS = 16,
    ICC_PROGRESS_CLASS = 32,
    ICC_HOTKEY_CLASS = 64,
    ICC_ANIMATE_CLASS = 128,
    ICC_WIN95_CLASSES = 255,
    ICC_DATE_CLASSES = 256,
    ICC_USEREX_CLASSES = 512,
    ICC_COOL_CLASSES = 1024,
    ICC_INTERNET_CLASSES = 2048,
    ICC_PAGESCROLLER_CLASS = 4096,
    ICC_NATIVEFNTCTL_CLASS = 8192,
    INFOTIPSIZE = 1024,
    ICC_STANDARD_CLASSES = 0x00004000,
    ICC_LINK_CLASS = 0x00008000
};


#endif //BXSCRIPT_WIN32ENUM_H
