#include "parser_table.h"
#include <iostream>

ParserTable::ParserTable() : grammar("./grammar.ini") {
    buildParseTable();
}

void ParserTable::buildParseTable() {
    // 遍历文法的产生式，填充预测分析表
}

void ParserTable::printParseTable() const {
    // 打印预测分析表
}

Grammar ParserTable::getGrammar() {
    return grammar;
}