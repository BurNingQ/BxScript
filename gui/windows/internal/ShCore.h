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
 * @brief    ShCore
 */
#ifndef BXSCRIPT_SHCORE_H
#define BXSCRIPT_SHCORE_H

#include <windows.h>
#include <shellscalingapi.h>

class ShCore {
    // 定义函数指针类型
    typedef HRESULT (STDAPICALLTYPE *PFN_GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT *, UINT *);

    typedef HRESULT (STDAPICALLTYPE *PFN_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);

    typedef HRESULT (STDAPICALLTYPE *PFN_GetProcessDpiAwareness)(HANDLE, PROCESS_DPI_AWARENESS *);

    // 内部单例：负责加载 DLL 和函数
    struct DynamicLoader {
        HMODULE hModule = nullptr;
        PFN_GetDpiForMonitor pGetDpiForMonitor = nullptr;
        PFN_SetProcessDpiAwareness pSetProcessDpiAwareness = nullptr;
        PFN_GetProcessDpiAwareness pGetProcessDpiAwareness = nullptr;

        DynamicLoader() {
            // 尝试加载 DLL
            hModule = LoadLibraryW(L"shcore.dll");
            if (hModule) {
                pGetDpiForMonitor = reinterpret_cast<PFN_GetDpiForMonitor>(GetProcAddress(hModule, "GetDpiForMonitor"));
                pSetProcessDpiAwareness = reinterpret_cast<PFN_SetProcessDpiAwareness>(GetProcAddress(hModule, "SetProcessDpiAwareness"));
                pGetProcessDpiAwareness = reinterpret_cast<PFN_GetProcessDpiAwareness>(GetProcAddress(hModule, "GetProcessDpiAwareness"));
            }
        }

        ~DynamicLoader() {
            if (hModule) FreeLibrary(hModule);
        }
    };

    // 获取单例实例 (C++11 保证线程安全)
    static DynamicLoader &Get() {
        static DynamicLoader loader;
        return loader;
    }

public:
    /**
     * 判断当前系统是否支持 ShCore (Win8.1+)
     */
    static bool IsAvailable() {
        return Get().hModule != nullptr;
    }

    /**
     * [安全封装] 获取显示器 DPI
     * @return
     *   S_OK: 获取成功 (Win8.1+)
     *   E_NOINTERFACE: 系统不支持 (Win7)，此时 outX/outY 会被设为 96 (默认DPI)
     */
    static HRESULT W32_GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY) {
        auto &api = Get();
        if (api.pGetDpiForMonitor) {
            return api.pGetDpiForMonitor(hmonitor, dpiType, dpiX, dpiY);
        }

        // Win7 缺省处理：返回标准的 96 DPI
        if (dpiX) *dpiX = 96;
        if (dpiY) *dpiY = 96;
        return E_NOINTERFACE; // 告诉调用者：这是个假数据，或者你需要用 GetDeviceCaps 自己查
    }

    /**
     * [安全封装] 设置进程 DPI 感知
     */
    static HRESULT W32_SetProcessDpiAwareness(PROCESS_DPI_AWARENESS value) {
        auto &api = Get();
        if (api.pSetProcessDpiAwareness) {
            return api.pSetProcessDpiAwareness(value);
        }
        // Win7 不支持此 API，但 Win7 默认也没那么复杂的 DPI 问题
        return E_NOINTERFACE;
    }

    static HRESULT W32_GetProcessDpiAwareness(HANDLE hprocess, PROCESS_DPI_AWARENESS *value) {
        auto &api = Get();
        if (api.pGetProcessDpiAwareness) {
            return api.pGetProcessDpiAwareness(hprocess, value);
        }
        if (value) *value = PROCESS_DPI_UNAWARE;
        return E_NOINTERFACE;
    }
};

#endif //BXSCRIPT_SHCORE_H
