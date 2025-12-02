//
// Created by BurNingLi on 2025/9/19.
//

#include "Parser.h"
#include "lexer/Lexer.h"

Token Parser::NextToken() {
    if (!this->TokenQueue.empty()) {
        auto token = this->TokenQueue.front();
        this->TokenQueue.pop_front();
        return token;
    }
    return this->lexer.NextToken();
}

void Parser::BackToken(const Token &token) {
    this->TokenQueue.push_front(token);
}

Program Parser::ParserSourceCode(const std::string &code) {
    std::vector<std::unique_ptr<Statement> > body{};
    Token token = this->NextToken();
    while (token._TokenType.GetEnum() != TokenType::FILE_END) {
        body.push_back(this->ParseStatement());
        token = this->NextToken();
        if (token._TokenType.GetEnum() != TokenType::FILE_END) {
            this->BackToken(token);
        }
    }
    std::vector<std::unique_ptr<FunctionDeclaration> > DeclarationList{};
    std::vector<std::unique_ptr<ImportStatement> > imports{};
    Program pro{};
    pro.Body = std::move(body);
    pro.Imports = std::move(imports);
    return pro;
}

// import win.ui as ui;
std::unique_ptr<Statement> Parser::ParseImportStatements() {
    auto tk = this->NextToken(); // 此处tk = import
    tk = this->NextToken(); // 此处tk 应该为 identity
    if (tk._TokenType.GetEnum() != TokenType::IDENTITY) {
        Error(tk, "import语句错误");
    }
    std::string alias{};
    std::vector<std::string> body{};
    body.push_back(tk.TokenValue);
    while (true) {
        tk = this->NextToken();
        if (tk.TokenValue == ".") {
            tk = this->NextToken();
            if (tk._TokenType.GetEnum() != TokenType::IDENTITY) {
                Error(tk, "import语句错误");
            }
            body.push_back(tk.TokenValue);
            continue;
        }
        break;
    }
    if (tk.TokenValue == "as") {
        tk = this->NextToken();
        if (tk._TokenType.GetEnum() != TokenType::IDENTITY) {
            Error(tk, "import语句错误: as后应该为别名");
        }
        alias = tk.TokenValue;
        tk = this->NextToken();
        if (tk.TokenValue != ";") {
            Error(tk, "import语句错误: 应该以分号结束");
        }
    } else {
        Error(tk, "import语句错误: 必须包含别名,格式 import x.x as A;");
    }
    auto impt = make_unique<ImportStatement>(body, alias);
    this->VM->imports.push_back(std::move(impt));
    return make_unique<EmptyStatement>();
}

std::unique_ptr<Statement> Parser::ParseStatement() {
    const auto tk = this->NextToken();
    this->BackToken(tk);
    if (tk.TokenValue == "{") {
        return this->ParseBlockStatement();
    }
    if (tk.TokenValue == "if") {
        return this->ParseIfStatement();
    }
    if (tk.TokenValue == "for") {
        return this->ParseForOrForInStatement();
    }
    if (tk.TokenValue == "while") {
        return this->ParseWhileStatement();
    }
    if (tk.TokenValue == "let") {
        return this->ParseVariableStatement();
    }
    if (tk.TokenValue == "function") {
        return this->ParseFunctionStatement();
    }
    if (tk.TokenValue == "throw") {
        return this->ParseThrowStatement();
    }
    if (tk.TokenValue == "try") {
        return this->ParseTryStatement();
    }
    if (tk.TokenValue == "import") {
        return this->ParseImportStatements();
    }
    if (tk.TokenValue == "break") {
        return this->ParseBreakStatement();
    }
    if (tk.TokenValue == "continue") {
        return this->ParseContinueStatement();
    }
    if (tk.TokenValue == "return") {
        return this->ParseReturnStatement();
    }
    auto expState = make_unique<ExpressionStatement>(ParseExpression());
    this->Semicolon();
    return expState;
}

std::unique_ptr<Statement> Parser::ParseBlockStatement() {
    auto tk = this->NextToken();
    if (tk.TokenValue != "{") {
        Error(tk, "此处期望{");
    }
    std::vector<std::unique_ptr<Statement> > statementList{};
    while (true) {
        tk = this->NextToken();
        if (tk.TokenValue == "}") break;
        this->BackToken(tk);
        statementList.push_back(this->ParseStatement());
    }
    return make_unique<BlockStatement>(std::move(statementList));
}

std::unique_ptr<Statement> Parser::ParseIfStatement() {
    std::unique_ptr<Statement> ok = nullptr;
    std::unique_ptr<Statement> _else = nullptr;
    std::unique_ptr<Statement> _elseif = nullptr;

    auto tk = this->NextToken(); // if
    tk = this->NextToken();
    if (tk.TokenValue != "(") {
        Error(tk, "if语句错误: if后应该为(");
    }
    auto condition = this->ParseExpression();
    tk = this->NextToken();
    if (tk.TokenValue != ")") {
        Error(tk, "if语句错误: if后应该为)");
    }
    tk = this->NextToken();
    if (tk.TokenValue == "{") {
        this->BackToken(tk);
        ok = this->ParseBlockStatement();
    } else {
        Error(tk, "if语句错误: if至少应该包含一个分支");
    }
    const auto tk1 = this->NextToken();
    if (tk1.TokenValue == "else") {
        const auto tk2 = this->NextToken();
        if (tk2.TokenValue == "if") {
            tk = this->NextToken();
            if (tk.TokenValue == "(") {
                this->BackToken(tk);
                _elseif = this->ParseIfStatement();
            } else {
                Error(tk, "if语句错误: else if后应该为(");
            }
        } else {
            if (tk2.TokenValue == "{") {
                this->BackToken(tk2);
                _else = this->ParseBlockStatement();
            } else {
                Error(tk2, "if语句错误: else后应该为{ 或者 if");
            }
        }
    } else {
        this->BackToken(tk1);
    }
    return make_unique<IfStatement>(std::move(condition), std::move(ok), std::move(_else), std::move(_elseif));
}

std::unique_ptr<Statement> Parser::ParseForOrForInStatement() {
    this->OpenPVM()->InFor = true;
    auto tk = this->NextToken(); // for
    tk = this->NextToken(); // (
    if (tk.TokenValue != "(") {
        Error(tk, "for语句错误: for后应为(");
    }
    bool isForIn = false;
    std::vector<std::unique_ptr<Expression> > leftExpressions{};
    // 可能是let i = 0; for(;i<10;i++){}, 此处解析for()
    tk = this->NextToken();
    if (tk.TokenValue != ";") {
        if (tk.TokenValue == "let") {
            // this->BackToken(tk);
            auto vds = this->ParseVariableDeclarationList();
            tk = this->NextToken();
            if (vds.size() == 1 && tk.TokenValue == "in") {
                isForIn = true;
                leftExpressions.push_back(std::move(vds.at(0)));
            } else {
                for (auto &ve: vds) {
                    leftExpressions.push_back(std::move(ve));
                }
                vds.clear();
            }
        } else {
            this->BackToken(tk);
            leftExpressions.push_back(this->ParseExpression());
            tk = this->NextToken();
            if (tk.TokenValue == "in") {
                isForIn = true;
            }
        }
    }
    // 解析for in
    if (isForIn) {
        auto exp = std::move(leftExpressions.at(0));
        if (!dynamic_cast<Identifier *>(exp.get())
            && !dynamic_cast<DotExpression *>(exp.get())
            && !dynamic_cast<BracketExpression *>(exp.get())
            && !dynamic_cast<VariableExpression *>(exp.get())) {
            Error(tk, "for语句错误: for-in左侧表达式必须是(标识符、属性访问表达式、变量表达式)");
        }
        auto inSource = this->ParseExpression();
        tk = this->NextToken();
        if (tk.TokenValue != ")") {
            Error(tk, "此处期望得到: )");
        }
        // 解析器是否进入for 用来判断是否可以解析break, continue
        auto body = this->ParseStatement();
        this->ClosePVM();
        return make_unique<ForInStatement>(std::move(exp), std::move(inSource), std::move(body));
    }
    if (tk.TokenValue != ";") {
        Error(tk, "此处期望: ;");
    }
    auto initializer = make_unique<SequenceExpression>(std::move(leftExpressions));
    std::unique_ptr<Expression> test{};
    std::unique_ptr<Expression> updater{};
    tk = this->NextToken();
    if (tk.TokenValue != ";") {
        this->BackToken(tk);
        test = this->ParseExpression();
    }
    tk = this->NextToken();
    if (tk.TokenValue != ";") {
        Error(tk, "此处期望: ;");
    }
    tk = this->NextToken();
    if (tk.TokenValue != ")") {
        this->BackToken(tk);
        updater = this->ParseExpression();
    }
    tk = this->NextToken();
    if (tk.TokenValue != ")") {
        Error(tk, "此处期望: )");
    }
    auto body = this->ParseStatement();
    this->ClosePVM();
    return make_unique<ForStatement>(std::move(initializer), std::move(updater), std::move(test), std::move(body));
}

std::unique_ptr<Statement> Parser::ParseWhileStatement() {
    this->OpenPVM()->InFor = true;
    auto tk = this->NextToken(); // while
    tk = this->NextToken();
    if (tk.TokenValue != "(") {
        Error(tk, "此处期望: (");
    }
    auto condition = this->ParseExpression();
    tk = this->NextToken();
    if (tk.TokenValue != ")") {
        Error(tk, "此处期望: )");
    }
    auto body = this->ParseStatement();
    this->ClosePVM();
    return make_unique<ForStatement>(nullptr, nullptr, std::move(condition), std::move(body));
}

// 通过BranchStatement的token内容判断是Break还是Continue
std::unique_ptr<BreakStatement> Parser::ParseBreakStatement() {
    auto tk = this->NextToken();
    if (!this->VM->InFor) {
        Error(tk, "break应在for语句中");
    }
    if (tk.TokenValue != "break") {
        Error(tk, "此处期望: break,");
    }
    tk = this->NextToken();
    if (tk.TokenValue != ";") {
        if (tk.TokenValue != "}") {
            Error(tk, "break后不应存在其他表达式");
        } else {
            this->BackToken(tk);
        }
    }
    return make_unique<BreakStatement>();
}

// 通过BranchStatement的token内容判断是Break还是Continue
std::unique_ptr<ContinueStatement> Parser::ParseContinueStatement() {
    auto tk = this->NextToken();
    if (!this->VM->InFor) {
        Error(tk, "continue应在for语句中");
    }
    if (tk.TokenValue != "continue") {
        Error(tk, "此处期望: continue");
    }
    tk = this->NextToken();
    if (tk.TokenValue != ";") {
        if (tk.TokenValue != "}") {
            Error(tk, "continue后不应存在其他表达式");
        } else {
            this->BackToken(tk);
        }
    }
    return make_unique<ContinueStatement>();
}

std::unique_ptr<ReturnStatement> Parser::ParseReturnStatement() {
    auto tk = this->NextToken();
    if (!this->VM->InFunc) {
        Error(tk, "return应在function语句中");
    }
    if (tk.TokenValue != "return") {
        Error(tk, "此处期望: return");
    }
    std::unique_ptr<Expression> arg = nullptr; // 默认为空
    tk = this->NextToken();
    if (tk.TokenValue == ";" || tk.TokenValue == "}") {
        if (tk.TokenValue == ";") {
        } else {
            this->BackToken(tk);
        }
    } else {
        this->BackToken(tk);
        arg = this->ParseExpression();
        tk = this->NextToken();
        if (tk.TokenValue == ";") {
        } else if (tk.TokenValue == "}") {
            this->BackToken(tk);
        } else {
            Error(tk, "return语句应以;或}结束");
        }
    }
    return make_unique<ReturnStatement>(std::move(arg));
}

std::unique_ptr<Statement> Parser::ParseVariableStatement() {
    const auto tk = this->NextToken();
    if (tk.TokenValue != "let") {
        Error(tk, "此处期望: let");
    }
    auto vars = this->ParseVariableDeclarationList();
    auto varState = make_unique<VariableStatement>(std::move(vars));
    this->Semicolon();
    return std::move(varState);
}

std::unique_ptr<Statement> Parser::ParseFunctionStatement() {
    auto func = make_unique<FunctionStatement>(this->ParseFunction(false));
    return func;
}

std::unique_ptr<ParameterList> Parser::ParseParameterList() {
    auto params = std::vector<std::unique_ptr<Expression> >();
    auto tk = this->NextToken();
    if (tk.TokenValue == "(") {
        tk = this->NextToken();
        while (tk.TokenValue != ")") {
            this->BackToken(tk);
            auto exp = this->ParsePrimaryExpression();
            params.push_back(std::move(exp));
            tk = this->NextToken();
            if (tk.TokenValue != ")") {
                if (tk.TokenValue != ",") {
                    Error(tk, "参数需以,分割");
                }
                tk = this->NextToken();
            }
        }
    } else {
        Error(tk, "参数列表应该以(开始");
    }
    return make_unique<ParameterList>(std::move(params));
}

std::unique_ptr<FunctionLiteral> Parser::ParseFunction(const bool isAnonymous) {
    auto tk = this->NextToken();
    if (tk.TokenValue != "function") {
        Error(tk, "此处期望: function");
    }
    tk = this->NextToken();
    auto name = make_unique<Identifier>("");
    if (tk._TokenType.GetEnum() == TokenType::IDENTITY) {
        if (isAnonymous) {
            Error(tk, "声明式函数需要函数名");
        }
        this->BackToken(tk);
        name = std::move(this->ParseIdentifier());
    } else {
        this->BackToken(tk);
    }
    auto params = this->ParseParameterList();
    auto body = this->ParseFunctionBlock();
    return make_unique<FunctionLiteral>(std::move(name), std::move(params), std::move(body));
}

std::unique_ptr<Statement> Parser::ParseFunctionBlock() {
    this->OpenPVM();
    this->VM->InFunc = true;
    auto body = this->ParseBlockStatement();
    this->ClosePVM();
    return std::move(body);
}

std::unique_ptr<Statement> Parser::ParseThrowStatement() {
    const auto tk = this->NextToken();
    if (tk.TokenValue != "throw") {
        Error(tk, "此处期望: throw");
    }
    auto throwState = make_unique<ThrowStatement>(this->ParseExpression());
    this->Semicolon();
    return std::move(throwState);
}

std::unique_ptr<Statement> Parser::ParseTryStatement() {
    auto tk = this->NextToken();
    if (tk.TokenValue != "try") {
        Error(tk, "此处期望: try");
    }
    // 解析try
    auto tryBody = this->ParseBlockStatement();
    std::unique_ptr<Identifier> catchParam{};
    std::unique_ptr<Statement> catchBody{};
    std::unique_ptr<Statement> finally{};
    tk = this->NextToken();
    if (tk.TokenValue != "catch") {
        Error(tk, "此处期望: catch");
    } else {
        tk = this->NextToken();
        if (tk.TokenValue != "(") {
            Error(tk, "此处期望: (");
        } else {
            tk = this->NextToken();
            if (tk._TokenType.GetEnum() != TokenType::IDENTITY) {
                Error(tk, "此处期望: 标识符");
            } else {
                this->BackToken(tk);
            }
            catchParam = std::move(this->ParseIdentifier());
            tk = this->NextToken();
            if (tk.TokenValue != ")") {
                Error(tk, "此处期望: )");
            } else {
                catchBody = std::move(this->ParseBlockStatement());
            }
        }
    }
    tk = this->NextToken();
    if (tk.TokenValue == "finally") {
        finally = std::move(this->ParseBlockStatement());
    } else {
        this->BackToken(tk);
    }
    auto _catch = make_unique<CatchStatement>(std::move(catchParam), std::move(catchBody));
    return make_unique<TryStatement>(std::move(tryBody), std::move(_catch), std::move(finally));
}

std::unique_ptr<Expression> Parser::ParseExpression() {
    auto next = this->ParseAssignmentExpression();
    auto tk = this->NextToken();
    if (tk.TokenValue == ",") {
        auto sequence = std::vector<std::unique_ptr<Expression> >{};
        sequence.push_back(std::move(next));
        while (true) {
            if (tk.TokenValue != ",") {
                break;
            }
            tk = this->NextToken();
            sequence.push_back(std::move(this->ParseAssignmentExpression()));
        }
        return make_unique<SequenceExpression>(std::move(sequence));
    }
    this->BackToken(tk);
    return std::move(next);
}

std::unique_ptr<Identifier> Parser::ParseIdentifier() {
    auto tk = this->NextToken();
    return make_unique<Identifier>(tk.TokenValue);
}

std::unique_ptr<Expression> Parser::ParsePrimaryExpression() {
    auto tk = this->NextToken();
    if (tk._TokenType.GetEnum() == TokenType::IDENTITY) {
        return make_unique<Identifier>(tk.TokenValue);
    }
    if (tk._TokenType.GetEnum() == TokenType::STRING) {
        return make_unique<StringLiteral>(tk.TokenValue);
    }
    if (tk._TokenType.GetEnum() == TokenType::INT || tk._TokenType.GetEnum() == TokenType::FLOAT) {
        return make_unique<NumberLiteral>(tk.TokenValue);
    }
    if (tk.TokenValue == "{") {
        this->BackToken(tk);
        return this->ParseObjectLiteral();
    }
    if (tk.TokenValue == "[") {
        this->BackToken(tk);
        return this->ParseArrayLiteral();
    }
    if (tk.TokenValue == "(") {
        auto exp = this->ParseExpression();
        tk = this->NextToken();
        if (tk.TokenValue != ")") {
            Error(tk, "此处期望: )");
        }
        return exp;
    }
    if (tk._TokenType.GetEnum() == TokenType::KEYWORD) {
        if (tk.TokenValue == "null") {
            return make_unique<NullLiteral>("null");
        }
        if (tk.TokenValue == "true" || tk.TokenValue == "false") {
            return make_unique<BooleanLiteral>(tk.TokenValue, tk.TokenValue == "true");
        }
        if (tk.TokenValue == "this") {
            return make_unique<ThisExpression>();
        }
        if (tk.TokenValue == "function") {
            this->BackToken(tk);
            return this->ParseFunction(true);
        }
    }
    return make_unique<BadExpression>();
}

std::unique_ptr<VariableExpression> Parser::ParseVariableDeclaration() {
    auto tk = this->NextToken();
    if (tk._TokenType.GetEnum() != TokenType::IDENTITY) {
        Error(tk, "此处期望: 标识符");
    }
    auto literal = tk.TokenValue;
    std::unique_ptr<Expression> initializer = nullptr;
    tk = this->NextToken();
    if (tk.TokenValue == "=") {
        initializer = this->ParseAssignmentExpression();
    } else {
        this->BackToken(tk);
    }
    return make_unique<VariableExpression>(std::move(literal), std::move(initializer));
}


std::vector<std::unique_ptr<Expression> > Parser::ParseVariableDeclarationList() {
    auto exps = std::vector<std::unique_ptr<Expression> >{};
    while (true) {
        auto exp = this->ParseVariableDeclaration();
        exps.push_back(std::move(exp));
        auto tk = this->NextToken();
        if (tk.TokenValue != ",") {
            this->BackToken(tk);
            break;
        }
    }
    return exps;
}

std::string Parser::ParseObjectPropertyKey() {
    return this->NextToken().TokenValue;
}

std::unique_ptr<Property> Parser::ParseObjectProperty() {
    auto k = this->ParseObjectPropertyKey();
    const auto tk = this->NextToken();
    if (tk.TokenValue != ":") {
        Error(tk, "此处期望: :");
    }
    return make_unique<Property>(std::move(k), std::move(this->ParseAssignmentExpression()));
}

std::unique_ptr<Expression> Parser::ParseObjectLiteral() {
    std::vector<std::unique_ptr<Property> > props{};
    auto tk = this->NextToken();
    if (tk.TokenValue != "{") {
        Error(tk, "此处期望: {");
    }
    tk = this->NextToken();
    if (tk.TokenValue != "}") {
        this->BackToken(tk);
        while (tk.TokenValue != "}") {
            props.push_back(std::move(this->ParseObjectProperty()));
            tk = this->NextToken();
            if (tk.TokenValue == ",") {
                tk = this->NextToken();
                if (tk.TokenValue == "}") {
                    Error(tk, "此处期望: 标识符");
                } else {
                    this->BackToken(tk);
                }
            }
        }
    }
    return make_unique<ObjectLiteral>(std::move(props));
}

std::unique_ptr<Expression> Parser::ParseArrayLiteral() {
    std::vector<std::unique_ptr<Expression> > exps{};
    auto tk = this->NextToken(); // [
    tk = this->NextToken();
    while (tk.TokenValue != "]") {
        if (tk.TokenValue == ",") {
            tk = this->NextToken();
            continue;
        }
        this->BackToken(tk);
        auto exp = this->ParseAssignmentExpression();
        exps.push_back(std::move(exp));
        tk = this->NextToken();
        if (tk.TokenValue != "," && tk.TokenValue != "]") {
            Error(tk, "此处期望: ,或]");
        }
    }
    return make_unique<ArrayLiteral>(std::move(exps));
}

std::vector<std::unique_ptr<Expression> > Parser::ParseArgumentList() {
    std::vector<std::unique_ptr<Expression> > exps{};
    auto tk = this->NextToken();
    if (tk.TokenValue != "(") {
        Error(tk, "此处期望: (");
    }
    tk = this->NextToken();
    if (tk.TokenValue != ")") {
        while (true) {
            this->BackToken(tk);
            auto exp = this->ParseAssignmentExpression();
            exps.push_back(std::move(exp));
            tk = this->NextToken();
            if (tk.TokenValue != ",") {
                break;
            }
            tk = this->NextToken();
        }
    }
    return exps;
}

std::unique_ptr<Expression> Parser::ParseCallExpression(std::unique_ptr<Expression> left) {
    auto args = this->ParseArgumentList();
    return make_unique<CallExpression>(std::move(left), std::move(args));
}

std::unique_ptr<Expression> Parser::ParseDotMember(std::unique_ptr<Expression> left) {
    auto tk = this->NextToken();
    if (tk.TokenValue != ".") {
        Error(tk, "此处期望: .");
    }
    tk = this->NextToken();
    if (tk._TokenType.GetEnum() != TokenType::IDENTITY) {
        Error(tk, "此处期望: 标识符");
    }
    return make_unique<DotExpression>(std::move(left), make_unique<Identifier>(std::move(tk.TokenValue)));
}

std::unique_ptr<Expression> Parser::ParseBracketMember(std::unique_ptr<Expression> left) {
    auto tk = this->NextToken();
    if (tk.TokenValue != "[") {
        Error(tk, "此处期望: [");
    }
    auto m = this->ParseExpression();
    tk = this->NextToken();
    if (tk.TokenValue != "]") {
        Error(tk, "此处期望: ]");
    }
    return make_unique<BracketExpression>(std::move(left), std::move(m));
}

std::unique_ptr<Expression> Parser::ParseLeftHandSideExpressionAllowCall() {
    auto left = this->ParsePrimaryExpression();
    while (true) {
        auto tk = this->NextToken();
        this->BackToken(tk);
        if (tk.TokenValue == ".") {
            left = this->ParseDotMember(std::move(left));
        } else if (tk.TokenValue == "[") {
            left = this->ParseBracketMember(std::move(left));
        } else if (tk.TokenValue == "(") {
            left = this->ParseCallExpression(std::move(left));
        } else {
            break;
        }
    }
    return std::move(left);
}

std::unique_ptr<Expression> Parser::ParsePostfixExpression() {
    auto operand = this->ParseLeftHandSideExpressionAllowCall();
    const auto tk = this->NextToken();
    if (tk.TokenValue == "++" || tk.TokenValue == "--") {
        if (!dynamic_cast<Identifier *>(operand.get()) &&
            !dynamic_cast<DotExpression *>(operand.get()) &&
            !dynamic_cast<BracketExpression *>(operand.get())) {
            Error(tk, "不支持的表达式");
        }
        return make_unique<UnaryExpression>(tk, std::move(operand), true);
    }
    this->BackToken(tk);
    return std::move(operand);
}

std::unique_ptr<Expression> Parser::ParseUnaryExpression() {
    auto tk = this->NextToken();
    if (tk.TokenValue == "!" || tk.TokenValue == "+" || tk.TokenValue == "-" || tk.TokenValue == "delete") {
        return make_unique<UnaryExpression>(tk, std::move(this->ParseUnaryExpression()), false);
    }
    if (tk.TokenValue == "++" || tk.TokenValue == "--") {
        auto operand = this->ParseUnaryExpression();
        if (!dynamic_cast<Identifier *>(operand.get()) &&
            !dynamic_cast<DotExpression *>(operand.get()) &&
            !dynamic_cast<BracketExpression *>(operand.get())) {
            Error(tk, "不支持的表达式");
        }
        return make_unique<UnaryExpression>(tk, std::move(operand), true);
    }
    this->BackToken(tk);
    return std::move(this->ParsePostfixExpression());
}

std::unique_ptr<Expression> Parser::ParseMultiplicativeExpression() {
    auto left = this->ParseUnaryExpression();
    auto tk = this->NextToken();
    if (tk.TokenValue == "*" || tk.TokenValue == "/" || tk.TokenValue == "%") {
        while (true) {
            left = make_unique<BinaryExpression>(tk, std::move(left), std::move(this->ParseUnaryExpression()), false);
            tk = this->NextToken();
            if (tk.TokenValue != "*" && tk.TokenValue != "/" && tk.TokenValue != "%") {
                this->BackToken(tk);
                break;
            }
        }
    } else {
        this->BackToken(tk);
    }
    return std::move(left);
}

std::unique_ptr<Expression> Parser::ParseAdditiveExpression() {
    auto left = this->ParseMultiplicativeExpression();
    auto tk = this->NextToken();
    if (tk.TokenValue == "+" || tk.TokenValue == "-") {
        while (true) {
            left = make_unique<BinaryExpression>(tk, std::move(left), std::move(this->ParseMultiplicativeExpression()),
                                                 false);
            tk = this->NextToken();
            if (tk.TokenValue != "+" && tk.TokenValue != "-") {
                this->BackToken(tk);
                break;
            }
        }
    } else {
        this->BackToken(tk);
    }
    return std::move(left);
}

std::unique_ptr<Expression> Parser::ParseShiftExpression() {
    auto left = this->ParseAdditiveExpression();
    auto tk = this->NextToken();
    if (tk.TokenValue == "<<" || tk.TokenValue == ">>") {
        while (true) {
            left = make_unique<BinaryExpression>(tk, std::move(left), std::move(this->ParseAdditiveExpression()),
                                                 false);
            tk = this->NextToken();
            if (tk.TokenValue != "<<" && tk.TokenValue != ">>") {
                this->BackToken(tk);
                break;
            }
        }
    } else {
        this->BackToken(tk);
    }
    return std::move(left);
}

std::unique_ptr<Expression> Parser::ParseRelationalExpression() {
    auto left = this->ParseShiftExpression();
    auto tk = this->NextToken();
    if (tk.TokenValue == "<" || tk.TokenValue == "<=" || tk.TokenValue == ">" || tk.TokenValue == ">=") {
        return make_unique<BinaryExpression>(tk, std::move(left), std::move(this->ParseRelationalExpression()), true);
    }
    this->BackToken(tk);
    return std::move(left);
}

std::unique_ptr<Expression> Parser::ParseEqualityExpression() {
    auto left = this->ParseRelationalExpression();
    auto tk = this->NextToken();
    if (tk.TokenValue == "==" || tk.TokenValue == "!=") {
        while (true) {
            left = make_unique<BinaryExpression>(tk, std::move(left), std::move(this->ParseRelationalExpression()),
                                                 true);
            tk = this->NextToken();
            if (tk.TokenValue != "!=" && tk.TokenValue != "==") {
                this->BackToken(tk);
                break;
            }
        }
    } else {
        this->BackToken(tk);
    }
    return std::move(left);
}

std::unique_ptr<Expression> Parser::ParseLogicalAndExpression() {
    auto left = this->ParseEqualityExpression();
    auto tk = this->NextToken();
    if (tk.TokenValue == "&&") {
        while (true) {
            left = make_unique<BinaryExpression>(tk, std::move(left), std::move(this->ParseRelationalExpression()),
                                                 false);
            tk = this->NextToken();
            if (tk.TokenValue != "&&") {
                this->BackToken(tk);
                break;
            }
        }
    } else {
        this->BackToken(tk);
    }
    return std::move(left);
}

std::unique_ptr<Expression> Parser::ParseLogicalOrExpression() {
    auto left = this->ParseLogicalAndExpression();
    auto tk = this->NextToken();
    if (tk.TokenValue == "||") {
        while (true) {
            left = make_unique<BinaryExpression>(tk, std::move(left), std::move(this->ParseLogicalAndExpression()),
                                                 false);
            tk = this->NextToken();
            if (tk.TokenValue != "||") {
                this->BackToken(tk);
                break;
            }
        }
    } else {
        this->BackToken(tk);
    }
    return std::move(left);
}

std::unique_ptr<Expression> Parser::ParseConditionExpression() {
    auto left = this->ParseLogicalOrExpression();
    auto tk = this->NextToken();
    if (tk.TokenValue == "?") {
        auto ok = this->ParseAssignmentExpression();
        tk = this->NextToken();
        if (tk.TokenValue == ":") {
            Error(tk, "此处期望: :");
        }
        return make_unique<ConditionalExpression>(std::move(left), std::move(ok),
                                                  std::move(this->ParseAssignmentExpression()));
    }
    this->BackToken(tk);
    return std::move(left);
}

std::unique_ptr<Expression> Parser::ParseAssignmentExpression() {
    auto left = this->ParseConditionExpression();
    std::string oper{};
    auto tk = this->NextToken();
    if (tk.TokenValue == "=") {
        oper = "=";
    } else if (tk.TokenValue == "+=") {
        oper = "+";
    } else if (tk.TokenValue == "-=") {
        oper = "-";
    } else if (tk.TokenValue == "*=") {
        oper = "*";
    } else if (tk.TokenValue == "/=") {
        oper = "/";
    } else if (tk.TokenValue == "%=") {
        oper = "%";
    } else {
        this->BackToken(tk);
    }
    if (!oper.empty()) {
        if (!dynamic_cast<Identifier *>(left.get()) &&
            !dynamic_cast<DotExpression *>(left.get()) &&
            !dynamic_cast<BracketExpression *>(left.get())) {
            Error(tk, "不支持的表达式");
        }
        return make_unique<AssignExpression>(tk, std::move(left), std::move(this->ParseAssignmentExpression()));
    }
    return left;
}

Program Parser::ParseProgram() {
    std::vector<std::unique_ptr<Statement> > body;
    while (true) {
        auto tk = this->NextToken();
        if (tk._TokenType.GetEnum() == TokenType::FILE_END) {
            break;
        }
        this->BackToken(tk);
        auto stmt = this->ParseStatement();
        if (auto exprStmt = dynamic_cast<ExpressionStatement *>(stmt.get())) {
            if (dynamic_cast<BadExpression *>(exprStmt->Expression.get())) {
                tk = this->NextToken();
                continue;
            }
        }
        body.push_back(std::move(stmt));
    }
    return Program{
        std::move(body),
        std::move(this->VM->imports)
    };
}
