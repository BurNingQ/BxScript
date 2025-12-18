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
            return Token{TokenKind(TokenKind::FILE_END), "", 0, 0};
        }
        if (chaz.empty()) {
            continue;
        }
        if (std::isalpha(chaz[0]) || chaz[0] == '_' || chaz[0] == '$') {
            Token token{TokenKind(TokenKind::IDENTITY), chaz, Lexer::RowNum, RowPos};
            chaz = this->NextChar();
            while (!chaz.empty() && (std::isalnum(chaz[0]) || chaz[0] == '_' || chaz[0] == '$')) {
                token.TokenValue += chaz;
                chaz = this->NextChar();
            }
            this->RollBack();
            if (KeyWord::isKeyword(token.TokenValue)) {
                token._TokenType = TokenKind(TokenKind::KEYWORD);
            }
            return token;
        }
        if (std::isdigit(chaz[0])) {
            Token token{TokenKind(TokenKind::INT), chaz, Lexer::RowNum, RowPos};
            chaz = this->NextChar();
            while (true) {
                if (!chaz.empty() && std::isdigit(chaz[0])) {
                    token.TokenValue += chaz;
                    chaz = this->NextChar();
                    continue;
                }
                if (chaz == ".") {
                    token.TokenValue += ".";
                    token._TokenType = TokenKind(TokenKind::FLOAT);
                    chaz = this->NextChar();
                    continue;
                }
                break;
            }
            this->RollBack();
            return token;
        }
        if (chaz == R"(")") {
            Token token{TokenKind(TokenKind::STRING), "", Lexer::RowNum, RowPos};
            chaz = this->NextChar();
            bool isEscape = false;
            while (true) {
                if (chaz.empty()) {
                    throw std::runtime_error("字符串未闭合");
                }
                if (isEscape) {
                    if (chaz == R"(")") {
                        token.TokenValue += R"(")";
                    } else {
                        token.TokenValue += R"(\)" + chaz;
                    }
                    isEscape = false;
                } else if (chaz == "\\") {
                    isEscape = true;
                } else if (chaz == R"(")") {
                    break;
                } else {
                    token.TokenValue += chaz;
                }
                chaz = this->NextChar();
            }
            return token;
        }
        if (Symbols::isSymbols(chaz)) {
            Token token{TokenKind(TokenKind::SYMBOL), chaz, Lexer::RowNum, RowPos};
            if (chaz == "=" || chaz == ">" || chaz == "<" || chaz == "!") {
                chaz = this->NextChar();
                if (chaz == "=") {
                    token.TokenValue += "=";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "&") {
                chaz = this->NextChar();
                if (chaz == "&") {
                    token.TokenValue += "&";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "|") {
                chaz = this->NextChar();
                if (chaz == "|") {
                    token.TokenValue += "|";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "/") {
                chaz = this->NextChar();
                if (chaz == "/") {
                    this->RowPos = 999999999;
                    // return this->NextToken();
                    continue;
                }
                this->RollBack();
            } else if (chaz == "+") {
                chaz = this->NextChar();
                if (chaz == "+") {
                    token.TokenValue += "+";
                } else if (chaz == "=") {
                    token.TokenValue += "=";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "-") {
                chaz = this->NextChar();
                if (chaz == "-") {
                    token.TokenValue += "-";
                } else if (chaz == "=") {
                    token.TokenValue += "=";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "*") {
                chaz = this->NextChar();
                if (chaz == "=") {
                    token.TokenValue += "=";
                } else {
                    this->RollBack();
                }
            } else if (chaz == "%") {
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
    return Token{TokenKind(TokenKind::FILE_END), "", RowNum, RowPos};
}
