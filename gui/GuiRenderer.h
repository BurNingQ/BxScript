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
#include <vector>

#include "windows/ControlBase.h"
#include "common/StringKit.h"
#include "evaluator/EventLoop.h"
#include "evaluator/Interpreter.h"
#include "evaluator/Value.h"
#include "windows/App.h"
#include "windows/Button.h"
#include "windows/ComboBox.h"
#include "windows/Form.h"
#include "windows/ImageView.h"
#include "windows/Input.h"
#include "windows/Label.h"
#include "windows/ListView.h"
#include "windows/Panel.h"
#include "windows/ProgressBar.h"
#include "windows/Slider.h"

class GuiRenderer {
public:
    static inline std::vector<Form *> Roots{};

    static inline std::unordered_map<std::string, ControlBase *> ControlMap{};

    static void Render(const ValuePtr &dataTree) {
        if (dataTree->type != ValueType::OBJECT) return;
        static bool isInit = false;
        if (!isInit) {
            App::Init();
            isInit = true;
        }
        ControlBase *rootCtrl = BuildRecursive(dataTree, nullptr);
        if (rootCtrl != nullptr) {
            if (const auto form = dynamic_cast<Form *>(rootCtrl)) {
                Roots.push_back(form);
                form->Show();
            }
        }
    }

private:
    static ControlBase *BuildRecursive(const ValuePtr &node, ControlBase *parent) {
        if (node->type != ValueType::OBJECT) return nullptr;
        const auto obj = std::static_pointer_cast<ObjectValue>(node);
        const auto typeVal = obj->Get("_type");
        if (typeVal->type != ValueType::STRING) return nullptr;
        const std::string type = typeVal->ToString();
        std::string id{};
        const auto idVal = obj->Get("id");
        if (idVal->type == ValueType::STRING) id = idVal->ToString();
        ControlBase *ctrl = nullptr;
        if (type == "form") {
            ctrl = Form::New(nullptr);
        } else if (type == "button") {
            ctrl = static_cast<ControlBase *>(PushButton::Create(parent));
        } else if (type == "label") {
            ctrl = Label::Create(parent);
        } else if (type == "input") {
            ctrl = Edit::Create(parent);
        } else if (type == "group") {
            ctrl = static_cast<ControlBase *>(GroupBox::Create(parent));
        } else if (type == "checkbox") {
            ctrl = static_cast<ControlBase *>(CheckBox::Create(parent));
        } else if (type == "radio") {
            ctrl = static_cast<ControlBase *>(RadioButton::Create(parent));
        } else if (type == "multiline") {
            ctrl = static_cast<ControlBase *>(MultiEdit::Create(parent));
        } else if (type == "panel") {
            ctrl = static_cast<ControlBase *>(Panel::New(parent));
        } else if (type == "slider") {
            ctrl = static_cast<ControlBase *>(Slider::New(parent));
        } else if (type == "progress") {
            ctrl = static_cast<ControlBase *>(ProgressBar::New(parent));
        } else if (type == "image") {
            ctrl = static_cast<ControlBase *>(ImageView::New(parent));
        } else if (type == "list") {
            ctrl = static_cast<ControlBase *>(ListView::NewListView(parent));
        } else if (type == "select") ctrl = static_cast<ControlBase *>(ComboBox::Create(parent));
        if (!ctrl) return nullptr;

        // 4. 注册 ID 映射
        if (!id.empty()) {
            ControlMap[id] = ctrl;
        }

        // 5. 应用属性 (Text, Pos, Size, Events)
        ApplyProperties(ctrl, obj);

        // 6. 递归处理子节点
        auto childrenVal = obj->Get("children");
        if (childrenVal->type == ValueType::ARRAY) {
            const auto arr = std::static_pointer_cast<ArrayValue>(childrenVal);
            for (const auto &childNode: arr->Elements) {
                BuildRecursive(childNode, ctrl); // 递归
            }
        }

        return ctrl;
    }

    static void ApplyProperties(ControlBase *ctrl, const std::shared_ptr<ObjectValue> &obj) {
        // --- 基础属性 (Text)
        if (const auto v = obj->Get("text"); v->type == ValueType::STRING) {
            ctrl->SetText(StringKit::U8ToU16(v->ToString()));
        }

        // --- 几何属性 (X, Y, W, H)
        int x = 0, y = 0, w = 0, h = 0;
        bool hasPos = false, hasSize = false;

        if (const auto v = obj->Get("x"); v->type == ValueType::NUMBER) {
            x = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            hasPos = true;
        }
        if (const auto v = obj->Get("y"); v->type == ValueType::NUMBER) {
            y = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            hasPos = true;
        }
        if (const auto v = obj->Get("width"); v->type == ValueType::NUMBER) {
            w = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            hasSize = true;
        }
        if (const auto v = obj->Get("height"); v->type == ValueType::NUMBER) {
            h = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            hasSize = true;
        }

        if (hasPos) ctrl->SetPos(x, y);
        if (hasSize) ctrl->SetSize(w, h);

        // --- 样式属性 (Visible, Disable)
        if (auto v = obj->Get("visible"); v->type == ValueType::BOOL) {
            if (!std::static_pointer_cast<BoolValue>(v)->Value) ctrl->Hide();
        }
        if (auto v = obj->Get("disable"); v->type == ValueType::BOOL) {
            if (std::static_pointer_cast<BoolValue>(v)->Value) ctrl->SetEnabled(false);
        }

        // --- 事件绑定
        auto onClick = obj->Get("onClick");
        if (onClick->type == ValueType::FUNCTION) {
            if (const auto btn = dynamic_cast<Button *>(ctrl)) {
                btn->OnClick.Bind([onClick](const Event &e) {
                    Interpreter::CallFunction(onClick, {});
                });
            }
            // Label 模拟点击
            else if (const auto lbl = dynamic_cast<Label *>(ctrl)) {
                lbl->OnClick().Bind([onClick](const Event &e) {
                    Interpreter::CallFunction(onClick, {});
                });
            }
        }

        auto onChange = obj->Get("onChange");
        if (onChange->type == ValueType::FUNCTION) {
            if (const auto edit = dynamic_cast<Edit *>(ctrl)) {
                edit->OnChange().Bind([onChange](const Event &e) {
                    Interpreter::CallFunction(onChange, {});
                });
            } else if (const auto mEdit = dynamic_cast<MultiEdit *>(ctrl)) {
                mEdit->OnChange().Bind([onChange](const Event &e) {
                    Interpreter::CallFunction(onChange, {});
                });
            } else if (const auto slider = dynamic_cast<Slider *>(ctrl)) {
                slider->OnScroll().Bind([onChange](const Event &e) {
                    Interpreter::CallFunction(onChange, {});
                });
            }
        }

        // --- 控件特有属性处理
        // 图片源 (src)
        if (const auto img = dynamic_cast<ImageView *>(ctrl)) {
            if (const auto v = obj->Get("src"); v->type == ValueType::STRING) {
                std::string src = v->ToString();
                // 简单判断是网络图片还是本地图片
                if (src.rfind("http", 0) == 0) {
                    img->DrawImageUrl(StringKit::U8ToU16(src));
                } else {
                    img->DrawImageFile(StringKit::U8ToU16(src));
                }
            }
        }

        // 进度条/滑块 (min, max, value)
        if (const auto v = obj->Get("value"); v->type == ValueType::NUMBER) {
            const int val = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            if (const auto slider = dynamic_cast<Slider *>(ctrl)) slider->SetValue(val);
            else if (const auto pbar = dynamic_cast<ProgressBar *>(ctrl)) pbar->SetValue(val);
        }

        // 复选框/单选框 (checked)
        if (const auto v = obj->Get("checked"); v->type == ValueType::BOOL) {
            const bool checked = std::static_pointer_cast<BoolValue>(v)->Value;
            if (const auto btn = dynamic_cast<Button *>(ctrl)) {
                btn->SetChecked(checked);
            }
        }
    }
};

#endif //BXSCRIPT_GUIRENDERER_H
