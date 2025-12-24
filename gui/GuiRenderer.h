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

    static bool GetBool(const std::shared_ptr<ObjectValue> &obj, const std::string &key, bool def = false) {
        if (const auto val = obj->Get(key); val && val->type == ValueType::BOOL) {
            return std::static_pointer_cast<BoolValue>(val)->Value;
        }
        return def;
    }

    static nk_color GetColor(const std::shared_ptr<ObjectValue> &obj, const std::string &key, const nk_color def) {
        if (const auto val = obj->Get(key); val && val->type == ValueType::OBJECT) {
            const auto cObj = std::static_pointer_cast<ObjectValue>(val);
            return nk_rgba(
                static_cast<int>(GetFloat(cObj, "R", def.r)),
                static_cast<int>(GetFloat(cObj, "G", def.g)),
                static_cast<int>(GetFloat(cObj, "B", def.b)),
                static_cast<int>(GetFloat(cObj, "A", def.a))
            );
        }
        return def;
    }

    static nk_flags GetAlign(const std::shared_ptr<ObjectValue> &obj) {
        const std::string align = GetString(obj, "align", "left");
        if (align == "center") return NK_TEXT_CENTERED;
        if (align == "right") return NK_TEXT_RIGHT;
        return NK_TEXT_LEFT;
    }

    static void TriggerEvent(const std::shared_ptr<ObjectValue> &obj, const std::string &eventName, const std::vector<ValuePtr> &args = {}) {
        if (const auto cb = obj->Get(eventName); cb && cb->type == ValueType::FUNCTION) {
            Interpreter::CallFunction(cb, args);
        }
    }

public:
    static void RenderWidget(struct nk_context *ctx, ValuePtr widgetVal) {
        if (!widgetVal || widgetVal->type != ValueType::OBJECT) return;
        const auto obj = std::static_pointer_cast<ObjectValue>(widgetVal);
        if (!GetBool(obj, "visible", true)) return;
        const bool disabled = GetBool(obj, "disable", false);
        const float x = GetFloat(obj, "x", 0);
        const float y = GetFloat(obj, "y", 0);
        const float w = GetFloat(obj, "width", 100);
        const float h = GetFloat(obj, "height", 30);
        nk_layout_space_push(ctx, nk_rect(x, y, w, h));
        const std::string type = GetString(obj, "_type");
        const std::string text = GetString(obj, "text");
        int colorPushed = 0;
        if (disabled) {
            const nk_color gray = nk_rgb(100, 100, 100);
            nk_style_push_color(ctx, &ctx->style.text.color, gray);
            nk_style_push_color(ctx, &ctx->style.button.normal.data.color, nk_rgb(40, 40, 40));
            nk_style_push_color(ctx, &ctx->style.button.hover.data.color, nk_rgb(40, 40, 40));
            nk_style_push_color(ctx, &ctx->style.button.active.data.color, nk_rgb(40, 40, 40));
            colorPushed += 4;
        } else {
            auto fontColorVal = obj->Get("fontColor");
            if (fontColorVal && fontColorVal->type == ValueType::OBJECT) {
                nk_style_push_color(ctx, &ctx->style.text.color, GetColor(obj, "fontColor", nk_rgb(255, 255, 255)));
                colorPushed++;
            }
            if (type == "button") {
                ctx->style.button.text_normal = GetColor(obj, "fontColor", nk_rgb(255, 255, 255));
                auto bgVal = obj->Get("backgroundColor");
                if (bgVal && bgVal->type == ValueType::OBJECT) {
                    const nk_color bg = GetColor(obj, "backgroundColor", nk_rgb(50, 50, 50));
                    nk_style_push_color(ctx, &ctx->style.button.normal.data.color, bg);
                    nk_style_push_color(ctx, &ctx->style.button.hover.data.color, nk_rgb(bg.r + 20, bg.g + 20, bg.b + 20));
                    nk_style_push_color(ctx, &ctx->style.button.active.data.color, nk_rgb(bg.r - 20, bg.g - 20, bg.b - 20));
                    colorPushed += 3;
                }
            }
        }
        if (type == "button") {
            if (nk_button_label(ctx, text.c_str())) {
                if (!disabled) TriggerEvent(obj, "click");
            }
        } else if (type == "label") {
            nk_label(ctx, text.c_str(), GetAlign(obj));
        } else if (type == "input" || type == "password") {
            const std::string valStr = GetString(obj, "value");
            char buffer[256];
            strncpy(buffer, valStr.c_str(), 255);
            int len = valStr.length();
            nk_flags flags = NK_EDIT_FIELD;
            if (type == "password") flags |= NK_EDIT_SIG_ENTER;
            nk_edit_string(ctx, flags, buffer, &len, 255, nk_filter_default);
            std::string newVal(buffer, len);
            if (!disabled) {
                if (newVal != valStr) {
                    obj->Set("value", std::make_shared<StringValue>(newVal));
                    TriggerEvent(obj, "change", {std::make_shared<StringValue>(newVal)});
                }
            }
        } else if (type == "group") {
            if (nk_group_begin(ctx, text.c_str(), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
                nk_layout_space_begin(ctx, NK_STATIC, h, INT_MAX);
                auto children = obj->Get("children");
                if (children && children->type == ValueType::ARRAY) {
                    const auto arr = std::static_pointer_cast<ArrayValue>(children);
                    for (const auto &child: arr->Elements) {
                        RenderWidget(ctx, child);
                    }
                }
                nk_layout_space_end(ctx);
                nk_group_end(ctx);
            }
        }

        while (colorPushed > 0) {
            nk_style_pop_color(ctx);
            colorPushed--;
        }
    }


    static void RenderContentOnly(nk_context *ctx) {
        if (GuiModule::GlobalForms.empty()) return;
        const auto form = std::static_pointer_cast<ObjectValue>(GuiModule::GlobalForms[0]);
        auto children = form->Get("children");
        if (children->type == ValueType::ARRAY) {
            const auto arr = std::static_pointer_cast<ArrayValue>(children);
            for (auto &child: arr->Elements) {
                RenderWidget(ctx, child);
            }
        }
    }

    static void RenderContent(nk_context *ctx) {
        if (GuiModule::GlobalForms.empty()) return;
        const auto mainForm = std::static_pointer_cast<ObjectValue>(GuiModule::GlobalForms[0]);
        const float contentHeight = GetFloat(mainForm, "height", 600);
        nk_layout_space_begin(ctx, NK_STATIC, contentHeight, INT_MAX);
        auto children = mainForm->Get("children");
        if (children && children->type == ValueType::ARRAY) {
            const auto arr = std::static_pointer_cast<ArrayValue>(children);
            for (const auto& child : arr->Elements) {
                RenderWidget(ctx, child);
            }
        }
        nk_layout_space_end(ctx);
    }
};


#endif //BXSCRIPT_GUIRENDERER_H
