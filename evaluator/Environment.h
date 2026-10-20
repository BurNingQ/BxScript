/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025-12-15
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    执行环境，上下文切换，GC等
 */

#ifndef BXSCRIPT_ENVIRONMENT_H
#define BXSCRIPT_ENVIRONMENT_H
#include "Value.h"
#include "../parser/Expression.h"

class Environment : public std::enable_shared_from_this<Environment> {
public:
    std::shared_ptr<Environment> parent;
    std::unordered_map<std::string, ValuePtr> variables;
    std::vector<Statement *> defers;
    bool isFunctionBoundary = false;

    explicit Environment(std::shared_ptr<Environment> p = nullptr) : parent(std::move(p)) {
    }

    ValuePtr DeclareVar(const std::string &name, ValuePtr value) {
        // if (variables.find(name) != variables.end()) {
        //     throw std::runtime_error("变量重复定义: " + name);
        // }
        variables[name] = value;
        return value;
    }

    /**
     * 声明变量，已存在则覆盖（不做重复定义检查）。
     * 用于函数声明：同名 function 允许重复，后者生效，且同一声明重复执行不应报错。
     */
    ValuePtr DefineVar(const std::string &name, ValuePtr value) {
        variables[name] = std::move(value);
        return variables[name];
    }

    ValuePtr AssignVar(const std::string &name, ValuePtr value) {
        if (variables.find(name) != variables.end()) {
            variables[name] = value;
            return value;
        }
        if (parent) {
            return parent->AssignVar(name, value);
        }
        variables[name] = value;
        return value;
        // throw std::runtime_error("变量未定义: " + name);
    }

    ValuePtr LookupVar(const std::string &name) {
        if (variables.find(name) != variables.end()) {
            return variables[name];
        }
        if (parent) {
            return parent->LookupVar(name);
        }
        return std::make_shared<NullValue>();
        // throw std::runtime_error("变量未定义: " + name);
    }
};


#endif //BXSCRIPT_ENVIRONMENT_H
