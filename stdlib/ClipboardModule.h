/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/4/22
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    ClipboardModule
 */
#ifndef BXSCRIPT_CLIPBOARDMODULE_H
#define BXSCRIPT_CLIPBOARDMODULE_H
#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include "../evaluator/Value.h"
#include "../common/StringKit.h"

#if defined(_WIN32)
#include "../gui/windows/internal/User32.h"
#include "../gui/windows/internal/Kernel32.h"

#ifndef CF_UNICODETEXT
#define CF_UNICODETEXT 13
#endif

#ifndef GMEM_MOVEABLE
#define GMEM_MOVEABLE 0x0002
#endif

#ifndef GMEM_ZEROINIT
#define GMEM_ZEROINIT 0x0040
#endif
#endif

class ClipboardModule {
    static void InitGetText(std::shared_ptr<ObjectValue> &o) {
        o->Set("getText", std::make_shared<NativeFunctionValue>([](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
            if (!User32::W32_IsClipboardFormatAvailable(CF_UNICODETEXT)) {
                return std::make_shared<NullValue>();
            }
            if (!User32::W32_OpenClipboard(nullptr)) {
                return std::make_shared<NullValue>();
            }
            void *hData = User32::W32_GetClipboardData(CF_UNICODETEXT);
            if (!hData) {
                User32::W32_CloseClipboard();
                return std::make_shared<NullValue>();
            }
            void *pText = Kernel32::W32_GlobalLock(hData);
            if (!pText) {
                User32::W32_CloseClipboard();
                return std::make_shared<NullValue>();
            }
            std::wstring wstr(static_cast<wchar_t *>(pText));
            Kernel32::W32_GlobalUnlock(hData);
            User32::W32_CloseClipboard();
            return std::make_shared<StringValue>(StringKit::U16ToU8(wstr));
#else
            return std::make_shared<NullValue>();
#endif
        }));
    }

    static void InitSetText(std::shared_ptr<ObjectValue> &o) {
        o->Set("setText", std::make_shared<NativeFunctionValue>([](const std::vector<ValuePtr> &args) -> ValuePtr {
#if defined(_WIN32)
            if (args.empty()) return std::make_shared<BoolValue>(false);
            const std::string utf8Str = args[0]->ToString();
            std::wstring wstr = StringKit::U8ToU16(utf8Str);
            const size_t byteSize = (wstr.length() + 1) * sizeof(wchar_t);
            void *hMem = Kernel32::W32_GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, byteSize);
            if (!hMem) return std::make_shared<BoolValue>(false);
            void *pMem = Kernel32::W32_GlobalLock(hMem);
            if (!pMem) {
                Kernel32::W32_GlobalFree(hMem);
                return std::make_shared<BoolValue>(false);
            }
            std::memcpy(pMem, wstr.c_str(), byteSize);
            Kernel32::W32_GlobalUnlock(hMem);
            if (!User32::W32_OpenClipboard(nullptr)) {
                Kernel32::W32_GlobalFree(hMem);
                return std::make_shared<BoolValue>(false);
            }
            User32::W32_EmptyClipboard();
            User32::W32_SetClipboardData(CF_UNICODETEXT, hMem);
            User32::W32_CloseClipboard();
            return std::make_shared<BoolValue>(true);
#else
            return std::make_shared<BoolValue>(false);
#endif
        }));
    }

    static void InitClear(std::shared_ptr<ObjectValue> &o) {
        o->Set("clear", std::make_shared<NativeFunctionValue>([](const std::vector<ValuePtr> &args) -> ValuePtr {
#ifdef _WIN32
            bool success = false;
            if (User32::W32_OpenClipboard(nullptr)) {
                success = (User32::W32_EmptyClipboard() != 0);
                User32::W32_CloseClipboard();
            }
            return std::make_shared<BoolValue>(success);
#else
            return std::make_shared<BoolValue>(false);
#endif
        }));
    }

public:
    static ValuePtr CreateClipboardModule() {
        auto o = std::make_shared<ObjectValue>();
        InitGetText(o);
        InitSetText(o);
        InitClear(o);
        return o;
    }
};


#endif //BXSCRIPT_CLIPBOARDMODULE_H
