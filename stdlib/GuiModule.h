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

class GuiModule {
    static ValuePtr CreateWidget(const std::shared_ptr<ObjectValue> &winObj, const std::string &type, const std::vector<ValuePtr> &args) {
        if (args.empty()) return std::make_shared<NullValue>();
        auto widget = std::make_shared<ObjectValue>();
        widget->Set("_type", std::make_shared<StringValue>(type));
        const std::string id = args[0]->ToString();
        widget->Set("id", args[0]);
        if (args.size() > 1) widget->Set("text", args[1]);
        if (args.size() > 2) widget->Set("width", args[2]);
        if (args.size() > 3) widget->Set("height", args[3]);
        if (args.size() > 4) widget->Set("x", args[4]);
        if (args.size() > 5) widget->Set("y", args[5]);
        if (args.size() > 6) widget->Set("fontSize", args[6]);
        if (args.size() > 7) widget->Set("fontColor", args[7]);
        std::weak_ptr weak_widget = widget;
        auto const fn = std::make_shared<NativeFunctionValue>(
            [weak_widget](const std::vector<ValuePtr> &addArgs) -> ValuePtr {
                auto self = weak_widget.lock();
                if (!self) return std::make_shared<NullValue>();
                const auto childrenVal = self->Get("children");
                std::shared_ptr<ArrayValue> children;
                if (childrenVal->type != ValueType::ARRAY) {
                    children = std::make_shared<ArrayValue>(std::vector<ValuePtr>{});
                    self->Set("children", children);
                } else {
                    children = std::static_pointer_cast<ArrayValue>(childrenVal);
                }
                if (!addArgs.empty()) {
                    if (addArgs[0]->type == ValueType::ARRAY) {
                        const auto arr = std::static_pointer_cast<ArrayValue>(addArgs[0]);
                        children->Elements.insert(children->Elements.end(), arr->Elements.begin(), arr->Elements.end());
                    } else {
                        children->Elements.push_back(addArgs[0]);
                    }
                }
                return self;
            }
        );
        widget->Set("add", fn);
        winObj->Set(id, widget);
        return widget;
    }

    static void InjectWidgetMethods(const std::shared_ptr<ObjectValue> &widget) {
        // pos(x, y)
        std::weak_ptr weak_w = widget;
        const auto posFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (self && args.size() >= 2) {
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
                if (self && args.size() >= 2) {
                    self->Set("width", args[0]);
                    self->Set("height", args[1]);
                }
                return self;
            });
        widget->Set("size", sizeFn);

        // center()
        auto const centerFn = std::make_shared<NativeFunctionValue>(
            [weak_w](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto self = weak_w.lock();
                if (self) self->Set("center", std::make_shared<BoolValue>(true));
                return self;
            }
        );
        widget->Set("center", centerFn);
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
            return std::make_shared<NativeFunctionValue>(
                [o, type](const std::vector<ValuePtr> &args) -> ValuePtr {
                    return CreateWidget(o, type, args);
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
        InitForm(win);
        InitControls(win);
        return win;
    }

    inline static std::vector<ValuePtr> GlobalForms{};
};

#endif //BXSCRIPT_GUIMODULE_H
