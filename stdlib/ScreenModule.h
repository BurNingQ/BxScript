/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/30
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    MouseModule
 */
#ifndef BXSCRIPT_MOUSEMODULE_H
#define BXSCRIPT_MOUSEMODULE_H
#include <memory>

#include "evaluator/Value.h"
#include <windows.h>

class ScreenModule {
    static void InitWidth(const std::shared_ptr<ObjectValue> &o) {
        const auto widthFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &)-> ValuePtr {
#if defined(_WIN32)
                return std::make_shared<NumberValue>(GetSystemMetrics(SM_CXSCREEN));
#endif
                return std::shared_ptr<NumberValue>(0);
            });
        o->Set("width", widthFn);
    }

    static void InitHeight(const std::shared_ptr<ObjectValue> &o) {
        const auto heightFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &)-> ValuePtr {
#if defined(_WIN32)
                return std::make_shared<NumberValue>(GetSystemMetrics(SM_CYSCREEN));
#endif
                return std::shared_ptr<NumberValue>(0);
            });
        o->Set("width", heightFn);
    }

    static void InitWorkArea(const std::shared_ptr<ObjectValue> &o) {
        const auto workAreaFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &)-> ValuePtr {
#if defined(_WIN32)
                RECT rc;
                SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
                auto res = std::make_shared<ObjectValue>();
                res->Set("width", std::make_shared<NumberValue>(rc.right - rc.left));
                res->Set("height", std::make_shared<NumberValue>(rc.bottom - rc.top));
                return res;
#endif
                return std::shared_ptr<NumberValue>(0);
            });
        o->Set("workArea", workAreaFn);
    }

    static void InitCount(const std::shared_ptr<ObjectValue> &o) {
        const auto countFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &)-> ValuePtr {
#if defined(_WIN32)
                return std::make_shared<NumberValue>(GetSystemMetrics(SM_CMONITORS));
#endif
                return std::shared_ptr<NumberValue>(0);
            });
        o->Set("count", countFn);
    }

    static void InitRate(const std::shared_ptr<ObjectValue> &o) {
        const auto rateFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &)-> ValuePtr {
#if defined(_WIN32)
                DEVMODEW dm;
                dm.dmSize = sizeof(dm);
                EnumDisplaySettingsW(nullptr, ENUM_CURRENT_SETTINGS, &dm);
                return std::make_shared<NumberValue>(dm.dmDisplayFrequency);
#endif
                return std::shared_ptr<NumberValue>(0);
            });
        o->Set("rate", rateFn);
    }

    static void InitBpp(const std::shared_ptr<ObjectValue> &o) {
        const auto bppFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &)-> ValuePtr {
#if defined(_WIN32)
                const HDC hdc = GetDC(nullptr);
                int bpp = GetDeviceCaps(hdc, BITSPIXEL);
                ReleaseDC(nullptr, hdc);
                return std::make_shared<NumberValue>(bpp);
#endif
                return std::shared_ptr<NumberValue>(0);
            });
        o->Set("bpp", bppFn);
    }

public:
    static ValuePtr CreateScreenModule() {
        auto sc = std::make_shared<ObjectValue>();
        InitWidth(sc);
        InitHeight(sc);
        InitWorkArea(sc);
        InitCount(sc);
        InitRate(sc);
        InitBpp(sc);
        return sc;
    }
};


#endif //BXSCRIPT_MOUSEMODULE_H
