/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/22
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    EventKit
 */
#ifndef BXSCRIPT_EVENTKIT_H
#define BXSCRIPT_EVENTKIT_H

#include "StringKit.h"
#include "../evaluator/Value.h"
#include "../gui/windows/Event.h"
#include "../gui/windows/EventData.h"

class EventKit {
public:
    static ValuePtr FromEvent(const Event &e) {
        auto eventObj = std::make_shared<ObjectValue>();
        if (!e.Data.has_value()) {
            return eventObj;
        }
        if (e.Data.type() == typeid(MouseEventData *)) {
            const auto data = std::any_cast<MouseEventData *>(e.Data);
            eventObj->Set("type", std::make_shared<StringValue>("mouse"));
            eventObj->Set("x", std::make_shared<NumberValue>(data->X));
            eventObj->Set("y", std::make_shared<NumberValue>(data->Y));
            eventObj->Set("button", std::make_shared<NumberValue>(data->Button));
            eventObj->Set("wheel", std::make_shared<NumberValue>(data->Wheel));
            return eventObj;
        }

        if (e.Data.type() == typeid(KeyEventData *)) {
            const auto data = std::any_cast<KeyEventData *>(e.Data);
            eventObj->Set("type", std::make_shared<StringValue>("keydown"));
            eventObj->Set("keyCode", std::make_shared<NumberValue>(data->VKey));
            eventObj->Set("scanCode", std::make_shared<NumberValue>(data->ScanCode));
            return eventObj;
        }

        if (e.Data.type() == typeid(KeyUpEventData *)) {
            const auto data = std::any_cast<KeyUpEventData *>(e.Data);
            eventObj->Set("type", std::make_shared<StringValue>("keyup"));
            eventObj->Set("keyCode", std::make_shared<NumberValue>(data->VKey));
            return eventObj;
        }

        if (e.Data.type() == typeid(SizeEventData *)) {
            const auto data = std::any_cast<SizeEventData *>(e.Data);
            eventObj->Set("type", std::make_shared<StringValue>("resize"));
            eventObj->Set("width", std::make_shared<NumberValue>(data->Width));
            eventObj->Set("height", std::make_shared<NumberValue>(data->Height));
            eventObj->Set("state", std::make_shared<NumberValue>(data->Type));
            return eventObj;
        }

        if (e.Data.type() == typeid(DropFilesEventData *)) {
            const auto data = std::any_cast<DropFilesEventData *>(e.Data);
            eventObj->Set("type", std::make_shared<StringValue>("drop"));
            eventObj->Set("x", std::make_shared<NumberValue>(data->X));
            eventObj->Set("y", std::make_shared<NumberValue>(data->Y));
            const auto fileList = std::make_shared<ArrayValue>(std::vector<ValuePtr>{});
            for (const auto &file: data->Files) {
                fileList->Elements.push_back(std::make_shared<StringValue>(StringKit::U16ToU8(file)));
            }
            eventObj->Set("files", fileList);
            return eventObj;
        }

        if (e.Data.type() == typeid(unsigned int)) {
            auto val = std::any_cast<unsigned int>(e.Data);
            eventObj->Set("value", std::make_shared<NumberValue>(val));
            return eventObj;
        }

        if (e.Data.type() == typeid(int)) {
            auto val = std::any_cast<int>(e.Data);
            eventObj->Set("value", std::make_shared<NumberValue>(val));
            return eventObj;
        }

        return eventObj;
    }
};


#endif //BXSCRIPT_EVENTKIT_H
