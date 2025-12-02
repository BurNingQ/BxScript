//
// Created by BurNingLi on 2025/11/21.
//

#ifndef BXSCRIPT_ENVIRONMENT_H
#define BXSCRIPT_ENVIRONMENT_H
#include "Value.h"

class Environment : public std::enable_shared_from_this<Environment> {
public:
    std::shared_ptr<Environment> parent;
    std::unordered_map<std::string, ValuePtr> variables;

    explicit Environment(std::shared_ptr<Environment> p = nullptr) : parent(std::move(p)) {
    }

    ValuePtr DeclareVar(const std::string &name, ValuePtr value) {
        if (variables.find(name) != variables.end()) {
            throw std::runtime_error("变量重复定义: " + name);
        }
        variables[name] = value;
        return value;
    }

    ValuePtr AssignVar(const std::string &name, ValuePtr value) {
        if (variables.find(name) != variables.end()) {
            variables[name] = value;
            return value;
        }
        if (parent) {
            return parent->AssignVar(name, value);
        }
        throw std::runtime_error("变量未定义: " + name);
    }

    ValuePtr LookupVar(const std::string &name) {
        if (variables.find(name) != variables.end()) {
            return variables[name];
        }
        if (parent) {
            return parent->LookupVar(name);
        }
        throw std::runtime_error("变量未定义: " + name);
    }
};


#endif //BXSCRIPT_ENVIRONMENT_H
