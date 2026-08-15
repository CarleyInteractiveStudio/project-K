#include "parser.hpp"
#include <iostream>
#include <cstdlib>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

const Token& Parser::peek() const {
    if (pos >= tokens.size()) return tokens.back();
    return tokens[pos];
}

const Token& Parser::previous() const {
    if (pos == 0) return tokens[0];
    return tokens[pos - 1];
}

Token Parser::advance() {
    if (pos < tokens.size()) {
        pos++;
    }
    return previous();
}

bool Parser::check(TokenType type) const {
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::skipNewlines() {
    while (check(TokenType::TOK_NEWLINE)) {
        advance();
    }
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto prog = std::make_unique<ProgramNode>();

    while (!check(TokenType::TOK_EOF)) {
        skipNewlines();
        if (check(TokenType::TOK_EOF)) break;

        if (check(TokenType::TOK_IMPORT)) {
            prog->imports.push_back(std::unique_ptr<ImportStmt>(static_cast<ImportStmt*>(parseImport().release())));
        } else if (check(TokenType::TOK_FNC)) {
            prog->functions.push_back(std::unique_ptr<FunctionDecl>(static_cast<FunctionDecl*>(parseFunction().release())));
        } else {
            prog->topLevelStatements.push_back(parseStatement());
        }
    }

    return prog;
}

StmtPtr Parser::parseImport() {
    advance(); // consume 'import'
    std::string path;
    if (check(TokenType::TOK_IDENTIFIER) || check(TokenType::TOK_STRING)) {
        path = advance().value;
    } else {
        std::cerr << "Parser error: expected filename after import at line " << peek().line << "\n";
        exit(1);
    }

    // append .dk if not provided
    if (path.find('.') == std::string::npos) {
        path += ".dk";
    }

    return std::make_unique<ImportStmt>(path);
}

StmtPtr Parser::parseFunction() {
    advance(); // consume 'fnc'
    if (!check(TokenType::TOK_IDENTIFIER)) {
        std::cerr << "Parser error: expected function name at line " << peek().line << "\n";
        exit(1);
    }
    std::string fnName = advance().value;

    if (!match(TokenType::TOK_LPAREN)) {
        std::cerr << "Parser error: expected '(' after function name at line " << peek().line << "\n";
        exit(1);
    }

    std::vector<std::string> params;
    if (!check(TokenType::TOK_RPAREN)) {
        do {
            if (check(TokenType::TOK_IDENTIFIER)) {
                params.push_back(advance().value);
            } else {
                std::cerr << "Parser error: expected parameter name at line " << peek().line << "\n";
                exit(1);
            }
        } while (match(TokenType::TOK_COMMA));
    }

    if (!match(TokenType::TOK_RPAREN)) {
        std::cerr << "Parser error: expected ')' after parameters at line " << peek().line << "\n";
        exit(1);
    }

    skipNewlines();

    if (!match(TokenType::TOK_LBRACE)) {
        std::cerr << "Parser error: expected '{' before function body at line " << peek().line << "\n";
        exit(1);
    }

    std::vector<StmtPtr> body;
    while (!check(TokenType::TOK_RBRACE) && !check(TokenType::TOK_EOF)) {
        skipNewlines();
        if (check(TokenType::TOK_RBRACE)) break;
        body.push_back(parseStatement());
    }

    if (!match(TokenType::TOK_RBRACE)) {
        std::cerr << "Parser error: expected '}' after function body at line " << peek().line << "\n";
        exit(1);
    }

    return std::make_unique<FunctionDecl>(fnName, params, std::move(body));
}

StmtPtr Parser::parseStatement() {
    skipNewlines();
    if (check(TokenType::TOK_IF)) return parseIf();
    if (check(TokenType::TOK_WHILE)) return parseWhile();
    if (check(TokenType::TOK_RETURN)) return parseReturn();
    if (check(TokenType::TOK_MEM_WRITE)) return parseMemWrite();
    if (check(TokenType::TOK_OUTB)) return parseOutb();

    return parseAssignOrExpr();
}

StmtPtr Parser::parseIf() {
    advance(); // consume 'if'
    if (!match(TokenType::TOK_LPAREN)) {
        std::cerr << "Parser error: expected '(' after if at line " << peek().line << "\n";
        exit(1);
    }
    auto cond = parseExpression();
    if (!match(TokenType::TOK_RPAREN)) {
        std::cerr << "Parser error: expected ')' after condition at line " << peek().line << "\n";
        exit(1);
    }

    skipNewlines();
    if (!match(TokenType::TOK_LBRACE)) {
        std::cerr << "Parser error: expected '{' after if condition at line " << peek().line << "\n";
        exit(1);
    }

    std::vector<StmtPtr> thenBranch;
    while (!check(TokenType::TOK_RBRACE) && !check(TokenType::TOK_EOF)) {
        skipNewlines();
        if (check(TokenType::TOK_RBRACE)) break;
        thenBranch.push_back(parseStatement());
    }

    if (!match(TokenType::TOK_RBRACE)) {
        std::cerr << "Parser error: expected '}' after if body at line " << peek().line << "\n";
        exit(1);
    }

    return std::make_unique<IfStmt>(std::move(cond), std::move(thenBranch));
}

StmtPtr Parser::parseWhile() {
    advance(); // consume 'while'
    if (!match(TokenType::TOK_LPAREN)) {
        std::cerr << "Parser error: expected '(' after while at line " << peek().line << "\n";
        exit(1);
    }
    auto cond = parseExpression();
    if (!match(TokenType::TOK_RPAREN)) {
        std::cerr << "Parser error: expected ')' after condition at line " << peek().line << "\n";
        exit(1);
    }

    skipNewlines();
    if (!match(TokenType::TOK_LBRACE)) {
        std::cerr << "Parser error: expected '{' after while condition at line " << peek().line << "\n";
        exit(1);
    }

    std::vector<StmtPtr> body;
    while (!check(TokenType::TOK_RBRACE) && !check(TokenType::TOK_EOF)) {
        skipNewlines();
        if (check(TokenType::TOK_RBRACE)) break;
        body.push_back(parseStatement());
    }

    if (!match(TokenType::TOK_RBRACE)) {
        std::cerr << "Parser error: expected '}' after while body at line " << peek().line << "\n";
        exit(1);
    }

    return std::make_unique<WhileStmt>(std::move(cond), std::move(body));
}

StmtPtr Parser::parseReturn() {
    advance(); // consume 'return'
    ExprPtr val = nullptr;
    if (!check(TokenType::TOK_NEWLINE) && !check(TokenType::TOK_RBRACE) && !check(TokenType::TOK_EOF)) {
        val = parseExpression();
    }
    return std::make_unique<ReturnStmt>(std::move(val));
}

StmtPtr Parser::parseMemWrite() {
    advance(); // consume 'mem_write'
    if (!match(TokenType::TOK_LPAREN)) {
        std::cerr << "Parser error: expected '(' after mem_write at line " << peek().line << "\n";
        exit(1);
    }
    auto addr = parseExpression();
    if (!match(TokenType::TOK_COMMA)) {
        std::cerr << "Parser error: expected ',' in mem_write at line " << peek().line << "\n";
        exit(1);
    }
    auto val = parseExpression();
    if (!match(TokenType::TOK_RPAREN)) {
        std::cerr << "Parser error: expected ')' after mem_write args at line " << peek().line << "\n";
        exit(1);
    }

    return std::make_unique<MemWriteStmt>(std::move(addr), std::move(val));
}

StmtPtr Parser::parseOutb() {
    advance(); // consume 'outb'
    if (!match(TokenType::TOK_LPAREN)) {
        std::cerr << "Parser error: expected '(' after outb at line " << peek().line << "\n";
        exit(1);
    }
    auto port = parseExpression();
    if (!match(TokenType::TOK_COMMA)) {
        std::cerr << "Parser error: expected ',' in outb at line " << peek().line << "\n";
        exit(1);
    }
    auto val = parseExpression();
    if (!match(TokenType::TOK_RPAREN)) {
        std::cerr << "Parser error: expected ')' after outb args at line " << peek().line << "\n";
        exit(1);
    }

    return std::make_unique<OutbStmt>(std::move(port), std::move(val));
}

StmtPtr Parser::parseAssignOrExpr() {
    if (check(TokenType::TOK_IDENTIFIER) && pos + 1 < tokens.size() && tokens[pos + 1].type == TokenType::TOK_ASSIGN) {
        std::string varName = advance().value;
        advance(); // consume '='
        auto val = parseExpression();
        return std::make_unique<AssignStmt>(varName, std::move(val));
    }

    auto expr = parseExpression();
    return std::make_unique<ExprStmt>(std::move(expr));
}

ExprPtr Parser::parseExpression() {
    return parseEquality();
}

ExprPtr Parser::parseEquality() {
    auto left = parseComparison();
    while (match(TokenType::TOK_EQ_EQ)) {
        std::string op = previous().value;
        auto right = parseComparison();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::parseComparison() {
    auto left = parseTerm();
    while (check(TokenType::TOK_GT) || check(TokenType::TOK_LT)) {
        advance();
        std::string op = previous().value;
        auto right = parseTerm();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::parseTerm() {
    auto left = parseFactor();
    while (check(TokenType::TOK_PLUS) || check(TokenType::TOK_MINUS)) {
        advance();
        std::string op = previous().value;
        auto right = parseFactor();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::parseFactor() {
    auto left = parsePrimary();
    while (check(TokenType::TOK_STAR) || check(TokenType::TOK_SLASH)) {
        advance();
        std::string op = previous().value;
        auto right = parsePrimary();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::parsePrimary() {
    if (match(TokenType::TOK_NUMBER)) {
        uint64_t val = std::strtoull(previous().value.c_str(), nullptr, 0);
        return std::make_unique<NumberExpr>(val);
    }

    if (match(TokenType::TOK_STRING)) {
        return std::make_unique<StringExpr>(previous().value);
    }

    if (match(TokenType::TOK_MEM_READ)) {
        if (!match(TokenType::TOK_LPAREN)) {
            std::cerr << "Parser error: expected '(' after mem_read at line " << peek().line << "\n";
            exit(1);
        }
        auto addr = parseExpression();
        if (!match(TokenType::TOK_RPAREN)) {
            std::cerr << "Parser error: expected ')' after mem_read at line " << peek().line << "\n";
            exit(1);
        }
        return std::make_unique<MemReadExpr>(std::move(addr));
    }

    if (match(TokenType::TOK_INB)) {
        if (!match(TokenType::TOK_LPAREN)) {
            std::cerr << "Parser error: expected '(' after inb at line " << peek().line << "\n";
            exit(1);
        }
        auto port = parseExpression();
        if (!match(TokenType::TOK_RPAREN)) {
            std::cerr << "Parser error: expected ')' after inb at line " << peek().line << "\n";
            exit(1);
        }
        return std::make_unique<InbExpr>(std::move(port));
    }

    if (match(TokenType::TOK_IDENTIFIER)) {
        std::string name = previous().value;
        if (match(TokenType::TOK_LPAREN)) {
            std::vector<ExprPtr> args;
            if (!check(TokenType::TOK_RPAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::TOK_COMMA));
            }
            if (!match(TokenType::TOK_RPAREN)) {
                std::cerr << "Parser error: expected ')' after argument list at line " << peek().line << "\n";
                exit(1);
            }
            return std::make_unique<CallExpr>(name, std::move(args));
        }
        return std::make_unique<VariableExpr>(name);
    }

    if (match(TokenType::TOK_LPAREN)) {
        auto expr = parseExpression();
        if (!match(TokenType::TOK_RPAREN)) {
            std::cerr << "Parser error: expected ')' after expression at line " << peek().line << "\n";
            exit(1);
        }
        return expr;
    }

    std::cerr << "Parser error: unexpected token '" << peek().value << "' at line " << peek().line << "\n";
    exit(1);
}
