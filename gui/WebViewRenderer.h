/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/27
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    WebViewRenderer
 */
#ifndef BXSCRIPT_WEBVIEWRENDERER_H
#define BXSCRIPT_WEBVIEWRENDERER_H

#include <uxtheme.h>

#include "evaluator/Value.h"
#include "webview/webview.h"
#include "windows.h"
#include <dwmapi.h>


class WebViewRenderer {
    inline static std::unique_ptr<webview::webview> instance = nullptr;

    static void ReMountNativeFn(std::shared_ptr<ObjectValue> obj, const HWND hwnd) {
        obj->Set("doMin", std::make_shared<NativeFunctionValue>([hwnd,obj](const std::vector<ValuePtr> &) {
            doMin(hwnd);
            return obj;
        }));
        obj->Set("doMax", std::make_shared<NativeFunctionValue>([hwnd,obj](const std::vector<ValuePtr> &) {
            doMax(hwnd);
            return obj;
        }));
        obj->Set("doCap", std::make_shared<NativeFunctionValue>([hwnd,obj](const std::vector<ValuePtr> &) {
            doCap(hwnd);
            return obj;
        }));
    }

public:
    static void Run(std::shared_ptr<ObjectValue> o) {
        const std::string title = o->Get("_title") && o->Get("_title")->type != ValueType::NULL_TYPE
                                      ? o->Get("_title")->ToString()
                                      : "BxScriptWebView";
        const int width = o->Get("_width") && o->Get("_width")->type == ValueType::NUMBER
                              ? static_cast<int>(std::static_pointer_cast<NumberValue>(o->Get("_width"))->Value)
                              : 800;
        const int height = o->Get("_height") && o->Get("_height")->type == ValueType::NUMBER
                               ? static_cast<int>(std::static_pointer_cast<NumberValue>(o->Get("_height"))->Value)
                               : 600;
        const std::string html = o->Get("_html") && o->Get("_html")->type != ValueType::NULL_TYPE
                                     ? o->Get("_html")->ToString()
                                     : "<H1 style='text-align:center'>bxscript webview demo</H1>";
        bool debug = o->Get("_debug") && o->Get("_debug")->type == ValueType::BOOL
                         ? static_cast<int>(std::static_pointer_cast<BoolValue>(o->Get("_debug"))->Value)
                         : false;
        const bool transparent = o->Get("_transparent") && o->Get("_transparent")->type == ValueType::BOOL
                                     ? static_cast<int>(std::static_pointer_cast<BoolValue>(o->Get("_transparent"))->Value)
                                     : false;
        const auto methods = o->Get("_methods");
        instance = std::make_unique<webview::webview>(debug, nullptr);
        if (methods->type == ValueType::OBJECT) {
            const auto objMethods = std::static_pointer_cast<ObjectValue>(o->Get("_methods"));
            for (const auto &[k,v]: objMethods->Properties) {
                if (v->type == ValueType::FUNCTION) {
                    instance->bind(k, [v](const std::string &arg)-> std::string {
                        const auto r = Interpreter::CallFunction(v, {std::make_shared<StringValue>(arg)});
                        return r->ToString();
                    });
                }
            }
        }
        instance->set_title(title);
        instance->set_size(width, height, WEBVIEW_HINT_NONE);
        instance->set_html(html);
        if (transparent) {
            setBackgroundColor(static_cast<HWND>(instance->window().value()),
                               static_cast<ICoreWebView2Controller *>(instance->browser_controller().value()), transparent);
        }
        setCenter(static_cast<HWND>(instance->window().value()));
        ReMountNativeFn(o, static_cast<HWND>(instance->window().value()));
    }

    static void setCenter(HWND hwnd) {
        WINDOWINFO wi = {sizeof(WINDOWINFO)};
        GetWindowInfo(hwnd, &wi);
        const bool frameless = (wi.dwStyle & WS_POPUP) != 0;

        const HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = {sizeof(MONITORINFO)};
        GetMonitorInfoW(hMonitor, &mi);

        auto [left, top, right, bottom] = mi.rcWork;
        const int screenMiddleW = left + (right - left) / 2;
        const int screenMiddleH = top + (bottom - top) / 2;

        RECT winRect;
        if (!frameless) GetWindowRect(hwnd, &winRect);
        else GetClientRect(hwnd, &winRect);

        const int winWidth = winRect.right - winRect.left;
        const int winHeight = winRect.bottom - winRect.top;
        const int windowX = screenMiddleW - (winWidth / 2);
        const int windowY = screenMiddleH - (winHeight / 2);

        SetWindowPos(hwnd, HWND_TOP, windowX, windowY, winWidth, winHeight, SWP_NOSIZE);
    }

    static void setBackgroundColor(HWND hwnd, ICoreWebView2Controller *icc, bool isTransparent) {
        if (!isTransparent) return;
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        SetWindowLong(hwnd, GWL_STYLE, (style & ~WS_CAPTION & ~WS_THICKFRAME) | WS_POPUP);
        MARGINS margins = {-1};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
        ICoreWebView2Controller2 *pController2 = nullptr;
        if (SUCCEEDED(icc->QueryInterface(IID_ICoreWebView2Controller2, reinterpret_cast<void **>(&pController2)))) {
            COREWEBVIEW2_COLOR transparentColor = {0, 0, 0, 0};
            pController2->put_DefaultBackgroundColor(transparentColor);
            pController2->Release();
        }
        SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(GetStockObject(NULL_BRUSH)));
        // 强制重绘
        ShowWindow(hwnd, SW_MINIMIZE);
        ShowWindow(hwnd, SW_RESTORE);
    }
};


#endif //BXSCRIPT_WEBVIEWRENDERER_H
