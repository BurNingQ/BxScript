//
// Created by BurNingLi on 2025/9/19.
//

#ifndef BXSCRIPT_PARSER_H
#define BXSCRIPT_PARSER_H
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>

#include "Expression.h"
#include "ParserVM.h"
#include "lexer/Lexer.h"
#include "lexer/Token.h"

#if __cplusplus < 201402L
// C++11
template<typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args &&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#else
// C++14+
using std::make_unique;
#endif

class Parser {
public:
    ~Parser() {
        while (this->VM) {
            const auto old = this->VM;
            this->VM = this->VM->OuterVM;
            delete old;
        }
    }

    explicit Parser(const std::string &sourceCode) : lexer(sourceCode) {
        this->VM = new ParserVM();
        this->VM->OuterVM = nullptr;
    }

    ParserVM *VM{};

    Token NextToken();

    void BackToken(const Token &token);

    Program ParserSourceCode(const std::string &code);

    void ClosePVM() {
        if (!this->VM) return;
        const auto oldVm = this->VM;
        this->VM = oldVm->OuterVM;
        delete oldVm;
    }

    ParserVM *OpenPVM() {
        const auto oldPvm = this->VM;
        this->VM = new ParserVM();
        this->VM->OuterVM = oldPvm;
        return this->VM;
    }

    static void Error(const Token &token, const std::string &message) {
        std::ostringstream oss;
        oss << "\033[1;31m[语法错误]\033[0m " << message << "\n"
                << "  位于第 " << token.LineNum + 1 << " 行，第 " << token.ColsNum + 1 << " 列";
        if (!token.TokenValue.empty()) {
            oss << "，出错的符号为：‘" << token.TokenValue << "’";
        }
        oss << "\n";
        if (!token.TokenValue.empty()) {
            int spaceCount = token.ColsNum > 0 ? token.ColsNum : 0;
            oss << "  " << token.TokenValue << "\n"
                    << "  " << std::string(spaceCount, ' ') << "↑\n";
        }
        std::cerr << oss.str() << std::endl;
        throw std::runtime_error(message);
    }

    std::unique_ptr<Statement> ParseImportStatements();

    std::unique_ptr<Statement> ParseBlockStatement();

    // std::vector<Statement> ParseStatements();

    std::unique_ptr<Statement> ParseStatement();

    std::unique_ptr<Statement> ParseIfStatement();

    // std::unique_ptr<Statement> ParseForStatement();
    //
    // std::unique_ptr<Statement> ParseForInStatement();

    // std::unique_ptr<Statement> ParseWhileStatement();

    std::unique_ptr<Statement> ParseForOrForInStatement();

    std::unique_ptr<Statement> ParseWhileStatement();

    Program ParseProgram();

    std::unique_ptr<BreakStatement> ParseBreakStatement();

    std::unique_ptr<ContinueStatement> ParseContinueStatement();

    std::unique_ptr<ReturnStatement> ParseReturnStatement();

    std::unique_ptr<Statement> ParseThrowStatement();

    std::unique_ptr<Statement> ParseTryStatement();

    std::unique_ptr<Statement> ParseVariableStatement();

    std::unique_ptr<Statement> ParseFunctionStatement();

    std::unique_ptr<FunctionLiteral> ParseFunction(bool isAnonymous);

    std::unique_ptr<ParameterList> ParseParameterList();

    std::unique_ptr<Statement> ParseFunctionBlock();

    // function的body尝试用ParseBlockStatement
    // FunctionLiteral ParseFunctionLiteral();

    // 以下为表达式解析

    std::unique_ptr<Expression> ParseExpression();

    std::unique_ptr<Identifier> ParseIdentifier();

    std::unique_ptr<Expression> ParsePrimaryExpression();

    std::unique_ptr<VariableExpression> ParseVariableDeclaration();

    std::vector<std::unique_ptr<Expression> > ParseVariableDeclarationList();

    std::string ParseObjectPropertyKey();

    std::unique_ptr<Property> ParseObjectProperty();

    std::unique_ptr<Expression> ParseObjectLiteral();

    std::unique_ptr<Expression> ParseArrayLiteral();

    std::vector<std::unique_ptr<Expression> > ParseArgumentList();

    std::unique_ptr<Expression> ParseCallExpression(std::unique_ptr<Expression> left);

    std::unique_ptr<Expression> ParseDotMember(std::unique_ptr<Expression> left);

    std::unique_ptr<Expression> ParseBracketMember(std::unique_ptr<Expression> left);

    std::unique_ptr<Expression> ParseLeftHandSideExpressionAllowCall();

    std::unique_ptr<Expression> ParsePostfixExpression();

    std::unique_ptr<Expression> ParseUnaryExpression();

    std::unique_ptr<Expression> ParseMultiplicativeExpression();

    std::unique_ptr<Expression> ParseAdditiveExpression();

    std::unique_ptr<Expression> ParseShiftExpression();

    std::unique_ptr<Expression> ParseRelationalExpression();

    std::unique_ptr<Expression> ParseEqualityExpression();

    std::unique_ptr<Expression> ParseLogicalAndExpression();

    std::unique_ptr<Expression> ParseLogicalOrExpression();

    std::unique_ptr<Expression> ParseConditionExpression();

    std::unique_ptr<Expression> ParseAssignmentExpression();

    void Semicolon() {
        const auto tk = this->NextToken();
        if (tk.TokenValue == ";" || tk._TokenType.GetEnum() == TokenType::FILE_END) {
        } else {
            this->BackToken(tk);
        }
    }

private:
    std::deque<Token> TokenQueue{};
    Lexer lexer;
};


#endif //BXSCRIPT_PARSER_H
