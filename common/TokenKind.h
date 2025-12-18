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
 * @brief    Token类型
 */

#ifndef BXSCRIPT_TOKENTYPE_H
#define BXSCRIPT_TOKENTYPE_H

#include <string>

class TokenKind {
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

    constexpr explicit TokenKind(const Value v) : _value(v) {
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
