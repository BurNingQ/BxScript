//
// Created by BurNingLi on 2025/9/17.
//

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
            return Token{TokenType(TokenType::FILE_END), "", 0, 0};
        }
        if (chaz.empty()) {
            continue;
        }
        if (std::isalpha(chaz[0]) || chaz[0] == '_' || chaz[0] == '$') {
            Token token{TokenType(TokenType::IDENTITY), chaz, Lexer::RowNum, RowPos};
            chaz = this->NextChar();
            while (!chaz.empty() && (std::isalnum(chaz[0]) || chaz[0] == '_' || chaz[0] == '$')) {
                token.TokenValue += chaz;
                chaz = this->NextChar();
            }
            this->RollBack();
            if (KeyWord::isKeyword(token.TokenValue)) {
                token._TokenType = TokenType(TokenType::KEYWORD);
            }
            return token;
        }
        if (std::isdigit(chaz[0])) {
            Token token{TokenType(TokenType::INT), chaz, Lexer::RowNum, RowPos};
            chaz = this->NextChar();
            while (true) {
                if (!chaz.empty() && std::isdigit(chaz[0])) {
                    token.TokenValue += chaz;
                    chaz = this->NextChar();
                    continue;
                }
                if (chaz == ".") {
                    token.TokenValue += ".";
                    token._TokenType = TokenType(TokenType::FLOAT);
                    chaz = this->NextChar();
                    continue;
                }
                break;
            }
            this->RollBack();
            return token;
        }
        if (chaz == R"(")") {
            Token token{TokenType(TokenType::STRING), "", Lexer::RowNum, RowPos};
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
                } else if (chaz == R"("\")") {
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
            Token token{TokenType(TokenType::SYMBOL), chaz, Lexer::RowNum, RowPos};
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
    return Token{TokenType(TokenType::FILE_END), "", RowNum, RowPos};
}
