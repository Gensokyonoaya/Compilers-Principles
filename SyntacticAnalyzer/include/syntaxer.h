#ifndef SYNTAXER_H
#define SYNTAXER_H

#include <vector>
#include <stack>
#include "../include/token.h"
#include "../include/parser_table.h"

class SyntaxAnalyzer {
public:
    SyntaxAnalyzer();
    void parse(const std::vector<Token>& tokens);

private:
    ParserTable parserTable; // 预测分析表

    std::stack<std::string> parseStack;
    // 其他成员变量，用于表的生成和语法分析的管理
};

#endif