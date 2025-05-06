#include "parser_table.h"
#include <iostream>

ParserTable::ParserTable() {
    buildParseTable();
}

void ParserTable::buildParseTable() {

}
void ParserTable::printParseTable() const {
    // 打印预测分析表的逻辑
    for (const auto& entry : parseTable) {
        const std::string& nonTerminal = entry.first;
        const auto& productions = entry.second;
        std::cout << nonTerminal << ":\n";
        for (const auto& production : productions) {
            std::cout << "  " << production.first << " -> ";
            for (const auto& symbol : production.second) {
                std::cout << symbol << " ";
            }
            std::cout << "\n";
        }
    }
}