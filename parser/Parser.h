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
 * @brief    语义解析器定义
 */

#ifndef BXSCRIPT_PARSER_H
#define BXSCRIPT_PARSER_H
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <vector>

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

inline bool IsSymbol(const Token &tk, const std::string &symbol) {
    return tk._TokenType.GetEnum() == TokenKind::SYMBOL && tk.TokenValue == symbol;
}

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

    std::unique_ptr<Statement> ParseDeferStatement();

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

    /**
     * 语句结束。合法结尾：;、}、文件结束，或换行（下一个 token 在更靠后的行）。
     * 分号会被消费；其余 token 原样放回，交给外层（块结束、if/else、下一条语句）。
     */
    void Semicolon() {
        const int prevLine = LastConsumedLine(); // 必须在 NextToken 之前取
        const auto tk = this->NextToken();
        if (IsSymbol(tk, ";")) {
            return; // 消费分号
        }
        if (IsSymbol(tk, "}") ||
            tk._TokenType.GetEnum() == TokenKind::END_OF_FILE ||
            (prevLine >= 0 && tk.LineNum > prevLine)) {
            this->BackToken(tk); // } / EOF / 下一行的 token 交还外层
            return;
        }
        Error(tk, "此处期望: ; 或换行");
    }

private:
    std::deque<Token> TokenQueue{};
    // NextToken 压栈、BackToken 弹栈；栈顶 = 最近一次真正保留的 token 所在行。
    // 不变量：所有 BackToken 都是对最近一次 NextToken 的 LIFO 回退。
    std::vector<int> ConsumedLines{};
    int LastConsumedLine() const {
        return ConsumedLines.empty() ? -1 : ConsumedLines.back();
    }
    Lexer lexer;
};


#endif //BXSCRIPT_PARSER_H
