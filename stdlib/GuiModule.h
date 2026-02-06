/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/22
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    GuiModule
 */

/**
 * Architecture Note / 架构说明:
 *
 * [English]
 * Control methods (like .text(), .pos()) capture the widget via std::shared_ptr (Strong Reference).
 * This intentionally creates a circular reference (Widget -> Function -> Closure -> Widget),
 * preventing automatic destruction via reference counting.
 *
 * Reason: To support fluent/chaining APIs (e.g., win.label().text().pos()) where temporary
 * objects must survive between method calls during expression evaluation.
 *
 * Impact: Since GUI controls typically live for the duration of the window/application,
 * granular garbage collection is not a priority compared to API usability.
 * Resources are reclaimed by the OS upon process termination.
 *
 * [中文]
 * 控件的方法（如 .text(), .pos()）通过 std::shared_ptr（强引用）捕获控件对象。
 * 这会有意地造成循环引用（控件 -> 函数 -> 闭包 -> 控件），从而阻止引用计数机制自动销毁对象。
 *
 * 原因：为了支持流式/链式 API 调用（例如 win.label().text().pos()）。在解释器求值过程中，
 * 临时对象必须在连续的方法调用之间保持存活。
 *
 * 影响：鉴于 GUI 控件的生命周期通常伴随整个窗口或应用程序，相比于严格的垃圾回收，
 * API 的易用性优先级更高。所有资源将在进程终止时由操作系统回收。
 */
#ifndef BXSCRIPT_GUIMODULE_H
#define BXSCRIPT_GUIMODULE_H

#include <algorithm>
#include <unordered_set>

#include "../evaluator/Value.h"
#include "common/ColorKit.h"
#include "common/StringKit.h"
#include "gui/GuiRuntime.h"
#include "gui/windows/App.h"
#include "gui/windows/CommonDlgs.h"
#include "webview/webview.h"

static std::unordered_set<std::string> stdEvents{
    "click", "change", "mouseover", "mouseleave",
    "mousemove", "mousedown", "mouseup", "close",
    "create", "resize", "paint", "keyup",
    "keydown", "dbclick", "focus", "blur",
    "scroll"
};

class GuiModuleKit {
public:
    static bool IsValidEvent(const std::string &alias) {
        return stdEvents.find(alias) != stdEvents.end();
    }

    static ValuePtr ArgsColorToObject(const std::vector<ValuePtr> &args) {
        auto fc = std::make_shared<ObjectValue>();
        fc->Set("R", std::make_shared<NumberValue>(45));
        fc->Set("G", std::make_shared<NumberValue>(45));
        fc->Set("B", std::make_shared<NumberValue>(48));
        fc->Set("A", std::make_shared<NumberValue>(255));
        if (!args.empty()) {
            if (args[0]->type == ValueType::OBJECT) {
                const auto obj = std::static_pointer_cast<ObjectValue>(args[0]);
                if (obj->Get("R") && obj->Get("R")->type == ValueType::NUMBER) {
                    if (obj->Get("G") && obj->Get("G")->type == ValueType::NUMBER) {
                        if (obj->Get("B") && obj->Get("B")->type == ValueType::NUMBER) {
                            if (obj->Get("A") && obj->Get("A")->type == ValueType::NUMBER) {
                                return obj;
                            }
                            obj->Set("A", std::make_shared<NumberValue>(255));
                            return obj;
                        }
                    }
                }
            } else if (args[0]->type == ValueType::STRING) {
                const auto str = std::static_pointer_cast<StringValue>(args[0])->Value;
                const auto [r, g, b, a] = ColorKit::HexToRgba(str.c_str());
                fc->Set("R", std::make_shared<NumberValue>(r));
                fc->Set("G", std::make_shared<NumberValue>(g));
                fc->Set("B", std::make_shared<NumberValue>(b));
                fc->Set("A", std::make_shared<NumberValue>(a));
            }
        }
        return std::move(fc);
    }

    static void CollectRefs(const std::shared_ptr<ObjectValue> &rootForm, const std::shared_ptr<ObjectValue> &node) {
        if (auto const idVal = node->Get("id"); idVal->type == ValueType::STRING) {
            if (const std::string id = idVal->ToString(); !id.empty()) {
                if (auto const refs = rootForm->Get("refs"); refs->type == ValueType::OBJECT) {
                    std::static_pointer_cast<ObjectValue>(refs)->Set(id, node);
                }
            }
        }
        if (auto const childrenVal = node->Get("children"); childrenVal->type == ValueType::ARRAY) {
            auto const arr = std::static_pointer_cast<ArrayValue>(childrenVal);
            for (const auto &child: arr->Elements) {
                if (child->type == ValueType::OBJECT) {
                    CollectRefs(rootForm, std::static_pointer_cast<ObjectValue>(child));
                }
            }
        }
    }

    template<typename... Args>
    static void AddAccessor(const std::shared_ptr<ObjectValue> &widget, const std::string &funcName, Args... attributes) {
        std::vector<std::string> keys = {attributes...};
        const auto accessor = std::make_shared<NativeFunctionValue>(
            [widget, keys](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) {
                    if (keys.size() == 1) {
                        return widget->Get(keys[0]);
                    }
                    auto res = std::make_shared<ObjectValue>();
                    for (const auto &key: keys) {
                        std::string outKey = (key.length() > 1 && key[0] == '_') ? key.substr(1) : key;
                        res->Set(outKey, widget->Get(key));
                    }
                    return res;
                }
                // 对象属性赋值
                if (args.size() == 1 && args[0]->type == ValueType::OBJECT) {
                    const auto inputObj = std::static_pointer_cast<ObjectValue>(args[0]);
                    for (const auto &key: keys) {
                        std::string inKey = (key.length() > 1 && key[0] == '_') ? key.substr(1) : key;
                        if (const auto val = inputObj->Get(inKey); val && val->type != ValueType::NULL_TYPE) {
                            widget->Set(key, val);
                        }
                    }
                } else if (args.size() == keys.size()) {
                    for (size_t i = 0; i < keys.size(); ++i) {
                        widget->Set(keys[i], args[i]);
                    }
                } else if (args.size() == 1 && keys.size() > 1) {
                    // padding(0) => padding4个方向
                    for (const auto &key: keys) {
                        widget->Set(key, args[0]);
                    }
                }
                return widget;
            }
        );
        widget->Set(funcName, accessor);
    }
};

class GuiModule {
    static ValuePtr CreateWidget(const std::string &type, const std::vector<ValuePtr> &args) {
        if (args.empty()) return std::make_shared<NullValue>();
        auto widget = std::make_shared<ObjectValue>();
        widget->Set("_type", std::make_shared<StringValue>(type));
        const std::string id = args[0]->ToString();
        widget->Set("id", args[0]);
        InjectLayoutMethods(widget);
        if (type == "form") {
            InjectContainerMethods(widget);
            InjectFormMethods(widget);
            widget->Set("refs", std::make_shared<ObjectValue>());
        }
        if (type == "group") {
            InjectContainerMethods(widget);
        }
        if (type == "image") {
            InjectImageMethods(widget);
        }
        if (type == "list" || type == "select") {
            InjectListMethods(widget);
        }
        return widget;
    }

    static void InjectImageMethods(std::shared_ptr<ObjectValue> &widget) {
        GuiModuleKit::AddAccessor(widget, "src", "_src");
    }

    static void InjectFormMethods(std::shared_ptr<ObjectValue> &widget) {
        GuiModuleKit::AddAccessor(widget, "icon", "_icon");
        auto const menuFn = std::make_shared<NativeFunctionValue>(
            [widget](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (!args.empty() && args[0]->type == ValueType::ARRAY) {
                    auto const menuBar = std::make_shared<ObjectValue>();
                    menuBar->Set("_type", std::make_shared<StringValue>("menubar"));
                    menuBar->Set("children", args[0]);
                    widget->Set("_menu", menuBar);
                }
                return widget;
            });
        widget->Set("menu", menuFn);
        auto const trayFn = std::make_shared<NativeFunctionValue>(
            [widget](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::OBJECT) return widget;
                const auto conf = std::static_pointer_cast<ObjectValue>(args[0]);
                widget->Set("_trayConf", conf);
                return widget;
            });
        widget->Set("doTray", trayFn);
        auto const doMinFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &) -> ValuePtr {
                throw RuntimeError("函数还未挂载或挂载失败");
            });
        widget->Set("doMin", doMinFn);
        auto const doMaxFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &) -> ValuePtr {
                throw RuntimeError("函数还未挂载或挂载失败");
            });
        widget->Set("doMax", doMaxFn);
        auto const doCapFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &) -> ValuePtr {
                throw RuntimeError("函数还未挂载或挂载失败");
            });
        widget->Set("doCap", doCapFn);
    }

    static void InjectContainerMethods(std::shared_ptr<ObjectValue> &widget) {
        auto const centerFn = std::make_shared<NativeFunctionValue>(
            [widget](const std::vector<ValuePtr> &) -> ValuePtr {
                widget->Set("_center", std::make_shared<BoolValue>(true));
                return widget;
            }
        );
        widget->Set("center", centerFn);

        // add
        auto const fn = std::make_shared<NativeFunctionValue>(
            [widget](const std::vector<ValuePtr> &addArgs) -> ValuePtr {
                const auto childrenVal = widget->Get("children");
                std::shared_ptr<ArrayValue> children;
                if (!childrenVal || childrenVal->type == ValueType::NULL_TYPE) {
                    children = std::make_shared<ArrayValue>(std::vector<ValuePtr>{});
                    widget->Set("children", children);
                } else {
                    children = std::static_pointer_cast<ArrayValue>(childrenVal);
                }
                const std::string type = widget->Get("_type")->ToString();
                if (!addArgs.empty()) {
                    if (addArgs[0]->type == ValueType::ARRAY) {
                        const auto arr = std::static_pointer_cast<ArrayValue>(addArgs[0]);
                        children->Elements.insert(children->Elements.end(), arr->Elements.begin(), arr->Elements.end());
                        if (type == "form") {
                            for (auto &child: arr->Elements) {
                                if (child->type == ValueType::OBJECT) {
                                    GuiModuleKit::CollectRefs(widget, std::static_pointer_cast<ObjectValue>(child));
                                }
                            }
                        }
                    } else if (addArgs[0]->type == ValueType::OBJECT) {
                        children->Elements.push_back(addArgs[0]);
                        if (type == "form") {
                            GuiModuleKit::CollectRefs(widget, std::static_pointer_cast<ObjectValue>(addArgs[0]));
                        }
                    }
                }
                return widget;
            }
        );
        widget->Set("add", fn);
    }

    static void InjectLayoutMethods(std::shared_ptr<ObjectValue> &widget) {
        // 托底，保证属性健全, 不考虑内存，如果下方覆盖了，则计数归0
        widget->Set("_x", std::make_shared<NumberValue>(0));
        widget->Set("_y", std::make_shared<NumberValue>(0));
        widget->Set("_fontSize", std::make_shared<NumberValue>(12));
        widget->Set("_visible", std::make_shared<BoolValue>(true));
        widget->Set("_disable", std::make_shared<BoolValue>(false));
        widget->Set("_align", std::make_shared<StringValue>("left"));
        widget->Set("_text", std::make_shared<StringValue>(""));
        widget->Set("_bgColor", std::make_shared<NullValue>());

        GuiModuleKit::AddAccessor(widget, "text", "_text");
        GuiModuleKit::AddAccessor(widget, "bgColor", "_bgColor");
        GuiModuleKit::AddAccessor(widget, "visible", "_visible");
        GuiModuleKit::AddAccessor(widget, "disable", "_disable");
        GuiModuleKit::AddAccessor(widget, "align", "_align");
        GuiModuleKit::AddAccessor(widget, "font", "_fontSize", "_fontFamily", "_fontColor", "_fontBlod");
        GuiModuleKit::AddAccessor(widget, "x", "_x");
        GuiModuleKit::AddAccessor(widget, "y", "_y");
        GuiModuleKit::AddAccessor(widget, "width", "_width");
        GuiModuleKit::AddAccessor(widget, "height", "_height");
        GuiModuleKit::AddAccessor(widget, "pos", "_x", "_y");
        GuiModuleKit::AddAccessor(widget, "size", "_width", "_height");

        // hidden
        auto const hideFn = std::make_shared<NativeFunctionValue>(
            [widget](const std::vector<ValuePtr> &) -> ValuePtr {
                if (!widget) return std::make_shared<NullValue>();
                widget->Set("_visible", std::make_shared<BoolValue>(false));
                return widget;
            });
        widget->Set("hide", hideFn);

        // show
        auto const showFn = std::make_shared<NativeFunctionValue>(
            [widget](const std::vector<ValuePtr> &) -> ValuePtr {
                if (!widget) return std::make_shared<NullValue>();
                widget->Set("_visible", std::make_shared<BoolValue>(true));
                return widget;
            });
        widget->Set("show", showFn);

        // event
        auto const eventFn = std::make_shared<NativeFunctionValue>(
            [widget](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (!widget) return std::make_shared<NullValue>();
                if (args.empty() || args.size() < 2) throw RuntimeError("参数错误: widget.on('event', function)");
                if (args[0]->type != ValueType::STRING || args[1]->type != ValueType::FUNCTION) {
                    throw RuntimeError("参数错误: widget.on('event', function)");
                }
                const std::string evtAlias = args[0]->ToString();
                if (!GuiModuleKit::IsValidEvent(evtAlias)) {
                    throw RuntimeError("参数错误: " + evtAlias + " 不支持");
                }
                widget->Set(evtAlias, args[1]);
                return widget;
            });
        widget->Set("on", eventFn);
    }

    static void InitItem(std::shared_ptr<ObjectValue> &o) {
        auto const itemFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto item = std::make_shared<ObjectValue>();
                if (args.empty()) return item;
                std::string text = args[0]->ToString();
                if (text == "-") {
                    item->Set("_type", std::make_shared<StringValue>("separator"));
                    return item;
                }
                item->Set("_type", std::make_shared<StringValue>("menuitem"));
                item->Set("text", std::make_shared<StringValue>(text));
                if (args.size() > 1) {
                    auto &second = args[1];
                    if (second->type == ValueType::ARRAY) {
                        item->Set("children", second);
                    } else if (second->type == ValueType::FUNCTION) {
                        item->Set("click", second);
                    }
                }
                return item;
            }
        );
        o->Set("item", itemFn);
    }

    static void InitForm(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto form = CreateWidget("form", args);
                GlobalForms.push_back(form);
                return form;
            }
        );
        o->Set("form", fn);
    }

    static void InitWebView(std::shared_ptr<ObjectValue> &o) {
        const auto webViewFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &) -> ValuePtr {
                auto const ow = std::make_shared<ObjectValue>();
                GuiModuleKit::AddAccessor(ow, "title", "_title");
                GuiModuleKit::AddAccessor(ow, "html", "_html");
                GuiModuleKit::AddAccessor(ow, "width", "_width");
                GuiModuleKit::AddAccessor(ow, "height", "_height");
                GuiModuleKit::AddAccessor(ow, "size", "_width", "_height");

                auto const bindFn = std::make_shared<NativeFunctionValue>(
                    [ow](const std::vector<ValuePtr> &args) -> ValuePtr {
                        auto const methods = ow->Get("_methods") && ow->Get("_methods")->type != ValueType::NULL_TYPE
                                                 ? ow->Get("_methods")
                                                 : std::make_shared<ObjectValue>();
                        if (args.empty() || args.size() < 2) throw RuntimeError("参数错误: widget.bind('event', function)");
                        if (args[0]->type != ValueType::STRING || args[1]->type != ValueType::FUNCTION) {
                            throw RuntimeError("参数错误: widget.bind('event', function)");
                        }
                        const std::string evtAlias = args[0]->ToString();
                        methods->Set(evtAlias, args[1]);
                        ow->Set("_methods", methods);
                        return ow;
                    });
                ow->Set("bind", bindFn);

                auto const transparentFn = std::make_shared<NativeFunctionValue>(
                    [ow](const std::vector<ValuePtr> &) -> ValuePtr {
                        ow->Set("_transparent", std::make_shared<BoolValue>(true));
                        return ow;
                    });
                ow->Set("transparent", transparentFn);

                auto const debugFn = std::make_shared<NativeFunctionValue>(
                    [ow](const std::vector<ValuePtr> &) -> ValuePtr {
                        ow->Set("_debug", std::make_shared<BoolValue>(true));
                        return ow;
                    });
                ow->Set("debug", debugFn);

                auto const trayFn = std::make_shared<NativeFunctionValue>(
                    [ow](const std::vector<ValuePtr> &args) -> ValuePtr {
                        if (args.empty()) return ow;
                        if (args.size() > 1) {
                            if (args[0]->type == ValueType::OBJECT) {
                                const auto conf = std::static_pointer_cast<ObjectValue>(args[0]);
                                ow->Set("_trayConf", conf);
                            }
                            const auto conf = std::make_shared<ObjectValue>();
                            conf->Set("icon", args[0]);
                            conf->Set("tip", args[1]);
                            ow->Set("_trayConf", conf);
                        }
                        return ow;
                    });
                ow->Set("doTray", trayFn);
                auto const doMinFn = std::make_shared<NativeFunctionValue>(
                    [](const std::vector<ValuePtr> &) -> ValuePtr {
                        throw RuntimeError("函数还未挂载或挂载失败");
                    });
                ow->Set("doMin", doMinFn);
                auto const doMaxFn = std::make_shared<NativeFunctionValue>(
                    [](const std::vector<ValuePtr> &) -> ValuePtr {
                        throw RuntimeError("函数还未挂载或挂载失败");
                    });
                ow->Set("doMax", doMaxFn);
                auto const doCapFn = std::make_shared<NativeFunctionValue>(
                    [](const std::vector<ValuePtr> &) -> ValuePtr {
                        throw RuntimeError("函数还未挂载或挂载失败");
                    });
                ow->Set("doCap", doCapFn);
                WebViewConfig = ow;
                return ow;
            });
        o->Set("webview", webViewFn);
    }

    static void InitControls(const std::shared_ptr<ObjectValue> &o) {
        auto makeFactory = [](const std::string &type) {
            return std::make_shared<NativeFunctionValue>(
                [type](const std::vector<ValuePtr> &args) -> ValuePtr {
                    return CreateWidget(type, args);
                }
            );
        };
        o->Set("button", makeFactory("button"));
        o->Set("label", makeFactory("label"));
        o->Set("input", makeFactory("input"));
        o->Set("password", makeFactory("password"));
        o->Set("multiline", makeFactory("multiline"));
        o->Set("checkbox", makeFactory("checkbox"));
        o->Set("radio", makeFactory("radio"));
        o->Set("select", makeFactory("select"));
        o->Set("slider", makeFactory("slider"));
        o->Set("progress", makeFactory("progress"));
        o->Set("image", makeFactory("image"));
        o->Set("list", makeFactory("list"));
        o->Set("group", makeFactory("group"));
        o->Set("panel", makeFactory("panel"));
    }

    static void InitMessageLoop(std::shared_ptr<ObjectValue> &o) {
        const auto loopFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &) -> ValuePtr {
                if (!WebViewConfig && GlobalForms.size() > 0) {
                    GuiRuntime::Run(GlobalForms);
                    GlobalForms.clear();
                } else {
                    GuiRuntime::RunWebView(WebViewConfig);
                }
                return std::make_shared<NullValue>();
            }
        );
        o->Set("loop", loopFn);
    }

    static void InjectListMethods(const std::shared_ptr<ObjectValue> &widget) {
        widget->Set("_items", std::make_shared<ArrayValue>(std::vector<ValuePtr>{}));
        widget->Set("_heads", std::make_shared<ArrayValue>(std::vector<ValuePtr>{}));
        GuiModuleKit::AddAccessor(widget, "items", "_items");
        GuiModuleKit::AddAccessor(widget, "heads", "_heads");
    }

    static void InitExit(std::shared_ptr<ObjectValue> &o) {
        auto const exitFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                int code = 0;
                if (!args.empty() && args[0]->type == ValueType::NUMBER) {
                    code = static_cast<int>(std::static_pointer_cast<NumberValue>(args[0])->Value);
                }
                GlobalForms.clear();
                App::Exit(code);
                return std::make_shared<NullValue>();
            });
        o->Set("doExit", exitFn);
    }

public:
    static ValuePtr CreateGuiModule() {
        auto win = std::make_shared<ObjectValue>();
        InitForm(win);
        InitWebView(win);
        InitItem(win);
        InitControls(win);
        InitMessageLoop(win);
        InitExit(win);
        return win;
    }

    inline static std::vector<ValuePtr> GlobalForms{};
    inline static std::shared_ptr<ObjectValue> WebViewConfig{};
};

#endif //BXSCRIPT_GUIMODULE_H
