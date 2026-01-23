/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/23
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    DlgModule
 */
#ifndef BXSCRIPT_DLGMODULE_H
#define BXSCRIPT_DLGMODULE_H


#include "../evaluator/Value.h"
#include "common/StringKit.h"
#include "gui/windows/CommonDlgs.h"

class DlgModule {
    static void InitMethods(std::shared_ptr<ObjectValue> &o) {
        const auto msgFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                std::wstring msg, title = L"提示";
                if (!args.empty()) msg = StringKit::U8ToU16(args[0]->ToString());
                if (args.size() > 1) title = StringKit::U8ToU16(args[1]->ToString());
                CommonDlgs::MsgBoxInfo(nullptr, msg, title);
                return std::make_shared<NullValue>();
            });
        o->Set("alert", msgFn);

        const auto confirmFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                std::wstring msg = L"确认操作?", title = L"确认";
                if (!args.empty()) msg = StringKit::U8ToU16(args[0]->ToString());
                if (args.size() > 1) title = StringKit::U8ToU16(args[1]->ToString());
                bool res = CommonDlgs::MsgBoxConfirm(nullptr, msg, title);
                return std::make_shared<BoolValue>(res);
            });
        o->Set("confirm", confirmFn);

        const auto openFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                std::wstring title = L"打开文件", filter = L"All Files|*.*";
                if (!args.empty()) title = StringKit::U8ToU16(args[0]->ToString());
                if (args.size() > 1) filter = StringKit::U8ToU16(args[1]->ToString());
                const std::wstring res = CommonDlgs::ShowOpenFileDlg(nullptr, title, filter);
                return std::make_shared<StringValue>(StringKit::U16ToU8(res));
            });
        o->Set("open", openFn);

        const auto saveFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                std::wstring title = L"保存文件", filter = L"All Files|*.*";
                if (!args.empty()) title = StringKit::U8ToU16(args[0]->ToString());
                if (args.size() > 1) filter = StringKit::U8ToU16(args[1]->ToString());
                const std::wstring res = CommonDlgs::ShowSaveFileDlg(nullptr, title, filter);
                return std::make_shared<StringValue>(StringKit::U16ToU8(res));
            });
        o->Set("save", saveFn);

        const auto folderFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                std::wstring title = L"选择文件夹";
                if (!args.empty()) title = StringKit::U8ToU16(args[0]->ToString());

                const std::wstring res = CommonDlgs::ShowBrowseFolderDlg(nullptr, title);
                return std::make_shared<StringValue>(StringKit::U16ToU8(res));
            });
        o->Set("folder", folderFn);

        const auto errorFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                std::wstring msg = L"Error", title = L"错误";
                if (!args.empty()) msg = StringKit::U8ToU16(args[0]->ToString());
                if (args.size() > 1) title = StringKit::U8ToU16(args[1]->ToString());
                CommonDlgs::MsgBoxError(nullptr, msg, title);
                return std::make_shared<NullValue>();
            });
        o->Set("error", errorFn);
    }

public:
    static ValuePtr CreateDlgModule() {
        auto dlg = std::make_shared<ObjectValue>();
        InitMethods(dlg);
        return dlg;
    }
};


#endif //BXSCRIPT_DLGMODULE_H
