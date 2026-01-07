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

#define BXSCRIPT_IMPLEMENTATION

#include "windows/ControlBase.h"
#include "common/StringKit.h"
#include "evaluator/EventLoop.h"
#include "evaluator/Interpreter.h"
#include "evaluator/Value.h"
#include "windows/App.h"
#include "windows/Button.h"
#include "windows/Form.h"
#include "windows/Input.h"
#include "windows/Label.h"

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

    static void MainLoop() {
        while (App::PollEvents()) {
            bool hasScriptWork = EventLoop::Dispatch(5);
            if (!hasScriptWork) {
                App::WaitEvents(10);
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
            ctrl = (ControlBase*)PushButton::Create(parent);
        } else if (type == "label") {
            ctrl = Label::Create(parent);
        } else if (type == "input") {
            ctrl = Edit::Create(parent);
        } else if (type == "group") {
            ctrl = (ControlBase*)GroupBox::Create(parent);
        }
        // ... 其他控件 ...

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
            auto arr = std::static_pointer_cast<ArrayValue>(childrenVal);
            for (const auto &childNode: arr->Elements) {
                BuildRecursive(childNode, ctrl); // 递归
            }
        }

        return ctrl;
    }

    static void ApplyProperties(ControlBase *ctrl, const std::shared_ptr<ObjectValue> &obj) {
        // --- 基础属性 ---
        if (auto v = obj->Get("text"); v->type == ValueType::STRING) {
            ctrl->SetText(StringKit::U8ToU16(v->ToString()));
        }
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
        auto onClick = obj->Get("onClick");
        if (onClick->type == ValueType::FUNCTION) {
            if (const auto btn = dynamic_cast<Button *>(ctrl)) {
                btn->OnClick.Bind([onClick](const Event &e) {
                    Interpreter::CallFunction(onClick, {});
                });
            }
            else if (const auto lbl = dynamic_cast<Label *>(ctrl)) {
                lbl->OnClick().Bind([onClick](const Event &e) {
                    Interpreter::CallFunction(onClick, {});
                });
            }
        }

        // 绑定 onChange (Input)
        // ...
    }
};

#endif //BXSCRIPT_GUIRENDERER_H
