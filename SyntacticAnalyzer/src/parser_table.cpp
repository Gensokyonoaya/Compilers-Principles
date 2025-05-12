#include "parser_table.h"
#include <iostream>

ParserTable::ParserTable() : grammar("./grammar.ini") {
    buildParseTable();
}

void ParserTable::buildParseTable() {
    const SelectSets& selectSets = grammar.getSelectSets();
    for (const auto& [prod, select] : selectSets) {
        const std::string& nonTerminal = prod.first;
        const std::vector<std::string>& rhs = prod.second;

        for (const std::string& terminal : select) {
            std::pair<std::string, std::string> key = { nonTerminal, terminal };

            // 冲突检测（预测分析表冲突说明文法非LL(1)）
            if (parseTable.count(key)) {
                DEBUG_ERROR("Conflict in parsing table!");
            } else {
                parseTable[key] = rhs;
            }
        }
    }
}

void ParserTable::printParseTable() const {
    // 打印预测分析表
    std::cout << "Predictive Parsing Table:\n";
    for (const auto& [key, production] : parseTable) {
        const std::string& nonTerminal = key.first;
        const std::string& terminal = key.second;

        std::cout << "M[" << nonTerminal << ", " << terminal << "] = ";
        for (const auto& symbol : production) {
            std::cout << symbol << " ";
        }
        std::cout << "\n";
    }
}

Grammar ParserTable::getGrammar() {
    return grammar;
}