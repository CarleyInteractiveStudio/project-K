#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <memory>
#include "token.hpp"
#include "ast.hpp"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<ProgramNode> parseProgram();

private:
    std::vector<Token> tokens;
    size_t pos = 0;

    const Token& peek() const;
    const Token& previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    void skipNewlines();

    StmtPtr parseStatement();
    StmtPtr parseImport();
    StmtPtr parseFunction();
    StmtPtr parseIf();
    StmtPtr parseWhile();
    StmtPtr parseReturn();
    StmtPtr parseMemWrite();
    StmtPtr parseOutb();
    StmtPtr parseAssignOrExpr();

    ExprPtr parseExpression();
    ExprPtr parseEquality();
    ExprPtr parseComparison();
    ExprPtr parseTerm();
    ExprPtr parseFactor();
    ExprPtr parsePrimary();
};

#endif
