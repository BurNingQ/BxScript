//
// Created by BurNingLi on 2025/9/17.
//
#ifndef BXSCRIPT_TOKENTYPE_H
#define BXSCRIPT_TOKENTYPE_H

#include <string>

class TokenType {
public:
    enum Value {
        IDENTITY,
        KEYWORD,
        FLOAT,
        INT,
        STRING,
        SYMBOL,
        FILE_END,
        LINE_END,
        NONE,
    };

    constexpr explicit TokenType(const Value v) : _value(v) {
    }

    std::string ToString() const {
        switch (_value) {
            case IDENTITY:
                return "IDENTITY";
            case KEYWORD:
                return "KEYWORD";
            case FLOAT:
                return "FLOAT";
            case INT:
                return "INT";
            case STRING:
                return "STRING";
            case SYMBOL:
                return "SYMBOL";
            case FILE_END:
                return "EOF";
            case LINE_END:
                return "EOL";
            case NONE:
                return "NONE";
            default:
                return "UNKNOWN";
        }
    }

    Value GetEnum() const { return _value; }

private:
    Value _value;
};

#endif //BXSCRIPT_TOKENTYPE_H
