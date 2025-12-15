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
 * @brief    表达式定义等
 */

#ifndef BXSCRIPT_EXPRESSION_H
#define BXSCRIPT_EXPRESSION_H
#include <utility>
#include <vector>

#include "lexer/Token.h"

class Expression {
public:
    virtual ~Expression() = default;
};

class Statement {
public:
    virtual ~Statement() = default;
};

class Declaration {
public:
    virtual ~Declaration() = default;
};

class ArrayLiteral : public Expression {
public:
    explicit ArrayLiteral(std::vector<std::unique_ptr<Expression> > v) : Value(std::move(v)) {
    }

    std::vector<std::unique_ptr<Expression> > Value{};
};

class AssignExpression : public Expression {
public:
    explicit AssignExpression(Token _operator, std::unique_ptr<Expression> left,
                              std::unique_ptr<Expression> right) : Operator(std::move(_operator)),
                                                                   Left(std::move(left)), Right(std::move(right)) {
    }

    Token Operator{};
    std::unique_ptr<Expression> Left{};
    std::unique_ptr<Expression> Right{};
};

class BadExpression : public Expression {
public:
    explicit BadExpression() = default;
};

class BinaryExpression : public Expression {
public:
    explicit BinaryExpression(Token _operator, std::unique_ptr<Expression> _left, std::unique_ptr<Expression> _right,
                              bool comparison) : Operator(std::move(_operator)), Left(std::move(_left)),
                                                 Right(std::move(_right)),
                                                 Comparison(comparison) {
    }

    Token Operator{};
    std::unique_ptr<Expression> Left{}, Right{};
    bool Comparison = false;
};

class BooleanLiteral : public Expression {
public:
    explicit BooleanLiteral(std::string literal, const bool v) : Literal(std::move(literal)), Value(v) {
    }

    std::string Literal{};
    bool Value = false;
};

class BracketExpression : public Expression {
public:
    explicit BracketExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> m) : Left(std::move(l)),
        Member(std::move(m)) {
    }

    std::unique_ptr<Expression> Left{}, Member{};
};

class CallExpression : public Expression {
public:
    explicit CallExpression(std::unique_ptr<Expression> c,
                            std::vector<std::unique_ptr<Expression> > args) : Callee(std::move(c)),
                                                                              ArgumentList(std::move(args)) {
    }

    std::unique_ptr<Expression> Callee{};
    std::vector<std::unique_ptr<Expression> > ArgumentList;
};

class ConditionalExpression : public Expression {
public:
    explicit ConditionalExpression(std::unique_ptr<Expression> test, std::unique_ptr<Expression> ok,
                                   std::unique_ptr<Expression> _else) : Test(std::move(test)), Ok(std::move(ok)),
                                                                        Else(std::move(_else)) {
    }

    std::unique_ptr<Expression> Test{}, Ok{}, Else{};
};

class Identifier : public Expression {
public:
    explicit Identifier(std::string name) : Name{std::move(name)} {
    };
    std::string Name{};
};

class DotExpression : public Expression {
public:
    explicit DotExpression(std::unique_ptr<Expression> l, std::unique_ptr<Identifier> id) : Left(std::move(l)),
        Identifier(std::move(id)) {
    }

    std::unique_ptr<Expression> Left{};
    std::unique_ptr<Identifier> Identifier{};
};

class EmptyExpression : public Expression {
public:
    int Begin = 0, End = 0;
};

class ParameterList : public Expression {
public:
    explicit ParameterList(std::vector<std::unique_ptr<Expression> > params) : Parameters(std::move(params)) {
    }

    std::vector<std::unique_ptr<Expression> > Parameters;
};

class FunctionLiteral : public Expression {
public:
    explicit FunctionLiteral(std::unique_ptr<Identifier> _name,
                             std::unique_ptr<ParameterList> _params,
                             std::unique_ptr<Statement> _body)
        : Name(std::move(_name)), Parameters(std::move(_params)), Body(std::move(_body)) {
    }

    std::unique_ptr<Identifier> Name{};
    std::unique_ptr<ParameterList> Parameters{};
    std::unique_ptr<Statement> Body{};
};

class NullLiteral : public Expression {
public:
    explicit NullLiteral(std::string v) : Literal(std::move(v)) {
    }

    std::string Literal{};
};

class NumberLiteral : public Expression {
public:
    explicit NumberLiteral(std::string value) : Literal(std::move(value)) {
    }

    std::string Literal{};
};

class Property : public Expression {
public:
    explicit Property(std::string k, std::unique_ptr<Expression> v) : Key(std::move(k)), Value(std::move(v)) {
    }

    std::string Key{};
    std::unique_ptr<Expression> Value{};
};

class ObjectLiteral : public Expression {
public:
    explicit ObjectLiteral(std::vector<std::unique_ptr<Property> > v) : Value(std::move(v)) {
    }

    std::vector<std::unique_ptr<Property> > Value{};
};

class SequenceExpression : public Expression {
public:
    explicit SequenceExpression(std::vector<std::unique_ptr<Expression> > _sequence) : Sequence(std::move(_sequence)) {
    }

    std::vector<std::unique_ptr<Expression> > Sequence{};
};

class StringLiteral : public Expression {
public:
    explicit StringLiteral(std::string _literal) : Literal(std::move(_literal)) {
    }

    std::string Literal{};
};

class ThisExpression : public Expression {
public:
    explicit ThisExpression() {
    }
};

class UnaryExpression : public Expression {
public:
    explicit UnaryExpression(Token _operator, std::unique_ptr<Expression> operand,
                             bool postfix) : Operator(std::move(_operator)),
                                             Operand(std::move(operand)),
                                             Postfix(postfix) {
    }

    Token Operator{};
    std::unique_ptr<Expression> Operand{};
    bool Postfix = false;
};

class VariableExpression : public Expression {
public:
    explicit VariableExpression(std::string name, std::unique_ptr<Expression> initializer) : Name(std::move(name)),
        Initializer(std::move(initializer)) {
    }

    std::string Name{};
    std::unique_ptr<Expression> Initializer{};
};

// ================== Statement ==================

class BadStatement : public Statement {
public:
    int From = 0, To = 0;
};

class EmptyStatement : public Statement {
public:
    explicit EmptyStatement() {
    }
};

class BlockStatement : public Statement {
public:
    explicit BlockStatement(std::vector<std::unique_ptr<Statement> > _list)
        : StatementList(std::move(_list)) {
    }

    int LeftBrace = 0, RightBrace = 0;
    std::vector<std::unique_ptr<Statement> > StatementList;
};

// class BranchStatement : public Statement {
// public:
//     explicit BranchStatement(Token _token) : Token(std::move(_token)) {
//     }
//
//     std::string Label{};
//     Token Token{};
// };

class CatchStatement : public Statement {
public:
    explicit CatchStatement(std::unique_ptr<Identifier> param,
                            std::unique_ptr<Statement> state) : Parameter(std::move(param)),
                                                                Body(std::move(state)) {
    }

    std::unique_ptr<Identifier> Parameter{};
    std::unique_ptr<Statement> Body{};
};

class ExpressionStatement : public Statement {
public:
    explicit ExpressionStatement(std::unique_ptr<Expression> expression)
        : Expression(std::move(expression)) {
    }

    std::unique_ptr<Expression> Expression{};
};

class ForInStatement : public Statement {
public:
    explicit ForInStatement(std::unique_ptr<Expression> _into, std::unique_ptr<Expression> _source,
                            std::unique_ptr<Statement> _body)
        : Into(std::move(_into)), Source(std::move(_source)), Body(std::move(_body)) {
    }

    std::unique_ptr<Expression> Into{}, Source{};
    std::unique_ptr<Statement> Body{};
};

class ForStatement : public Statement {
public:
    explicit ForStatement(std::unique_ptr<Expression> _initializer,
                          std::unique_ptr<Expression> _update,
                          std::unique_ptr<Expression> _test,
                          std::unique_ptr<Statement> _body) : Initializer(std::move(_initializer)),
                                                              Update(std::move(_update)),
                                                              Test(std::move(_test)),
                                                              Body(std::move(_body)) {
    }

    std::unique_ptr<Expression> Initializer{}, Update{}, Test{};
    std::unique_ptr<Statement> Body{};
};

class FunctionStatement : public Statement {
public:
    explicit FunctionStatement(std::unique_ptr<FunctionLiteral> _func) : Function(std::move(_func)) {
    }

    std::unique_ptr<FunctionLiteral> Function{};
};

class IfStatement : public Statement {
public:
    explicit IfStatement(
        std::unique_ptr<Expression> _condition,
        std::unique_ptr<Statement> _ok,
        std::unique_ptr<Statement> _else = nullptr,
        std::unique_ptr<Statement> _elseIf = nullptr
    )
        : Condition(std::move(_condition)), Ok(std::move(_ok)), Else(std::move(_else)), ElseIf(std::move(_elseIf)) {
    }

    std::unique_ptr<Expression> Condition{};
    std::unique_ptr<Statement> Ok{}, Else{}, ElseIf{};
};

class LabelStatement : public Statement {
public:
    std::unique_ptr<Identifier> Label{};
    std::unique_ptr<Statement> Statement{};
};

class ReturnStatement : public Statement {
public:
    explicit ReturnStatement(std::unique_ptr<Expression> _arg) : Argument(std::move(_arg)) {
    }

    std::unique_ptr<Expression> Argument{};
};

class BreakStatement : public Statement {
public:
    explicit BreakStatement() = default;
};

class ContinueStatement : public Statement {
public:
    explicit ContinueStatement() = default;
};

class ThrowStatement : public Statement {
public:
    explicit ThrowStatement(std::unique_ptr<Expression> _arg) : Argument(std::move(_arg)) {
    }

    std::unique_ptr<Expression> Argument{};
};

class TryStatement : public Statement {
public:
    explicit TryStatement(std::unique_ptr<Statement> body, std::unique_ptr<CatchStatement> _catch,
                          std::unique_ptr<Statement> _finally) : Body(std::move(body)),
                                                                 Catch(std::move(_catch)),
                                                                 Finally(std::move(_finally)) {
    }

    std::unique_ptr<Statement> Body{};
    std::unique_ptr<CatchStatement> Catch{};
    std::unique_ptr<Statement> Finally{};
};

class VariableStatement : public Statement {
public:
    explicit VariableStatement(std::vector<std::unique_ptr<Expression> > _variable) : List(std::move(_variable)) {
    }

    std::vector<std::unique_ptr<Expression> > List{};
};

class ImportStatement : public Statement {
public:
    explicit ImportStatement(std::vector<std::string> _path, std::string _aliasName)
        : Path(std::move(_path)), AliasName(std::move(_aliasName)) {
    }

    std::vector<std::string> Path;
    std::string AliasName{};
};

// =================== Declaration =================== //

class FunctionDeclaration : public Declaration {
public:
    explicit FunctionDeclaration(std::unique_ptr<FunctionLiteral> _function) : Function(std::move(_function)) {
    }

    std::unique_ptr<FunctionLiteral> Function{};
};

class VariableDeclaration : public Declaration {
public:
    std::vector<std::unique_ptr<VariableExpression> > List{};
};

class Program {
public:
    explicit Program() = default;

    explicit Program(std::vector<std::unique_ptr<Statement> > _body,
                     std::vector<std::unique_ptr<ImportStatement> > _import) : Body(std::move(_body)),
                                                                               Imports(std::move(_import)) {
    }

    std::vector<std::unique_ptr<Statement> > Body{};
    std::vector<std::unique_ptr<ImportStatement> > Imports{};
};

#endif //BXSCRIPT_EXPRESSION_H
