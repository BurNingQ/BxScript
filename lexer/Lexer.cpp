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
 * @brief    Token读取
 */

#include "Lexer.h"

#include <iostream>

#include "common/KeyWord.h"
#include "common/Symbols.h"

std::string Lexer::NextChar() {
    if (EndOfFile) {
        return "";
    }
    if (mSourceCode.size() - 1 <= RowNum && RowPos >= mRowBuffer.size()) {
        EndOfFile = true;
        return "";
    }
    if (RowPos >= mRowBuffer.size()) {
        auto nextRow = [this] {
            RowNum++;
            RowPos = 0;
            mRowBuffer = mSourceCode.at(RowNum);
        };
        nextRow();
        while (mRowBuffer.empty() && RowNum < mSourceCode.size()) {
            nextRow();
        }
    }
    const char c = mRowBuffer.at(RowPos);
    RowPos += 1;
    std::string result(1, c);
    return result;
}

Token Lexer::NextToken() {
    while (true) {
        std::string chaz = this->NextChar();
        if (this->IsEndOfFile()) {
            return Token{TokenKind(TokenKind::END_OF_FILE), "", 0, 0};
        }
        if (chaz.empty()) {
            continue;
        }
        if (std::isalpha(chaz[0]) || chaz[0] == '_' || chaz[0] == '$') {
            Token token{TokenKind(TokenKind::IDENTITY), chaz, Lexer::RowNum, RowPos};
            // 标识符不跨行：行尾即结束，否则 `foo` 换行 `bar` 会被拼成 `foobar`
            while (!this->AtLineEnd()) {
                chaz = this->NextChar();
                if (chaz.empty() ||
                    !(std::isalnum(chaz[0]) || chaz[0] == '_' || chaz[0] == '$')) {
                    this->RollBack();
                    break;
                }
                token.TokenValue += chaz;
            }
            if (KeyWord::isKeyword(token.TokenValue)) {
                token._TokenType = TokenKind(TokenKind::KEYWORD);
            }
            return token;
        }
        if (std::isdigit(chaz[0])) {
            Token token{TokenKind(TokenKind::INT), chaz, Lexer::RowNum, RowPos};
            // 数字不跨行：行尾即结束，否则 `1` 换行 `2` 会被拼成 `12`
            if (this->AtLineEnd()) {
                return token;
            }
            chaz = this->NextChar();

            // 0x / 0b / 0o 前缀（不处理的话 "0x41" 会被切成 "0" + 标识符 "x41"）
            if (token.TokenValue == "0" && !chaz.empty()) {
                const char p = static_cast<char>(std::tolower(static_cast<unsigned char>(chaz[0])));
                if (p == 'x' || p == 'b' || p == 'o') {
                    token.TokenValue += chaz; // 变成 "0x" / "0b" / "0o"
                    const auto isDigitOfBase = [p](const char c) {
                        if (p == 'x') return std::isxdigit(static_cast<unsigned char>(c)) != 0;
                        if (p == 'b') return c == '0' || c == '1';
                        return c >= '0' && c <= '7'; // 0o
                    };
                    bool hasDigit = false;
                    while (!this->AtLineEnd()) {
                        chaz = this->NextChar();
                        if (chaz.empty() || !isDigitOfBase(chaz[0])) {
                            this->RollBack();
                            break;
                        }
                        token.TokenValue += chaz;
                        hasDigit = true;
                    }
                    if (!hasDigit) {
                        throw std::runtime_error("非法的数字字面量: '" + token.TokenValue
                                                 + "',行: " + std::to_string(RowNum) + ",列: " + std::to_string(
                                                     RowPos));
                    }
                    return token;
                }
            }

            while (true) {
                if (!chaz.empty() && std::isdigit(chaz[0])) {
                    token.TokenValue += chaz;
                } else if (chaz == ".") {
                    token.TokenValue += ".";
                    token._TokenType = TokenKind(TokenKind::FLOAT);
                } else {
                    this->RollBack();
                    break;
                }
                if (this->AtLineEnd()) {
                    break; // 行尾结束，不需要回退
                }
                chaz = this->NextChar();
            }
            return token;
        }
        if (chaz == R"(")") {
            Token token{TokenKind(TokenKind::STRING), "", Lexer::RowNum, RowPos};
            bool isEscape = false;

            // 保留\n
            const auto nextInString = [&]() -> std::string {
                const int rowBefore = RowNum;
                std::string c = this->NextChar();
                token.TokenValue.append(static_cast<size_t>(RowNum - rowBefore), '\n');
                return c;
            };

            chaz = nextInString();
            while (true) {
                if (chaz.empty()) {
                    throw std::runtime_error("字符串未闭合");
                }
                if (isEscape) {
                    if (chaz == "n") {
                        token.TokenValue += '\n'; // 换行符 (0x0A)
                    } else if (chaz == "r") {
                        token.TokenValue += '\r'; // 回车符 (0x0D)
                    } else if (chaz == "t") {
                        token.TokenValue += '\t'; // 制表符 (0x09)
                    } else if (chaz == "\\") {
                        token.TokenValue += '\\'; // 反斜杠
                    } else if (chaz == "\"") {
                        token.TokenValue += '"'; // 双引号
                    } else {
                        token.TokenValue += "\\" + chaz;
                    }
                    isEscape = false;
                } else if (chaz == "\\") {
                    isEscape = true;
                } else if (chaz == R"(")") {
                    break;
                } else {
                    token.TokenValue += chaz;
                }
                chaz = nextInString();
            }
            return token;
        }
        if (Symbols::isSymbols(chaz)) {
            Token token{TokenKind(TokenKind::SYMBOL), chaz, Lexer::RowNum, RowPos};
            // 符号不跨行：每次多字符前瞻前，若已在行尾就直接结束当前符号。
            if (chaz == "=" || chaz == ">" || chaz == "<" || chaz == "!") {
                if (this->AtLineEnd()) return token;
                chaz = this->NextChar();
                if (chaz == "=") {
                    token.TokenValue += "=";
                    // 拦截 ===
                    if (token.TokenValue == "==" || token.TokenValue == "!=") {
                        if (this->AtLineEnd()) return token;
                        chaz = this->NextChar();
                        if (chaz == "=") {
                            throw std::runtime_error("不支持的运算符: '" + token.TokenValue + "=',行: " + std::to_string(RowNum) + ",列: " + std::to_string(RowPos));
                        }
                        this->RollBack();
                    }
                } else {
                    this->RollBack();
                }
            } else if (chaz == "&") {
                if (this->AtLineEnd()) return token;
                chaz = this->NextChar();
                if (chaz == "&") {
                    token.TokenValue += "&";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "|") {
                if (this->AtLineEnd()) return token;
                chaz = this->NextChar();
                if (chaz == "|") {
                    token.TokenValue += "|";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "/") {
                if (this->AtLineEnd()) return token;
                chaz = this->NextChar();
                if (chaz == "/") {
                    this->RowPos = 999999999;
                    // return this->NextToken();
                    continue;
                }
                this->RollBack();
            } else if (chaz == "+") {
                if (this->AtLineEnd()) return token;
                chaz = this->NextChar();
                if (chaz == "+") {
                    token.TokenValue += "+";
                } else if (chaz == "=") {
                    token.TokenValue += "=";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "-") {
                if (this->AtLineEnd()) return token;
                chaz = this->NextChar();
                if (chaz == "-") {
                    token.TokenValue += "-";
                } else if (chaz == "=") {
                    token.TokenValue += "=";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "*") {
                if (this->AtLineEnd()) return token;
                chaz = this->NextChar();
                if (chaz == "=") {
                    token.TokenValue += "=";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "%") {
                if (this->AtLineEnd()) return token;
                chaz = this->NextChar();
                if (chaz == "=") {
                    token.TokenValue += "=";
                } else {
                    this->RollBack();
                }
            }
            return token;
        }
        if (std::isspace(chaz[0])) {
            continue;
        }
        throw std::runtime_error(
            "未知字符: '" + chaz + "',行: " + std::to_string(RowNum) + ",列: " + std::to_string(RowPos));
    }
    return Token{TokenKind(TokenKind::END_OF_FILE), "", RowNum, RowPos};
}
