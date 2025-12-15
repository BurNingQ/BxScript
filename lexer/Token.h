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
 * @brief    Token定义
 */

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
