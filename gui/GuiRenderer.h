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
 * @brief    GuiRenderer
 */
#ifndef BXSCRIPT_GUIRENDERER_H
#define BXSCRIPT_GUIRENDERER_H

#include <cstring>
#include "../stdlib/GuiModule.h"

class GuiRenderer {
    static float GetFloat(const ValuePtr &obj, const std::string &key, const float def = 0) {
        if (const auto v = obj->Get(key); v->type == ValueType::NUMBER) {
            return static_cast<float>(std::static_pointer_cast<NumberValue>(v)->Value);
        }
        return def;
    }

    static std::string GetString(const ValuePtr &obj, const std::string &key, const std::string &def = "") {
        if (const auto v = obj->Get(key); v->type == ValueType::STRING) {
            return std::static_pointer_cast<StringValue>(v)->Value;
        }
        return def;
    }

public:
    static void RenderWidget(nk_context *ctx, const ValuePtr& widget) {
        if (widget->type != ValueType::OBJECT) return;
        const auto obj = std::static_pointer_cast<ObjectValue>(widget);
        const std::string type = GetString(obj, "_type");
        auto const x = GetFloat(obj, "x");
        auto const y = GetFloat(obj, "y");
        auto const w = GetFloat(obj, "width", 100);
        auto const h = GetFloat(obj, "height", 30);
        const std::string text = GetString(obj, "text");
        nk_layout_space_push(ctx, nk_rect(x, y, w, h));
        if (type == "button") {
            if (nk_button_label(ctx, text.c_str())) {
                auto callback = obj->Get("onclick");
                if (callback->type == ValueType::FUNCTION) {
                    Interpreter::CallFunction(callback, {});
                }
            }
        }
        else if (type == "label") {
            // nk_label_colored(ctx, text.c_str(), NK_TEXT_LEFT, color); rgba
            nk_label(ctx, text.c_str(), NK_TEXT_LEFT);
        }
        else if (type == "input" || type == "password") {
            char buffer[256];
            const std::string currentText = GetString(obj, "value");
            strncpy(buffer, currentText.c_str(), 255);
            int len = currentText.length();
            nk_flags flags = NK_EDIT_FIELD;
            if (type == "password") flags |= NK_EDIT_SIG_ENTER;
            nk_edit_string(ctx, flags, buffer, &len, 255, nk_filter_default);
            std::string newText(buffer, len);
            if (newText != currentText) {
                obj->Set("value", std::make_shared<StringValue>(newText));
                auto callback = obj->Get("onchange");
                if (callback->type == ValueType::FUNCTION) {
                    Interpreter::CallFunction(callback, {std::make_shared<StringValue>(newText)});
                }
            }
        }
        else if (type == "group") {
            if (nk_group_begin(ctx, text.c_str(), NK_WINDOW_NO_SCROLLBAR)) {
                nk_layout_space_begin(ctx, NK_STATIC, h, INT_MAX);
                auto children = obj->Get("children");
                if (children->type == ValueType::ARRAY) {
                    const auto arr = std::static_pointer_cast<ArrayValue>(children);
                    for (const auto &child: arr->Elements) {
                        RenderWidget(ctx, child);
                    }
                }
                nk_layout_space_end(ctx);
                nk_group_end(ctx);
            }
        }
    }

    static void RenderContentOnly(nk_context *ctx) {
        if (GuiModule::GlobalForms.empty()) return;
        const auto form = std::static_pointer_cast<ObjectValue>(GuiModule::GlobalForms[0]);
        auto children = form->Get("children");
        if (children->type == ValueType::ARRAY) {
            const auto arr = std::static_pointer_cast<ArrayValue>(children);
            for (auto& child : arr->Elements) {
                RenderWidget(ctx, child);
            }
        }
    }

    static void RenderAll(nk_context *ctx) {
        for (auto &formVal: GuiModule::GlobalForms) {
            auto form = std::static_pointer_cast<ObjectValue>(formVal);
            std::string title = GetString(form, "text");
            if (nk_group_begin(ctx, "BxScriptContent", NK_WINDOW_BORDER)) {
                RenderContentOnly(ctx);
            }
            nk_group_end(ctx);
        }
    }
};


#endif //BXSCRIPT_GUIRENDERER_H
