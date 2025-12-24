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
#ifndef BXSCRIPT_GUIMODULE_H
#define BXSCRIPT_GUIMODULE_H

#include "../evaluator/Value.h"
#include "common/ColorKit.h"

class GuiModule {
    static ValuePtr CreateWidget(const std::shared_ptr<ObjectValue> &winObj, const std::string &type, const std::vector<ValuePtr> &args) {
        if (args.empty()) return std::make_shared<NullValue>();
        auto widget = std::make_shared<ObjectValue>();
        widget->Set("_type", std::make_shared<StringValue>(type));
        const std::string id = args[0]->ToString();
        widget->Set("id", args[0]);
        winObj->Get("refs")->Set(id, widget);
        InjectLayoutMethods(widget);
        if (type == "form" || type == "group") {
            InjectContainerMethods(widget);
        }
        return widget;
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

    static void InjectContainerMethods(const std::shared_ptr<ObjectValue> &widget) {
        std::weak_ptr weak_w = widget;
        // center()
        auto const centerFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &) -> ValuePtr {
                auto self = weak_w.lock();
                if (self) self->Set("center", std::make_shared<BoolValue>(true));
                return self;
            }
        );
        widget->Set("center", centerFn);

        // add
        auto const fn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &addArgs) -> ValuePtr {
                auto self = weak_w.lock();
                if (!self) return std::make_shared<NullValue>();
                const auto childrenVal = self->Get("children");
                std::shared_ptr<ArrayValue> children;
                if (!childrenVal) {
                    children = std::make_shared<ArrayValue>(std::vector<ValuePtr>{});
                    self->Set("children", children);
                } else {
                    children = std::static_pointer_cast<ArrayValue>(childrenVal);
                }
                if (!addArgs.empty()) {
                    if (addArgs[0]->type == ValueType::ARRAY) {
                        const auto arr = std::static_pointer_cast<ArrayValue>(addArgs[0]);
                        children->Elements.insert(children->Elements.end(), arr->Elements.begin(), arr->Elements.end());
                    } else if (addArgs[0]->type == ValueType::OBJECT) {
                        children->Elements.push_back(addArgs[0]);
                    }
                }
                return self;
            }
        );
        widget->Set("add", fn);
    }

    static void InjectLayoutMethods(const std::shared_ptr<ObjectValue> &widget) {
        // 托底，保证属性健全, 不考虑内存，如果下方覆盖了，则计数归0
        widget->Set("x", std::make_shared<NumberValue>(0));
        widget->Set("y", std::make_shared<NumberValue>(0));
        widget->Set("fontSize", std::make_shared<NumberValue>(16));
        widget->Set("visible", std::make_shared<BoolValue>(true));
        widget->Set("disable", std::make_shared<BoolValue>(false));
        widget->Set("align", std::make_shared<StringValue>("left"));
        std::weak_ptr weak_w = widget;
        // pos(x, y)
        const auto posFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (!self || args.empty()) return self;
                if (args[0]->type == ValueType::OBJECT) {
                    auto const x = args[0]->Get("x");
                    if (x && x->type == ValueType::NUMBER) {
                        self->Set("x", args[0]->Get("x"));
                    }
                    auto const y = args[0]->Get("y");
                    if (y && y->type == ValueType::NUMBER) {
                        self->Set("y", args[0]->Get("y"));
                    }
                } else if (args.size() >= 2 && args[0]->type == ValueType::NUMBER && args[1]->type == ValueType::NUMBER) {
                    self->Set("x", args[0]);
                    self->Set("y", args[1]);
                }
                return self;
            }
        );
        widget->Set("pos", posFn);

        // size(w, h)
        auto const sizeFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (self) {
                    if (args[0]->type == ValueType::OBJECT) {
                        if (args[0]->Get("width") && args[0]->Get("width")->type == ValueType::NUMBER) {
                            self->Set("width", args[0]->Get("width"));
                        }
                        if (args[0]->Get("height") && args[0]->Get("height")->type == ValueType::NUMBER) {
                            self->Set("height", args[0]->Get("height"));
                        }
                    } else if (self && args.size() >= 2 && args[0]->type == ValueType::NUMBER && args[1]->type == ValueType::NUMBER) {
                        self->Set("width", args[0]);
                        self->Set("height", args[1]);
                    }
                }
                return self;
            });
        widget->Set("size", sizeFn);

        // fontSize
        auto const fontSizeFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (!args.empty() && args[0]->type == ValueType::NUMBER) {
                    if (self) self->Set("fontSize", std::move(std::static_pointer_cast<NumberValue>(args[0])));
                }
                return self;
            });
        widget->Set("fontSize", fontSizeFn);

        // fontColor
        auto const fontColorFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                // ArgsColorToObject 有缺省托底
                if (self) self->Set("fontColor", ArgsColorToObject(args));
                return self;
            });
        widget->Set("fontColor", fontColorFn);

        // text
        auto const textFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                std::string str{};
                if (!args.empty()) {
                    str = args[0]->ToString();
                }
                self->Set("text", std::make_shared<StringValue>(str));
                return self;
            });
        widget->Set("text", textFn);

        // click
        auto const clickFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (!args.empty() && args[0]->type == ValueType::FUNCTION) {
                    if (self) self->Set("click", args[0]);
                }
                // 事件不托底
                return self;
            });
        widget->Set("onClick", clickFn);

        // backgroundColor
        auto const bgColorFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (self) self->Set("backgroundColor", ArgsColorToObject(args));
                return self;
            });
        widget->Set("backgroundColor", bgColorFn);

        // border
        auto const borderFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (!args.empty()) {
                    if (args[0]->type == ValueType::NUMBER) {
                        self->Set("borderWidth", args[0]);
                        if (args.size() > 1) {
                            self->Set("borderColor", ArgsColorToObject(std::vector(args.begin() + 1, args.end())));
                        }
                    } else if (args[0]->type == ValueType::OBJECT) {
                        auto const attrs = std::static_pointer_cast<ObjectValue>(args[0]);
                        auto const borderWidth = attrs->Get("width");
                        auto const borderColor = attrs->Get("color");
                        if (borderWidth->type == ValueType::NUMBER) {
                            self->Set("borderWidth", borderWidth);
                        }
                        self->Set("borderColor", ArgsColorToObject(std::vector{borderColor}));
                    }
                }
                return self;
            });
        widget->Set("border", borderFn);

        // hidden
        auto const hideFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &) -> ValuePtr {
                auto const self = weak_w.lock();
                if (self) self->Set("visible", std::make_shared<BoolValue>(false));
                return self;
            });
        widget->Set("hide", hideFn);

        // show
        auto const showFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &) -> ValuePtr {
                auto const self = weak_w.lock();
                if (self) self->Set("visible", std::make_shared<BoolValue>(true));
                return self;
            });
        widget->Set("show", showFn);

        // disable
        auto const disableFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                auto const disable = std::make_shared<BoolValue>(true);
                if (!args.empty() && args[0]->type == ValueType::BOOL) {
                    disable->Value = std::static_pointer_cast<BoolValue>(args[0])->Value;
                }
                if (self) self->Set("disable", disable);
                return self;
            });
        widget->Set("disable", disableFn);

        // align
        auto const alignFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (!args.empty() && args[0]->type == ValueType::STRING) {
                    auto const alignStr = args[0]->ToString();
                    if (alignStr == "left" || alignStr == "right" || alignStr == "center") {
                        if (self) self->Set("align", args[0]);
                    }
                }
                return self;
            });
        widget->Set("align", alignFn);

        // change
        auto const changeFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (!args.empty() && args[0]->type == ValueType::FUNCTION) {
                    if (self) self->Set("change", args[0]);
                }
                return self;
            });
        widget->Set("onChange", changeFn);

        // hover
        auto const hoverFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (!args.empty() && args[0]->type == ValueType::FUNCTION) {
                    if (self) self->Set("hover", args[0]);
                }
                return self;
            });
        widget->Set("onHover", hoverFn);

        // padding
        auto const paddingFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (!args.empty()) {
                    if (args.size() == 2) {
                        self->Set("padding-top", args[0]);
                        self->Set("padding-bottom", args[0]);
                        self->Set("padding-left", args[1]);
                        self->Set("padding-right", args[1]);
                    } else if (args.size() == 4) {
                        self->Set("padding-top", args[0]);
                        self->Set("padding-bottom", args[2]);
                        self->Set("padding-left", args[3]);
                        self->Set("padding-right", args[1]);
                    } else if (args.size() == 1) {
                        if (args[0]->type == ValueType::OBJECT) {
                            self->Set("padding-top", args[0]->Get("top"));
                            self->Set("padding-bottom", args[0]->Get("bottom"));
                            self->Set("padding-left", args[0]->Get("left"));
                            self->Set("padding-right", args[0]->Get("right"));
                        } else if (args[0]->type == ValueType::NUMBER) {
                            self->Set("padding-top", args[0]);
                            self->Set("padding-bottom", args[0]);
                            self->Set("padding-left", args[0]);
                            self->Set("padding-right", args[0]);
                        }
                    }
                }
                return self;
            });
        widget->Set("padding", paddingFn);
    }

    static void InitForm(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [o](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto form = CreateWidget(o, "form", args);
                GlobalForms.push_back(form);
                return form;
            }
        );
        o->Set("form", fn);
    }

    static void InitControls(std::shared_ptr<ObjectValue> &o) {
        auto makeFactory = [o](const std::string &type) {
            std::weak_ptr weak_o(o);
            return std::make_shared<NativeFunctionValue>(
                [weak_o, type](const std::vector<ValuePtr> &args) -> ValuePtr {
                    const auto self = weak_o.lock();
                    return CreateWidget(self, type, args);
                }
            );
        };
        o->Set("button", makeFactory("button"));
        o->Set("label", makeFactory("label"));
        o->Set("input", makeFactory("input"));
        o->Set("password", makeFactory("password"));
        o->Set("group", makeFactory("group"));
        o->Set("checkbox", makeFactory("checkbox"));
        o->Set("slider", makeFactory("slider"));
        o->Set("progress", makeFactory("progress"));
        o->Set("image", makeFactory("image"));
    }

public:
    static ValuePtr CreateGuiModule() {
        auto win = std::make_shared<ObjectValue>();
        win -> Set("refs", std::make_shared<ObjectValue>());
        InitForm(win);
        InitControls(win);
        return win;
    }

    inline static std::vector<ValuePtr> GlobalForms{};
};

#endif //BXSCRIPT_GUIMODULE_H
