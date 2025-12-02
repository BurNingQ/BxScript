//
// Created by Administrator on 2025/9/17.
//

#ifndef BXSCRIPT_TOKEN_H
#define BXSCRIPT_TOKEN_H

#include <utility>

#include "common/TokenType.h"
#include "string"

class Token {
public:
    explicit Token(const TokenType type, std::string value, const int lineNumber, const int mColsNum)
        : _TokenType(type), TokenValue(std::move(value)), LineNum(lineNumber), ColsNum(mColsNum) {
    }

    explicit Token() = default;

    std::string ToString() const {
        return "TOKEN: {\"字符\": " + this->TokenValue +
               ", \"行\": " + std::to_string(this->LineNum) +
               ", \"列\": " + std::to_string(this->ColsNum) +
               ", \"类型\": " + this->_TokenType.ToString() + "}";
    }

    TokenType _TokenType = TokenType(TokenType::NONE);
    std::string TokenValue;
    int LineNum = 0;
    int ColsNum = 0;
};

#endif //BXSCRIPT_TOKEN_H
