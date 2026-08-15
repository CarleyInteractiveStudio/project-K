#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    TOK_EOF,
    TOK_NEWLINE,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_STRING,
    TOK_IMPORT,
    TOK_FNC,
    TOK_IF,
    TOK_WHILE,
    TOK_RETURN,
    TOK_MEM_WRITE,
    TOK_MEM_READ,
    TOK_OUTB,
    TOK_INB,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_COMMA,
    TOK_ASSIGN,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_GT,
    TOK_LT,
    TOK_EQ_EQ
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};

#endif
