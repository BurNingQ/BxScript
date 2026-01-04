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
 * @brief    OleAut32
 */
#ifndef BXSCRIPT_OLEAUT32_H
#define BXSCRIPT_OLEAUT32_H

#include <windows.h>
#include <oleauto.h>

class OleAut32 {
public:
    // =========================================================================================
    // Group 1: BSTR 字符串操作 (Basic String)
    // @note BSTR 是宽字符串(wchar_t*)，但在内存头部隐藏了长度信息。
    //       普通 LPCWSTR 不能直接强转为 BSTR，必须用 SysAllocString 创建。
    // =========================================================================================

    /**
     * 分配系统字符串 (BSTR)。
     * @param sz 原始的 C 风格宽字符串。
     * @return 返回 BSTR 句柄。用完必须调用 SysFreeString。
     */
    static BSTR W32_SysAllocString(const OLECHAR *sz) {
        return SysAllocString(sz);
    }

    /**
     * 分配指定长度的系统字符串。
     */
    static BSTR W32_SysAllocStringLen(const OLECHAR *strIn, UINT ui) {
        return SysAllocStringLen(strIn, ui);
    }

    /**
     * 释放系统字符串。
     * @warning 必须用这个释放 BSTR，不能用 delete 或 free。
     */
    static void W32_SysFreeString(BSTR bstrString) {
        SysFreeString(bstrString);
    }

    /**
     * 获取 BSTR 的长度 (字符数，不含结尾 null)。
     * @note 这是一个 O(1) 操作，因为它直接读取头部隐藏的长度，比 wcslen 快。
     */
    static UINT W32_SysStringLen(BSTR bstrString) {
        return SysStringLen(bstrString);
    }


    // =========================================================================================
    // Group 2: VARIANT 动态类型 (Dynamic Typing)
    // @note VARIANT 是脚本语言与 COM 交互的核心数据结构，可以容纳 int, float, string, dispatch 等。
    // =========================================================================================

    /**
     * 初始化 VARIANT 结构体。
     * @usage 在使用任何 VARIANT 变量前，必须先调用此函数将其置为 VT_EMPTY。
     */
    static void W32_VariantInit(VARIANTARG *pvarg) {
        VariantInit(pvarg);
    }

    /**
     * 清除并释放 VARIANT 内容。
     * @usage 类似于析构函数。如果 VARIANT 里存的是 BSTR，它会调用 SysFreeString；如果是 Object，会调用 Release。
     */
    static HRESULT W32_VariantClear(VARIANTARG *pvarg) {
        return VariantClear(pvarg);
    }

    /**
     * 复制 VARIANT。
     * @note 执行深拷贝（Deep Copy）。
     */
    static HRESULT W32_VariantCopy(VARIANTARG *pvargDest, const VARIANTARG *pvargSrc) {
        return VariantCopy(pvargDest, pvargSrc);
    }


    // =========================================================================================
    // Group 3: Dispatch 与类型信息 (Scripting Support)
    // @usage 用于创建标准的分发接口，让 C++ 对象能被脚本语言调用。
    // =========================================================================================

    /**
     * 创建标准 IDispatch 接口实现。
     * @usage 这是一个简便方法，用于将 COM 对象的 IUnknown 转为 IDispatch，从而支持脚本调用。
     */
    static HRESULT W32_CreateStdDispatch(IUnknown *punkOuter, void *pvThis, ITypeInfo *ptinfo, IUnknown **ppunkStdDisp) {
        return CreateStdDispatch(punkOuter, pvThis, ptinfo, ppunkStdDisp);
    }

    /**
     * 从 INTERFACEDATA 创建类型信息。
     * @usage 用于在运行时动态构建类型信息，无需加载 .tlb 文件。
     */
    static HRESULT W32_CreateDispTypeInfo(INTERFACEDATA *pidata, LCID lcid, ITypeInfo **pptinfo) {
        return CreateDispTypeInfo(pidata, lcid, pptinfo);
    }
};

#endif //BXSCRIPT_OLEAUT32_H
