#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "finite_automaton.h"
#include "utils.h"
#include <string>
#include <vector>


class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    void print_tokens(); // 输出 Token 序列
    const std::vector<Token>& get_tokens() const { return tokens; } // 获取 Token 序列
    const std::string& get_source_code() const { return source_code; } // 获取源代码

private:
    std::string source_code;
    size_t current_position;
    char current_char;
    FiniteAutomaton fa;
    std::vector<Token> tokens; // 存储 Token 序列

    void advance();
    void skip_whitespace();
    Token get_next_token();
    void initialize_automaton();
};
#endif