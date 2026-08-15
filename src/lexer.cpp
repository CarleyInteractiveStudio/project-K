#include "lexer.hpp"
#include <cctype>
#include <iostream>

Lexer::Lexer(std::string source) : source(std::move(source)) {}

char Lexer::peek() const {
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    if (pos >= source.length()) return '\0';
    return source[pos++];
}

void Lexer::skipWhitespaceExceptNewline() {
    while (pos < source.length()) {
        char c = source[pos];
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '#') { // single line comment
            while (pos < source.length() && source[pos] != '\n') {
                advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::nextToken() {
    skipWhitespaceExceptNewline();

    if (pos >= source.length()) {
        return {TokenType::TOK_EOF, "", line};
    }

    char c = peek();

    if (c == '\n') {
        advance();
        int curLine = line++;
        return {TokenType::TOK_NEWLINE, "\n", curLine};
    }

    if (std::isalpha(c) || c == '_') {
        std::string val;
        while (std::isalnum(peek()) || peek() == '_' || peek() == '.') {
            val += advance();
        }
        if (val == "import") return {TokenType::TOK_IMPORT, val, line};
        if (val == "fnc" || val == "fn") return {TokenType::TOK_FNC, val, line};
        if (val == "if") return {TokenType::TOK_IF, val, line};
        if (val == "while") return {TokenType::TOK_WHILE, val, line};
        if (val == "return") return {TokenType::TOK_RETURN, val, line};
        if (val == "mem_write") return {TokenType::TOK_MEM_WRITE, val, line};
        if (val == "mem_read") return {TokenType::TOK_MEM_READ, val, line};
        if (val == "outb") return {TokenType::TOK_OUTB, val, line};
        if (val == "inb") return {TokenType::TOK_INB, val, line};
        return {TokenType::TOK_IDENTIFIER, val, line};
    }

    if (std::isdigit(c)) {
        std::string val;
        if (c == '0' && pos + 1 < source.length() && (source[pos + 1] == 'x' || source[pos + 1] == 'X')) {
            val += advance(); // '0'
            val += advance(); // 'x'
            while (std::isxdigit(peek())) {
                val += advance();
            }
        } else {
            while (std::isdigit(peek())) {
                val += advance();
            }
        }
        return {TokenType::TOK_NUMBER, val, line};
    }

    if (c == '"') {
        advance(); // consume '"'
        std::string val;
        while (pos < source.length() && peek() != '"') {
            if (peek() == '\\') {
                advance();
                if (peek() == 'n') { val += '\n'; advance(); }
                else if (peek() == 'r') { val += '\r'; advance(); }
                else if (peek() == 't') { val += '\t'; advance(); }
                else if (peek() == '0') { val += '\0'; advance(); }
                else { val += advance(); }
            } else {
                val += advance();
            }
        }
        if (peek() == '"') advance();
        return {TokenType::TOK_STRING, val, line};
    }

    advance();
    switch (c) {
        case '(': return {TokenType::TOK_LPAREN, "(", line};
        case ')': return {TokenType::TOK_RPAREN, ")", line};
        case '{': return {TokenType::TOK_LBRACE, "{", line};
        case '}': return {TokenType::TOK_RBRACE, "}", line};
        case ',': return {TokenType::TOK_COMMA, ",", line};
        case '+': return {TokenType::TOK_PLUS, "+", line};
        case '-': return {TokenType::TOK_MINUS, "-", line};
        case '*': return {TokenType::TOK_STAR, "*", line};
        case '/': return {TokenType::TOK_SLASH, "/", line};
        case '=':
            if (peek() == '=') { advance(); return {TokenType::TOK_EQ_EQ, "==", line}; }
            return {TokenType::TOK_ASSIGN, "=", line};
        case '>': return {TokenType::TOK_GT, ">", line};
        case '<': return {TokenType::TOK_LT, "<", line};
        default:
            std::cerr << "Lexer error: unexpected character '" << c << "' at line " << line << "\n";
            return {TokenType::TOK_EOF, "", line};
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token tok = nextToken();
        tokens.push_back(tok);
        if (tok.type == TokenType::TOK_EOF) break;
    }
    return tokens;
}
