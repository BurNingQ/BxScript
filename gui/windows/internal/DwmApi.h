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
 * @brief    DwmApi
 */
#ifndef BXSCRIPT_DWMAPI_H
#define BXSCRIPT_DWMAPI_H

#include <windows.h>
#include <dwmapi.h>

class DwmApi {
    // 定义函数指针
    typedef HRESULT (STDAPICALLTYPE *PFN_DwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);

    typedef HRESULT (STDAPICALLTYPE *PFN_DwmExtendFrameIntoClientArea)(HWND, const MARGINS *);

    typedef HRESULT (STDAPICALLTYPE *PFN_DwmIsCompositionEnabled)(BOOL *);

    struct DynamicLoader {
        HMODULE hModule = nullptr;
        PFN_DwmSetWindowAttribute pDwmSetWindowAttribute = nullptr;
        PFN_DwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = nullptr;
        PFN_DwmIsCompositionEnabled pDwmIsCompositionEnabled = nullptr;

        DynamicLoader() {
            hModule = LoadLibraryW(L"dwmapi.dll");
            if (hModule) {
                pDwmSetWindowAttribute = reinterpret_cast<PFN_DwmSetWindowAttribute>(GetProcAddress(hModule, "DwmSetWindowAttribute"));
                pDwmExtendFrameIntoClientArea = reinterpret_cast<PFN_DwmExtendFrameIntoClientArea>(GetProcAddress(
                    hModule, "DwmExtendFrameIntoClientArea"));
                pDwmIsCompositionEnabled = reinterpret_cast<PFN_DwmIsCompositionEnabled>(GetProcAddress(hModule, "DwmIsCompositionEnabled"));
            }
        }

        ~DynamicLoader() {
            if (hModule) FreeLibrary(hModule);
        }
    };

    static DynamicLoader &Get() {
        static DynamicLoader loader;
        return loader;
    }

public:
    /**
     * 判断 DWM 是否可用 (Win7+)
     */
    static bool IsAvailable() {
        return Get().hModule != nullptr;
    }

    /**
     * 设置窗口属性 (深色模式、圆角等)
     */
    static HRESULT W32_DwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) {
        auto &api = Get();
        if (api.pDwmSetWindowAttribute) {
            return api.pDwmSetWindowAttribute(hwnd, dwAttribute, pvAttribute, cbAttribute);
        }
        return E_NOTIMPL; // 不支持，直接返回未实现
    }

    /**
     * 扩展玻璃效果到客户区 (Win7 Aero 特效核心函数)
     */
    static HRESULT W32_DwmExtendFrameIntoClientArea(HWND hwnd, const MARGINS *pMarInset) {
        auto &api = Get();
        if (api.pDwmExtendFrameIntoClientArea) {
            return api.pDwmExtendFrameIntoClientArea(hwnd, pMarInset);
        }
        return E_NOTIMPL;
    }

    /**
     * 判断是否开启了 Aero / 桌面组合
     */
    static HRESULT W32_DwmIsCompositionEnabled(BOOL *pfEnabled) {
        auto &api = Get();
        if (api.pDwmIsCompositionEnabled) {
            return api.pDwmIsCompositionEnabled(pfEnabled);
        }
        if (pfEnabled) *pfEnabled = FALSE;
        return E_NOTIMPL;
    }

    // --- Helpers ---

    static void EnableDarkMode(HWND hwnd, bool enable) {
        BOOL value = enable ? TRUE : FALSE;
        // 20 = DWMWA_USE_IMMERSIVE_DARK_MODE (Win10 1809+)
        W32_DwmSetWindowAttribute(hwnd, 20, &value, sizeof(value));
    }

    static void SetCornerPreference(HWND hwnd, int mode) {
        // 33 = DWMWA_WINDOW_CORNER_PREFERENCE (Win11)
        W32_DwmSetWindowAttribute(hwnd, 33, &mode, sizeof(mode));
    }
};

#endif //BXSCRIPT_DWMAPI_H
