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
 * @brief    Ole32
 */
#ifndef BXSCRIPT_OLE32_H
#define BXSCRIPT_OLE32_H

#include <windows.h>
#include <objbase.h>

class Ole32 {
public:
    // =========================================================================================
    // Group 1: COM 环境初始化与销毁 (Lifecycle)
    // @warning 在使用任何 COM 功能（包括 GDI+ 加载图片流、打开文件对话框）之前，必须先初始化 COM。
    // =========================================================================================

    /**
     * 初始化 COM 库 (旧版 API)。
     * @note 等同于 CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)。
     * @return S_OK 或 S_FALSE (如果已经初始化过了)。
     */
    static HRESULT W32_CoInitialize() {
        return CoInitialize(nullptr);
    }

    /**
     * 初始化 COM 库 (推荐)。
     * @param dwCoInit 线程模型：
     *        - COINIT_APARTMENTTHREADED (0x2): 单线程单元 (GUI 程序常用)
     *        - COINIT_MULTITHREADED (0x0): 多线程单元 (后台服务常用)
     */
    static HRESULT W32_CoInitializeEx(DWORD dwCoInit) {
        return CoInitializeEx(nullptr, dwCoInit);
    }

    /**
     * 卸载 COM 库。
     * @warning 必须与 CoInitialize 成对调用（通常在程序退出前）。
     */
    static void W32_CoUninitialize() {
        CoUninitialize();
    }

    /**
     * 初始化 COM 安全设置。
     * @note 某些高级功能（如 WMI 查询）需要配置这个，普通 GUI 程序通常不需要。
     */
    static HRESULT W32_CoInitializeSecurity(PSECURITY_DESCRIPTOR pSecDesc, LONG cAuthSvc, SOLE_AUTHENTICATION_SERVICE *asAuthSvc,
                                            void *pReserved1, DWORD dwAuthnLevel, DWORD dwImpLevel,
                                            void *pAuthList, DWORD dwCapabilities, void *pReserved3) {
        return CoInitializeSecurity(pSecDesc, cAuthSvc, asAuthSvc, pReserved1, dwAuthnLevel, dwImpLevel, pAuthList, dwCapabilities, pReserved3);
    }


    // =========================================================================================
    // Group 2: 对象创建 (Object Creation)
    // =========================================================================================

    /**
     * 【核心函数】创建 COM 对象实例。
     * @usage 相当于 C++ 的 new，但用于创建系统组件（如 ITaskbarList, IFileOpenDialog）。
     * @param rclsid 类 ID (CLSID)
     * @param riid 接口 ID (IID)
     * @param ppv 输出的接口指针
     */
    static HRESULT W32_CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv) {
        return CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
    }

    /**
     * 获取当前活跃的对象（通常用于连接已经运行的程序，如 Excel）。
     */
    static HRESULT W32_CoGetObject(LPCWSTR pszName, BIND_OPTS *pBindOptions, REFIID riid, void **ppv) {
        return CoGetObject(pszName, pBindOptions, riid, ppv);
    }


    // =========================================================================================
    // Group 3: 内存管理 (Memory)
    // =========================================================================================

    /**
     * 释放 COM 任务内存。
     * @warning 凡是 COM API 返回的字符串 (LPWSTR)，如果不特殊说明，都必须用这个函数释放，不能用 delete/free。
     */
    static void W32_CoTaskMemFree(LPVOID pv) {
        CoTaskMemFree(pv);
    }


    // =========================================================================================
    // Group 4: 流操作 (Streams)
    // @usage 这里的 CreateStreamOnHGlobal 是 GDI+ 从内存加载图片的关键依赖。
    // =========================================================================================

    /**
     * 在内存块上创建一个 IStream 流对象。
     * @param hGlobal 内存句柄 (GlobalAlloc 分配的)。如果传 NULL，系统自动分配。
     * @param fDeleteOnRelease 如果为 TRUE，释放 IStream 时自动释放 hGlobal 内存。
     * @param ppstm 输出的 IStream 指针。
     */
    static HRESULT W32_CreateStreamOnHGlobal(HGLOBAL hGlobal, BOOL fDeleteOnRelease, LPSTREAM *ppstm) {
        return CreateStreamOnHGlobal(hGlobal, fDeleteOnRelease, ppstm);
    }


    // =========================================================================================
    // Group 5: 字符串与 GUID 转换 (Utilities)
    // =========================================================================================

    /**
     * 字符串 GUID 转 CLSID 结构体。
     * @param lpsz 如 "{00000000-0000-0000-C000-000000000046}"
     */
    static HRESULT W32_CLSIDFromString(LPCWSTR lpsz, LPCLSID pclsid) {
        return CLSIDFromString(lpsz, pclsid);
    }

    /**
     * ProgID (如 "Excel.Application") 转 CLSID。
     */
    static HRESULT W32_CLSIDFromProgID(LPCWSTR lpszProgID, LPCLSID lpclsid) {
        return CLSIDFromProgID(lpszProgID, lpclsid);
    }

    /**
     * 字符串 GUID 转 IID 结构体。
     */
    static HRESULT W32_IIDFromString(LPCWSTR lpsz, LPIID lpiid) {
        return IIDFromString(lpsz, lpiid);
    }

    /**
     * CLSID 转 字符串。
     * @warning 返回的字符串必须调用 W32_CoTaskMemFree 释放！
     */
    static HRESULT W32_StringFromCLSID(REFCLSID rclsid, LPOLESTR *lplpsz) {
        return StringFromCLSID(rclsid, lplpsz);
    }

    /**
     * IID 转 字符串。
     * @warning 返回的字符串必须调用 W32_CoTaskMemFree 释放！
     */
    static HRESULT W32_StringFromIID(REFIID rclsid, LPOLESTR *lplpsz) {
        return StringFromIID(rclsid, lplpsz);
    }
};

#endif //BXSCRIPT_OLE32_H
