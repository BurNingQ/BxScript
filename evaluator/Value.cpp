#include "Value.h"

#include "Logger.h"

std::shared_ptr<ObjectValue> StringValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> NumberValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> BoolValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> ArrayValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> FunctionValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> ObjectValue::Prototype = std::make_shared<ObjectValue>();

ValuePtr RuntimeValue::Get(const std::string &key) {
    Logger::Error("类型错误,不能获取属性'" + key + "' 来自: " + this->ToString());
    return std::make_shared<NullValue>();
}

void RuntimeValue::Set(const std::string &key, ValuePtr value) {
    Logger::Error("类型错误,不能设置属性'" + key + "' 来自: " + this->ToString());
}

bool RuntimeValue::Equal(ValuePtr v) {
    return false;
}