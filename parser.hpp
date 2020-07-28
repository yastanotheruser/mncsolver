#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstdlib>
#include <string>
#include <utility>
#include <map>
#include <queue>

enum class PNodeType {
    Node, Expression, Constant, Variable, UnaryOperation, BinaryOperation
};

enum class PUnaryOperator {
    Plus, Minus, Not
};

enum class PBinaryOperator {
    Plus, Minus, Times, Over, Power
};

enum class PTokenType {
    None, Invalid, Constant, Variable, Paren, Operator
};

std::map<std::string, PUnaryOperator> pUnaryOperatorMap = {
    {"+", PUnaryOperator::Plus},
    {"-", PUnaryOperator::Minus},
    {"!", PUnaryOperator::Not},
};

std::map<std::string, std::pair<PBinaryOperator, int>> pBinaryOperatorMap = {
    {"&", {PBinaryOperator::Plus, 1}},
    {"|", {PBinaryOperator::Plus, 1}},
    {"<", {PBinaryOperator::Plus, 2}},
    {"<=", {PBinaryOperator::Plus, 2}},
    {">", {PBinaryOperator::Plus, 2}},
    {">=", {PBinaryOperator::Plus, 2}},
    {"=", {PBinaryOperator::Plus, 2}},
    {"!=", {PBinaryOperator::Plus, 2}},
    {"+", {PBinaryOperator::Plus, 3}},
    {"-", {PBinaryOperator::Minus, 3}},
    {"*", {PBinaryOperator::Times, 4}},
    {"/", {PBinaryOperator::Over, 4}},
    {"^", {PBinaryOperator::Power, 5}}
};

struct PPosition {
    explicit PPosition() { }
    explicit PPosition(std::size_t l, std::size_t c) : line{l}, column{c} { }
    explicit PPosition(std::pair<std::size_t, std::size_t> p)
        : line{p.first}, column{p.second}
    {
    }

    std::size_t line = 0;
    std::size_t column = 0;
};

struct PLocation {
    explicit PLocation() { }
    explicit PLocation(PPosition s, PPosition e, const std::string *src = nullptr)
        : start{s}, end{e}, source{src}
    {
    }

    explicit PLocation(std::pair<std::size_t, std::size_t> s,
                       std::pair<std::size_t, std::size_t> e,
                       const std::string *src = nullptr)
        : start{PPosition(s)}, end{PPosition(e)}, source{src}
    {
    }

    PPosition start;
    PPosition end;
    const std::string *source = nullptr;
};

struct PNode {
    explicit PNode() : type{PNodeType::Node} { }
    explicit PNode(PNodeType t = PNodeType::Node) : type{t} { }
    PNodeType type;
    PLocation loc;
};

struct PExpression : PNode {
    explicit PExpression(PNodeType t = PNodeType::Expression) : PNode{t} { }
};

struct PConstant : PExpression {
    explicit PConstant() : PExpression{PNodeType::Constant} { }
    explicit PConstant(double v) : PExpression{PNodeType::Constant}, value{v} { }
    double value = 0;
};

struct PVariable : PExpression {
    explicit PVariable(std::string n) : PExpression{PNodeType::Variable}, name{n}
    {
    }

    std::string name;
};

struct PUnaryOperation : PExpression {
    explicit PUnaryOperation(PUnaryOperator _op, PExpression &_arg)
        : PExpression{PNodeType::UnaryOperation}, op{_op}, arg{_arg}
    {
    }

    PUnaryOperator op;
    PExpression &arg;
};

struct PBinaryOperation : PExpression {
    explicit PBinaryOperation(PBinaryOperator _op, PExpression &l,
                              PExpression &r)
        : PExpression{PNodeType::BinaryOperation}, op{_op}, left{l}, right{r}
    {
    }

    PBinaryOperator op;
    PExpression &left;
    PExpression &right;
};

struct PToken {
    explicit PToken(PTokenType _type, std::string _token)
        : type{_type}, token{_token}
    {
    }

    PTokenType type;
    std::string token;
};

using PStringIterator = std::string::const_iterator;

PToken parseToken(PStringIterator &pos, const PStringIterator &end);
std::queue<PToken> shuntingYard(const std::string &expr);

#endif
