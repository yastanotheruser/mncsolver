#include "parser.hpp"
#include <cctype>
#include <stack>

enum class OperatorTestResult {
    NoMatch, PartialMatch, AmbiguousMatch, WholeMatch
};

static OperatorTestResult testOperator(std::string t)
{
    if (t.empty()) {
        return OperatorTestResult::NoMatch;
    }

    int matches = 0;
    std::string match;

    for (const auto &entry : pUnaryOperatorMap) {
        if (entry.first.find(t) != 0) {
            continue;
        }

        matches++;
        if (matches == 1) {
            match = entry.first;
        } else if (matches == 2) {
            break;
        }
    }

    if (matches < 2) {
        std::string unaryOpMatch = match;
        for (const auto &entry : pBinaryOperatorMap) {
            if (entry.first.find(t) == 0 || entry.first == unaryOpMatch) {
                continue;
            }

            matches++;
            if (matches == 1) {
                match = entry.first;
            } else if (matches == 2) {
                break;
            }
        }
    }

    if (matches == 0) {
        return OperatorTestResult::NoMatch;
    } else if (matches == 1) {
        return t != match ? OperatorTestResult::PartialMatch :
                            OperatorTestResult::WholeMatch;
    } else {
        return OperatorTestResult::AmbiguousMatch;
    }
}

PToken parseToken(PStringIterator &pos, const PStringIterator &end)
{
    PTokenType type = PTokenType::None;
    std::string token;
    while (pos != end && std::isspace(*pos)) {
        ++pos;
    }

    if (pos == end) {
        return PToken(type, token);
    }

    char c = *pos;
    token += *pos++;
    OperatorTestResult oresult = testOperator(token);

    if (std::isdigit(c)) {
        type = PTokenType::Constant;
        for ( ; pos != end && std::isdigit(c); c = *++pos) {
            token += c;
        }
    } else if (std::isalpha(c)) {
        type = PTokenType::Variable;
        for ( ; pos != end && std::isalnum(c); c = *++pos) {
            token += c;
        }
    } else if (c == '(' || c == ')') {
        type = PTokenType::Paren;
    } else if (oresult != OperatorTestResult::NoMatch) {
        for ( ; pos != end && oresult != OperatorTestResult::WholeMatch; c = *++pos) {
            if (std::isspace(c) || std::isalnum(c)) {
                break;
            }
            token += c;
            OperatorTestResult r = testOperator(token);
            if (r == OperatorTestResult::NoMatch) {
                break;
            }
            oresult = r;
        }

        if (oresult == OperatorTestResult::PartialMatch) {
            type = PTokenType::Invalid;
        } else {
            type = PTokenType::Operator;
        }
    } else {
        type = PTokenType::Invalid;
    }

    return PToken(type, token);
}

std::queue<PToken> shuntingYard(const std::string &expr)
{
    std::queue<PToken> outputQueue;
    std::stack<PToken> opstack;
    auto it = expr.cbegin();
    auto const end = expr.cend();

    while (it != end) {
        PToken t = parseToken(it, end);
        PTokenType type = t.type;
        std::string token = t.token;

        switch (type) {
        case PTokenType::Constant:
        case PTokenType::Variable:
            outputQueue.push(t);
            break;
        case PTokenType::Operator:
            break;
        case PTokenType::Paren:
            break;
        case PTokenType::Invalid:
            break;
        default:
            break;
        }
    }

    return outputQueue;
}
