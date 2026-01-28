/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/22
 * @license  MIT License
 *
 * @brief    GuiRenderer (Refactored: Unified Property Binding)
 */
#ifndef BXSCRIPT_GUIRENDERER_H
#define BXSCRIPT_GUIRENDERER_H
#include <utility>
#include <vector>
#include <functional>

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
#include "windows/Menu.h"
#include "common/EventKit.h"

using PropSetter = std::function<void(ControlBase *, ValuePtr)>;
using PropGetter = std::function<ValuePtr(ControlBase *)>;

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
                Sync::ReMountFormNativeFn(rootCtrl, std::static_pointer_cast<ObjectValue>(dataTree));
                Roots.push_back(form);
                form->Show();
            }
        }
    }

private:
    struct Sync {
        static void ReMountFormNativeFn(ControlBase *ctrl, std::shared_ptr<ObjectValue> obj) {
            auto form = dynamic_cast<Form *>(ctrl);
            obj->Set("doMin", std::make_shared<NativeFunctionValue>([obj, form](...) {
                form->Minimise();
                return obj;
            }));
            obj->Set("doMax", std::make_shared<NativeFunctionValue>([obj, form](...) {
                form->Maximise();
                return obj;
            }));
            obj->Set("doCap", std::make_shared<NativeFunctionValue>([obj, form](...) {
                doCap(form->Handle());
                return obj;
            }));
        }

        static void Bind(ControlBase *ctrl, const std::shared_ptr<ObjectValue> &obj, const std::string &key, const PropSetter &setter,
                         const PropGetter &getter = nullptr) {
            if (const auto v = obj->Get(key); v && v->type != ValueType::NULL_TYPE) {
                setter(ctrl, v);
            }
            const auto hookGetter = getter ? [ctrl, getter]() { return getter(ctrl); } : ObjectValue::NativeGetter(nullptr);
            auto hookSetter = [ctrl, setter](ValuePtr v) { setter(ctrl, std::move(v)); };
            obj->RegisterHooks(key, hookGetter, hookSetter);
        }

        static void Text(ControlBase *c, const ValuePtr &v) {
            if (v->type == ValueType::STRING) {
                c->SetText(StringKit::U8ToU16(v->ToString()));
            }
        }

        static void Visible(ControlBase *c, const ValuePtr &v) {
            if (v->type == ValueType::BOOL) {
                std::static_pointer_cast<BoolValue>(v)->Value ? c->Show() : c->Hide();
            }
        }

        static void Disable(ControlBase *c, const ValuePtr &v) {
            if (v->type == ValueType::BOOL) {
                c->SetEnabled(!std::static_pointer_cast<BoolValue>(v)->Value);
            }
        }

        static void X(ControlBase *c, const ValuePtr &v) {
            if (v->type != ValueType::NUMBER) return;
            int x, y;
            c->Pos(x, y);
            const int newX = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            c->SetPos(newX, y);
        }

        static void Y(ControlBase *c, const ValuePtr &v) {
            if (v->type != ValueType::NUMBER) return;
            int x, y;
            c->Pos(x, y);
            const int newY = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            c->SetPos(x, newY);
        }

        static void Width(ControlBase *c, const ValuePtr &v) {
            if (v->type != ValueType::NUMBER) return;
            const int newW = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            c->SetSize(newW, c->Height());
        }

        static void Height(ControlBase *c, const ValuePtr &v) {
            if (v->type != ValueType::NUMBER) return;
            const int newH = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            c->SetSize(c->Width(), newH);
        }

        static void ImageSrc(ControlBase *c, const ValuePtr &v) {
            if (v->type != ValueType::STRING) return;
            if (const auto img = dynamic_cast<ImageView *>(c)) {
                if (const std::string src = v->ToString(); src.rfind("http", 0) == 0) {
                    img->DrawImageUrl(StringKit::U8ToU16(src));
                } else {
                    img->DrawImageFile(StringKit::U8ToU16(src));
                }
            }
        }

        static void ListHeads(ControlBase *c, const ValuePtr &v) {
            if (v->type != ValueType::ARRAY) return;
            const auto lv = dynamic_cast<ListView *>(c);
            if (!lv) return;
            const auto arr = std::static_pointer_cast<ArrayValue>(v);
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

        static void ListItems(ControlBase *c, const ValuePtr &v) {
            if (v->type != ValueType::ARRAY) return;
            const auto arr = std::static_pointer_cast<ArrayValue>(v);
            if (const auto cb = dynamic_cast<ComboBox *>(c)) {
                cb->DeleteAllItems();
                for (const auto &item: arr->Elements) {
                    cb->AddItem(StringKit::U8ToU16(item->ToString()));
                }
                if (!arr->Elements.empty()) cb->SetSelectedItem(0);
            }
            if (const auto lv = dynamic_cast<ListView *>(c)) {
                lv->DeleteAllItems();
                if (const int colCount = lv->GetColumnCount(); colCount == 0) {
                    lv->AddColumn(L"列表", -1);
                }
                int id = 0;
                for (auto &itemVal: arr->Elements) {
                    if (itemVal->type == ValueType::ARRAY) {
                        const auto rowArr = std::static_pointer_cast<ArrayValue>(itemVal);
                        std::vector<std::wstring> cols;
                        for (const auto &sub: rowArr->Elements) {
                            cols.push_back(StringKit::U8ToU16(sub->ToString()));
                        }
                        const auto listItem = new StringListItem(id++, cols, false);
                        lv->AddItem(listItem);
                    } else {
                        const auto listItem = new StringListItem(id++, StringKit::U8ToU16(itemVal->ToString()), false);
                        lv->AddItem(listItem);
                    }
                }
            }
        }

        static void FontCfg(ControlBase *c, const ValuePtr &v) {
        }
    };

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

    static void BindEvents(ControlBase *ctrl, const std::shared_ptr<ObjectValue> &obj) {
        auto Bind = [&](const std::string &propName, EventManager &cppEvent) {
            if (auto funcVal = obj->Get(propName); funcVal && funcVal->type == ValueType::FUNCTION) {
                cppEvent.Bind([funcVal](const Event &e) {
                    Interpreter::CallFunction(funcVal, {EventKit::FromEvent(e)});
                });
            }
        };

        Bind("scroll", ctrl->OnMouseWheel());
        Bind("mouseover", ctrl->OnMouseHover());
        Bind("mouseleave", ctrl->OnMouseLeave());
        Bind("mousemove", ctrl->OnMouseMove());
        Bind("mousedown", ctrl->OnLBDown());
        Bind("mouseup", ctrl->OnLBUp());
        Bind("keyup", ctrl->OnKeyUp());
        Bind("keydown", ctrl->OnKeyDown());
        Bind("resize", ctrl->OnSize());
        Bind("create", ctrl->OnCreate());
        Bind("close", ctrl->OnClose());
        Bind("paint", ctrl->OnPaint());

        if (const auto btn = dynamic_cast<Button *>(ctrl)) {
            Bind("click", btn->OnClick);
        } else if (const auto lbl = dynamic_cast<Label *>(ctrl)) {
            Bind("click", lbl->OnClick());
        }
        if (const auto edit = dynamic_cast<Edit *>(ctrl)) {
            Bind("change", edit->OnChange());
        } else if (const auto mEdit = dynamic_cast<MultiEdit *>(ctrl)) {
            Bind("change", mEdit->OnChange());
        } else if (const auto slider = dynamic_cast<Slider *>(ctrl)) {
            Bind("change", slider->OnScroll());
        } else if (const auto combo = dynamic_cast<ComboBox *>(ctrl)) {
            Bind("change", combo->OnSelectedChange);
        } else if (const auto lv = dynamic_cast<ListView *>(ctrl)) {
            Bind("change", lv->OnItemChanged());
        }
    }

    static void BindAllProperties(ControlBase *ctrl, const std::shared_ptr<ObjectValue> &obj) {
        Sync::Bind(ctrl, obj, "_text", Sync::Text, [](ControlBase *c) {
            return std::make_shared<StringValue>(StringKit::U16ToU8(c->Text()));
        });

        Sync::Bind(ctrl, obj, "_visible", Sync::Visible, [](ControlBase *c) {
            return std::make_shared<BoolValue>(c->Visible());
        });

        Sync::Bind(ctrl, obj, "_disable", Sync::Disable, [](ControlBase *c) {
            return std::make_shared<BoolValue>(!c->Enabled());
        });

        Sync::Bind(ctrl, obj, "_x", Sync::X, [](ControlBase *c) {
            int x, y;
            c->Pos(x, y);
            return std::make_shared<NumberValue>(x);
        });

        Sync::Bind(ctrl, obj, "_y", Sync::Y, [](ControlBase *c) {
            int x, y;
            c->Pos(x, y);
            return std::make_shared<NumberValue>(y);
        });

        Sync::Bind(ctrl, obj, "_width", Sync::Width, [](ControlBase *c) {
            return std::make_shared<NumberValue>(c->Width());
        });

        Sync::Bind(ctrl, obj, "_height", Sync::Height, [](ControlBase *c) {
            return std::make_shared<NumberValue>(c->Height());
        });

        if (dynamic_cast<ListView *>(ctrl)) {
            Sync::Bind(ctrl, obj, "_heads", Sync::ListHeads);
        }
        if (dynamic_cast<ListView *>(ctrl) || dynamic_cast<ComboBox *>(ctrl)) {
            Sync::Bind(ctrl, obj, "_items", Sync::ListItems);
        }
        if (dynamic_cast<ImageView *>(ctrl)) {
            Sync::Bind(ctrl, obj, "_src", Sync::ImageSrc);
        }

        if (const auto form = dynamic_cast<Form *>(ctrl)) {
            if (const auto v = obj->Get("_center"); v && v->type == ValueType::BOOL) {
                form->Center();
            }
            if (const auto v = obj->Get("_icon"); v && v->type == ValueType::STRING) {
                const auto icon = Icon::NewIconFromImageFile(StringKit::U8ToU16(v->ToString()));
                form->SetIcon(0, icon);
            }

            // Menu
            if (auto menuVal = obj->Get("_menu"); menuVal && menuVal->type == ValueType::OBJECT) {
                auto menuObj = std::static_pointer_cast<ObjectValue>(menuVal);
                auto childrenVal = menuObj->Get("children");
                if (childrenVal && childrenVal->type == ValueType::ARRAY) {
                    Menu *nativeMenu = form->NewMenu();
                    BuildMenuItems(std::static_pointer_cast<ArrayValue>(childrenVal), nativeMenu, true);
                    nativeMenu->Show();
                }
            }

            // Tray
            if (auto trayVal = obj->Get("_trayConf"); trayVal && trayVal->type == ValueType::OBJECT) {
                auto conf = std::static_pointer_cast<ObjectValue>(trayVal);
                std::string iconPath, tip;
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

        std::wstring fontFamily = DefaultFont->Family();
        int fontSize = DefaultFont->PointSize();
        int fontStyle = DefaultFont->Style();
        bool fontChanged = false;

        if (const auto v = obj->Get("_fontSize"); v && v->type == ValueType::NUMBER) {
            fontSize = static_cast<int>(std::static_pointer_cast<NumberValue>(v)->Value);
            fontChanged = true;
        }
        if (fontChanged) {
            auto ft = new Font(fontFamily, fontSize, fontStyle);
            ctrl->SetFont(ft);
        }

        if (const auto v = obj->Get("_fontColor"); v && v->type == ValueType::OBJECT) {
            auto const o = std::static_pointer_cast<ObjectValue>(v);
            auto const r = o->Get("R");
            auto const g = o->Get("G");
            auto const b = o->Get("B");
            if (r->type == ValueType::NUMBER) {
                ctrl->SetTextColor(Color::FromRGB(
                    static_cast<int>(std::static_pointer_cast<NumberValue>(r)->Value),
                    static_cast<int>(std::static_pointer_cast<NumberValue>(g)->Value),
                    static_cast<int>(std::static_pointer_cast<NumberValue>(b)->Value)
                ));
            }
        }
        if (const auto v = obj->Get("_bgColor"); v && v->type == ValueType::OBJECT) {
            auto const o = std::static_pointer_cast<ObjectValue>(v);
            auto const r = o->Get("R");
            auto const g = o->Get("G");
            auto const b = o->Get("B");
            if (r->type == ValueType::NUMBER && g->type == ValueType::NUMBER && b->type == ValueType::NUMBER) {
                ctrl->SetBackgroundColor(Color::FromRGB(
                    static_cast<int>(std::static_pointer_cast<NumberValue>(r)->Value),
                    static_cast<int>(std::static_pointer_cast<NumberValue>(g)->Value),
                    static_cast<int>(std::static_pointer_cast<NumberValue>(b)->Value)
                ));
            }
        }
        BindEvents(ctrl, obj);
    }

    static ControlBase *BuildRecursive(const ValuePtr &node, ControlBase *parent) {
        if (node->type != ValueType::OBJECT) return nullptr;
        const auto obj = std::static_pointer_cast<ObjectValue>(node);
        const auto typeVal = obj->Get("_type");
        if (typeVal->type != ValueType::STRING) return nullptr;
        const std::string type = typeVal->ToString();

        const auto idVal = obj->Get("id");
        std::string id;
        if (idVal->type == ValueType::STRING) id = idVal->ToString();

        ControlBase *ctrl = nullptr;
        if (type == "form") ctrl = Form::New(nullptr);
        else if (type == "button") ctrl = static_cast<ControlBase *>(PushButton::Create(parent));
        else if (type == "label") ctrl = Label::Create(parent);
        else if (type == "input") ctrl = Edit::Create(parent);
        else if (type == "password") ctrl = Edit::Create(parent)->SetPassword(true);
        else if (type == "group") ctrl = static_cast<ControlBase *>(GroupBox::Create(parent));
        else if (type == "checkbox") ctrl = static_cast<ControlBase *>(CheckBox::Create(parent));
        else if (type == "radio") ctrl = static_cast<ControlBase *>(RadioButton::Create(parent));
        else if (type == "multiline") ctrl = static_cast<ControlBase *>(MultiEdit::Create(parent));
        else if (type == "panel") ctrl = static_cast<ControlBase *>(Panel::New(parent));
        else if (type == "slider") ctrl = static_cast<ControlBase *>(Slider::New(parent));
        else if (type == "progress") ctrl = static_cast<ControlBase *>(ProgressBar::New(parent));
        else if (type == "image") ctrl = static_cast<ControlBase *>(ImageView::New(parent));
        else if (type == "list") ctrl = static_cast<ControlBase *>(ListView::NewListView(parent));
        else if (type == "select") ctrl = static_cast<ControlBase *>(ComboBox::Create(parent));

        if (!ctrl) return nullptr;

        if (!id.empty()) ControlMap[id] = ctrl;

        BindAllProperties(ctrl, obj);

        if (const auto childrenVal = obj->Get("children"); childrenVal && childrenVal->type == ValueType::ARRAY) {
            const auto arr = std::static_pointer_cast<ArrayValue>(childrenVal);
            for (const auto &childNode: arr->Elements) {
                BuildRecursive(childNode, ctrl);
            }
        }

        return ctrl;
    }
};

#endif //BXSCRIPT_GUIRENDERER_H
