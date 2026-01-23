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

#include "common/ColorKit.h"
#include "windows/ControlBase.h"
#include "common/StringKit.h"
#include "evaluator/EventLoop.h"
#include "evaluator/Interpreter.h"
#include "evaluator/Value.h"
#include "windows/App.h"
#include "windows/Button.h"
#include "windows/ComboBox.h"
#include "windows/Form.h"
#include "windows/GlobalVars.h"
#include "windows/ImageView.h"
#include "windows/Input.h"
#include "windows/Label.h"
#include "windows/ListView.h"
#include "windows/Panel.h"
#include "windows/ProgressBar.h"
#include "windows/Slider.h"
#include "common/EventKit.h"

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
    static void BuildMenuItems(const std::shared_ptr<ArrayValue> &children, void *parentPtr, bool isRoot) {
        for (const auto &childVal: children->Elements) {
            if (childVal->type != ValueType::OBJECT) continue;
            auto const childObj = std::static_pointer_cast<ObjectValue>(childVal);
            if (std::string type = childObj->Get("_type")->ToString(); type == "separator") {
                if (isRoot) continue;
                static_cast<MenuItem *>(parentPtr)->AddSeparator();
                continue;
            }
            std::string text;
            if (auto const t = childObj->Get("text"); t->type == ValueType::STRING) text = t->ToString();
            auto subChildrenVal = childObj->Get("children");
            bool const hasSubChildren = subChildrenVal && subChildrenVal->type == ValueType::ARRAY &&
                                        !std::static_pointer_cast<ArrayValue>(subChildrenVal)->Elements.empty();
            MenuItem *newItem = nullptr;
            if (hasSubChildren) {
                if (isRoot) {
                    newItem = static_cast<Menu *>(parentPtr)->AddSubMenu(StringKit::U8ToU16(text));
                } else {
                    newItem = static_cast<MenuItem *>(parentPtr)->AddSubMenu(StringKit::U8ToU16(text));
                }
                BuildMenuItems(std::static_pointer_cast<ArrayValue>(subChildrenVal), newItem, false);
            } else {
                if (isRoot) {
                    newItem = static_cast<Menu *>(parentPtr)->AddItem(StringKit::U8ToU16(text), Shortcut{0, 0});
                } else {
                    newItem = static_cast<MenuItem *>(parentPtr)->AddItem(StringKit::U8ToU16(text), Shortcut{0, 0});
                }
            }
            if (auto clickFunc = childObj->Get("click"); clickFunc && clickFunc->type == ValueType::FUNCTION) {
                newItem->OnClick().Bind([clickFunc](const Event &e) {
                    Interpreter::CallFunction(clickFunc, {EventKit::FromEvent(e)});
                });
            }
            if (auto v = childObj->Get("enabled"); v && v->type == ValueType::BOOL) {
                newItem->SetEnabled(std::static_pointer_cast<BoolValue>(v)->Value);
            }
            if (auto v = childObj->Get("checked"); v && v->type == ValueType::BOOL) {
                newItem->SetChecked(std::static_pointer_cast<BoolValue>(v)->Value);
            }
        }
    }

    static void BindNativeSync(ControlBase *ctrl, std::shared_ptr<ObjectValue> obj) {
        obj->RegisterHooks("_text", [ctrl]() -> ValuePtr {
                               const std::wstring txt = ctrl->Text();
                               return std::make_shared<StringValue>(StringKit::U16ToU8(txt));
                           },
                           [ctrl](const ValuePtr &v) {
                               ctrl->SetText(StringKit::U8ToU16(v->ToString()));
                           }
        );
        obj->RegisterHooks("_visible", [ctrl]() -> ValuePtr {
                               return std::make_shared<BoolValue>(ctrl->Visible());
                           },
                           [ctrl](const ValuePtr &v) {
                               if (v->type != ValueType::BOOL) return;
                               if (std::static_pointer_cast<BoolValue>(v)->Value) ctrl->Show();
                               else ctrl->Hide();
                           }
        );
        obj->RegisterHooks("_x", [ctrl]() -> ValuePtr {
                               int x, y;
                               ctrl->Pos(x, y);
                               return std::make_shared<NumberValue>(x);
                           },
                           [ctrl](const ValuePtr &v) {
                               if (v->type != ValueType::NUMBER) return;
                               int x, y;
                               ctrl->Pos(x, y);
                               const int newX = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
                               ctrl->SetPos(newX, y);
                           }
        );
        obj->RegisterHooks("_y", [ctrl]() -> ValuePtr {
                               int x, y;
                               ctrl->Pos(x, y);
                               return std::make_shared<NumberValue>(y);
                           },
                           [ctrl](const ValuePtr &v) {
                               if (v->type != ValueType::NUMBER) return;
                               int x, y;
                               ctrl->Pos(x, y);
                               const int newY = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
                               ctrl->SetPos(x, newY);
                           }
        );
        obj->RegisterHooks("_width", [ctrl]() -> ValuePtr {
                               return std::make_shared<NumberValue>(ctrl->Width());
                           },
                           [ctrl](const ValuePtr &v) {
                               if (v->type != ValueType::NUMBER) return;
                               const auto h = ctrl->Height();
                               const int newW = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
                               ctrl->SetSize(newW, h);
                           }
        );
        obj->RegisterHooks("_height", [ctrl]() -> ValuePtr {
                               return std::make_shared<NumberValue>(ctrl->Height());
                           },
                           [ctrl](const ValuePtr &v) {
                               if (v->type != ValueType::NUMBER) return;
                               const auto w = ctrl->Width();
                               const int newH = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
                               ctrl->SetSize(w, newH);
                           }
        );
        obj->RegisterHooks("_disable", [ctrl]() -> ValuePtr {
                               return std::make_shared<BoolValue>(ctrl->Enabled());
                           }, [ctrl](const ValuePtr &v) {
                               if (v->type != ValueType::BOOL) return;
                               const auto tv = std::static_pointer_cast<BoolValue>(v);
                               ctrl->SetEnabled(!tv->Value);
                           });
        obj->RegisterHooks("_src", [ctrl]() -> ValuePtr {
                               return std::make_shared<BoolValue>(ctrl->Enabled());
                           }, [ctrl](const ValuePtr &v) {
                               if (v->type != ValueType::STRING) return;
                               if (const std::string src = v->ToString(); src.rfind("http", 0) == 0) {
                                   dynamic_cast<ImageView *>(ctrl)->DrawImageUrl(StringKit::U8ToU16(src));
                               } else {
                                   dynamic_cast<ImageView *>(ctrl)->DrawImageFile(StringKit::U8ToU16(src));
                               }
                           });
        obj->RegisterHooks("_heads", nullptr, [ctrl](const ValuePtr &v) {
            if (v->type != ValueType::ARRAY) return;
            auto arr = std::static_pointer_cast<ArrayValue>(v);

            if (auto lv = dynamic_cast<ListView *>(ctrl)) {
                lv->DeleteAllColumns();
                int colIdx = 0;
                for (auto &head: arr->Elements) {
                    std::wstring title{};
                    int width = 100;
                    if (head->type == ValueType::STRING) {
                        title = StringKit::U8ToU16(head->ToString());
                    } else if (head->type == ValueType::OBJECT) {
                        const auto headObj = std::static_pointer_cast<ObjectValue>(head);
                        if (const auto t = headObj->Get("text"); t && t->type == ValueType::STRING) {
                            title = StringKit::U8ToU16(t->ToString());
                        }
                        if (auto w = headObj->Get("width"); w && w->type == ValueType::NUMBER) {
                            width = static_cast<int>(std::static_pointer_cast<NumberValue>(w)->Value);
                        }
                    }
                    lv->AddColumn(title, width);
                    colIdx++;
                }
                if (colIdx > 0) lv->StretchLastColumn();
            }
        });

        obj->RegisterHooks("_items", nullptr, [ctrl, obj](const ValuePtr &v) {
            if (v->type != ValueType::ARRAY) return;
            const auto arr = std::static_pointer_cast<ArrayValue>(v);
            if (const auto cb = dynamic_cast<ComboBox *>(ctrl)) {
                cb->DeleteAllItems();
                for (const auto &item: arr->Elements) {
                    cb->AddItem(StringKit::U8ToU16(item->ToString()));
                }
                if (!arr->Elements.empty()) cb->SetSelectedItem(0);
            }
            if (const auto lv = dynamic_cast<ListView *>(ctrl)) {
                lv->DeleteAllItems();
                if (int colCount = lv->GetColumnCount(); colCount == 0) {
                    lv->AddColumn(L"列表", -1);
                    colCount = 1;
                }
                int id = 0;
                for (auto &itemVal: arr->Elements) {
                    if (itemVal->type == ValueType::ARRAY) {
                        const auto rowArr = std::static_pointer_cast<ArrayValue>(itemVal);
                        std::vector<std::wstring> cols;
                        for (const auto &c: rowArr->Elements) {
                            cols.push_back(StringKit::U8ToU16(c->ToString()));
                        }
                        const auto listItem = new StringListItem(id++, cols, false);
                        lv->AddItem(listItem);
                    }
                }
            }
        });
    }

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
        } else if (type == "password") {
            ctrl = Edit::Create(parent)->SetPassword(true);
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

        // 注册 ID 映射
        if (!id.empty()) {
            ControlMap[id] = ctrl;
        }

        // 应用属性 (Text, Pos, Size, Events)
        ApplyProperties(ctrl, obj);

        // 递归处理子节点
        if (auto childrenVal = obj->Get("children"); childrenVal->type == ValueType::ARRAY) {
            const auto arr = std::static_pointer_cast<ArrayValue>(childrenVal);
            for (const auto &childNode: arr->Elements) {
                BuildRecursive(childNode, ctrl);
            }
        }

        return ctrl;
    }

    static void ApplyProperties(ControlBase *ctrl, const std::shared_ptr<ObjectValue> &obj) {
        // 基础属性 (Text)
        if (const auto v = obj->Get("_text"); v->type == ValueType::STRING) {
            ctrl->SetText(StringKit::U8ToU16(v->ToString()));
        }

        // 字体处理
        std::wstring fontFamily = DefaultFont->Family();
        int fontSize = DefaultFont->PointSize();
        int fontStyle = DefaultFont->Style();
        if (const auto v = obj->Get("_fontSize"); v->type == ValueType::NUMBER) {
            fontSize = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
        }
        auto ft = new Font(fontFamily, fontSize, fontStyle);
        ctrl->SetFont(ft);

        // 文字颜色处理
        if (const auto v = obj->Get("_fontColor"); v->type == ValueType::OBJECT) {
            auto const o = std::static_pointer_cast<ObjectValue>(v);
            auto const r = o->Get("R");
            auto const g = o->Get("G");
            auto const b = o->Get("B");
            if (r->type == ValueType::NUMBER && g->type == ValueType::NUMBER && b->type == ValueType::NUMBER) {
                auto const nr = static_cast<int>(std::static_pointer_cast<NumberValue>(r)->Value);
                auto const ng = static_cast<int>(std::static_pointer_cast<NumberValue>(g)->Value);
                auto const nb = static_cast<int>(std::static_pointer_cast<NumberValue>(b)->Value);
                auto const color = Color::FromRGB(nr, ng, nb);
                ctrl->SetTextColor(color);
            }
        }

        // 背景颜色处理
        if (const auto v = obj->Get("_bgColor"); v->type == ValueType::OBJECT) {
            auto const o = std::static_pointer_cast<ObjectValue>(v);
            auto const r = o->Get("R");
            auto const g = o->Get("G");
            auto const b = o->Get("B");
            if (r->type == ValueType::NUMBER && g->type == ValueType::NUMBER && b->type == ValueType::NUMBER) {
                auto const nr = static_cast<int>(std::static_pointer_cast<NumberValue>(r)->Value);
                auto const ng = static_cast<int>(std::static_pointer_cast<NumberValue>(g)->Value);
                auto const nb = static_cast<int>(std::static_pointer_cast<NumberValue>(b)->Value);
                auto const color = Color::FromRGB(nr, ng, nb);
                ctrl->SetBackgroundColor(color);
            }
        }

        // 几何属性 (X, Y, W, H)
        int x = 0, y = 0, w = 0, h = 0;
        bool hasPos = false, hasSize = false;

        if (const auto v = obj->Get("_x"); v->type == ValueType::NUMBER) {
            x = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            hasPos = true;
        }
        if (const auto v = obj->Get("_y"); v->type == ValueType::NUMBER) {
            y = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            hasPos = true;
        }
        if (const auto v = obj->Get("_width"); v->type == ValueType::NUMBER) {
            w = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            hasSize = true;
        }
        if (const auto v = obj->Get("_height"); v->type == ValueType::NUMBER) {
            h = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            hasSize = true;
        }

        // size高优先级，center等依赖大小
        if (hasSize) ctrl->SetSize(w, h);
        if (hasPos) ctrl->SetPos(x, y);

        if (const auto form = dynamic_cast<Form *>(ctrl)) {
            if (const auto v = obj->Get("_center"); v->type == ValueType::BOOL) {
                form->Center();
            }
            if (const auto v = obj->Get("_icon"); v->type == ValueType::STRING) {
                const auto icon = Icon::NewIconFromImageFile(StringKit::U8ToU16(v->ToString()));
                form->SetIcon(0, icon);
            }
            if (auto menuVal = obj->Get("_menu"); menuVal && menuVal->type == ValueType::OBJECT) {
                auto menuObj = std::static_pointer_cast<ObjectValue>(menuVal);
                auto childrenVal = menuObj->Get("children");
                if (childrenVal && childrenVal->type == ValueType::ARRAY) {
                    Menu *nativeMenu = form->NewMenu();
                    BuildMenuItems(std::static_pointer_cast<ArrayValue>(childrenVal), nativeMenu, true);
                    nativeMenu->Show();
                }
            }
            if (auto trayVal = obj->Get("_trayConf"); trayVal && trayVal->type == ValueType::OBJECT) {
                auto conf = std::static_pointer_cast<ObjectValue>(trayVal);
                std::string iconPath = "";
                std::string tip = "";
                if (auto v = conf->Get("icon"); v->type == ValueType::STRING) iconPath = v->ToString();
                if (auto v = conf->Get("tip"); v->type == ValueType::STRING) tip = v->ToString();
                form->SetTrayIcon(StringKit::U8ToU16(iconPath), StringKit::U8ToU16(tip));
                if (auto v = conf->Get("click"); v->type == ValueType::FUNCTION) {
                    form->OnTrayClick().Bind([v](const Event &e) {
                        Interpreter::CallFunction(v, {});
                    });
                }
            }
        }

        // 样式属性 (Visible, Disable)
        if (auto v = obj->Get("_visible"); v->type == ValueType::BOOL) {
            if (!std::static_pointer_cast<BoolValue>(v)->Value) ctrl->Hide();
        }
        if (auto v = obj->Get("_disable"); v->type == ValueType::BOOL) {
            if (std::static_pointer_cast<BoolValue>(v)->Value) ctrl->SetEnabled(false);
        }

        // 事件绑定
        auto BindEvent = [&](const std::string &propName, EventManager &cppEvent) {
            if (auto funcVal = obj->Get(propName); funcVal->type == ValueType::FUNCTION) {
                cppEvent.Bind([funcVal](const Event &e) {
                    ValuePtr eventData = EventKit::FromEvent(e);
                    Interpreter::CallFunction(funcVal, {eventData});
                });
            }
        };
        BindEvent("scroll", ctrl->OnMouseWheel());
        BindEvent("mouseover", ctrl->OnMouseHover());
        BindEvent("mouseleave", ctrl->OnMouseLeave());
        BindEvent("mousemove", ctrl->OnMouseMove());
        BindEvent("mousedown", ctrl->OnLBDown());
        BindEvent("mouseup", ctrl->OnLBUp());
        BindEvent("keyup", ctrl->OnKeyUp());
        BindEvent("keydown", ctrl->OnKeyDown());
        BindEvent("resize", ctrl->OnSize());
        BindEvent("create", ctrl->OnCreate());
        BindEvent("close", ctrl->OnClose());
        BindEvent("paint", ctrl->OnPaint());
        if (const auto btn = dynamic_cast<Button *>(ctrl)) {
            BindEvent("click", btn->OnClick);
        } else if (const auto lbl = dynamic_cast<Label *>(ctrl)) {
            BindEvent("click", lbl->OnClick());
        }
        if (const auto edit = dynamic_cast<Edit *>(ctrl)) {
            BindEvent("change", edit->OnChange());
        } else if (const auto mEdit = dynamic_cast<MultiEdit *>(ctrl)) {
            BindEvent("change", mEdit->OnChange());
        } else if (const auto slider = dynamic_cast<Slider *>(ctrl)) {
            BindEvent("change", slider->OnScroll());
        } else if (const auto combo = dynamic_cast<ComboBox *>(ctrl)) {
            BindEvent("change", combo->OnSelectedChange);
        } else if (const auto lv = dynamic_cast<ListView *>(ctrl)) {
            BindEvent("change", lv->OnItemChanged());
        }

        if (const auto img = dynamic_cast<ImageView *>(ctrl)) {
            if (const auto v = obj->Get("_src"); v->type == ValueType::STRING) {
                if (std::string src = v->ToString(); src.rfind("http", 0) == 0) {
                    img->DrawImageUrl(StringKit::U8ToU16(src));
                } else {
                    img->DrawImageFile(StringKit::U8ToU16(src));
                }
            }
        }

        if (const auto v = obj->Get("_value"); v->type == ValueType::NUMBER) {
            const int val = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            if (const auto slider = dynamic_cast<Slider *>(ctrl)) slider->SetValue(val);
            else if (const auto pbar = dynamic_cast<ProgressBar *>(ctrl)) pbar->SetValue(val);
        }

        if (const auto v = obj->Get("_checked"); v->type == ValueType::BOOL) {
            const bool checked = std::static_pointer_cast<BoolValue>(v)->Value;
            if (const auto btn = dynamic_cast<Button *>(ctrl)) {
                btn->SetChecked(checked);
            }
        }

        if (auto lv = dynamic_cast<ListView *>(ctrl)) {
            if (auto headsVal = obj->Get("_heads"); headsVal && headsVal->type == ValueType::ARRAY) {
                auto arr = std::static_pointer_cast<ArrayValue>(headsVal);
                int colIdx = 0;
                for (auto& head : arr->Elements) {
                    std::wstring title;
                    int width = 100;
                    if (head->type == ValueType::STRING) {
                        title = StringKit::U8ToU16(head->ToString());
                    }
                    else if (head->type == ValueType::OBJECT) {
                        auto headObj = std::static_pointer_cast<ObjectValue>(head);
                        if (auto t = headObj->Get("text"); t && t->type == ValueType::STRING) {
                            title = StringKit::U8ToU16(t->ToString());
                        }
                        if (auto w = headObj->Get("width"); w && w->type == ValueType::NUMBER) {
                            width = static_cast<int>(std::static_pointer_cast<NumberValue>(w)->Value);
                        }
                    }
                    lv->AddColumn(title, width);
                    colIdx++;
                }
                if (colIdx > 0) lv->StretchLastColumn();
            }

            if (auto itemsVal = obj->Get("_items"); itemsVal && itemsVal->type == ValueType::ARRAY) {
                auto arr = std::static_pointer_cast<ArrayValue>(itemsVal);
                if (lv->GetColumnCount() == 0) {
                    lv->AddColumn(L"列表", -1);
                }
                int id = 0;
                for (auto &itemVal: arr->Elements) {
                    if (itemVal->type == ValueType::ARRAY) {
                        auto rowArr = std::static_pointer_cast<ArrayValue>(itemVal);
                        std::vector<std::wstring> cols;
                        for (const auto &c: rowArr->Elements) {
                            cols.push_back(StringKit::U8ToU16(c->ToString()));
                        }
                        auto listItem = new StringListItem(id++, cols, false);
                        lv->AddItem(listItem);
                    } else {
                        auto listItem = new StringListItem(id++, StringKit::U8ToU16(itemVal->ToString()), false);
                        lv->AddItem(listItem);
                    }
                }
            }
        }

        if (auto cb = dynamic_cast<ComboBox *>(ctrl)) {
            if (auto itemsVal = obj->Get("_items"); itemsVal && itemsVal->type == ValueType::ARRAY) {
                auto arr = std::static_pointer_cast<ArrayValue>(itemsVal);
                for (auto &item: arr->Elements) {
                    cb->AddItem(StringKit::U8ToU16(item->ToString()));
                }
                if (!arr->Elements.empty()) cb->SetSelectedItem(0);
            }
        }

        BindNativeSync(ctrl, obj);
    }
};

#endif //BXSCRIPT_GUIRENDERER_H
