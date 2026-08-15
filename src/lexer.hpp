#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include "token.hpp"

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos = 0;
    int line = 1;

    char peek() const;
    char advance();
    void skipWhitespaceExceptNewline();
    Token nextToken();
};

#endif
