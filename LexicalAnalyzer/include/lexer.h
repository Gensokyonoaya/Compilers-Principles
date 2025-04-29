#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "finite_automaton.h"
#include <string>
#include <vector>


class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    void print_tokens(); // 输出 Token 序列

private:
    std::string source_code;
    size_t current_position;
    char current_char;
    FiniteAutomaton fa;

    void advance();
    void skip_whitespace();
    Token get_next_token();
    void initialize_automaton();
};
#endif