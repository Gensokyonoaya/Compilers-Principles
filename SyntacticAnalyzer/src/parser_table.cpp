#include "parser_table.h"
#include <iostream>

ParserTable::ParserTable() : grammar("./grammar.ini") {
    buildParseTable();
}

void ParserTable::buildParseTable() {
    // 遍历文法的产生式，填充预测分析表
}


bool ParserTable::isLL1Grammar() const {
    // 检查文法是否是 LL(1) 文法
    // 这里可以实现检查逻辑，例如检查 FIRST 和 FOLLOW 集是否满足 LL(1) 条件
    // 例如：检查是否有冲突的产生式
    // 这里是一个简单的示例，实际实现可能更复杂
    for (const auto& [nonTerminal, productions] : grammar.getRules()) {
        std::set<std::string> firstSet;
        for (const auto& production : productions) {
            for (const auto& symbol : production) {
                if (grammar.isTerminal(symbol)) {
                    firstSet.insert(symbol);
                    break; // 终结符，停止
                } else if (grammar.isNonTerminal(symbol)) {
                    const auto& firstOfSymbol = grammar.getFirstSet(symbol);
                    firstSet.insert(firstOfSymbol.begin(), firstOfSymbol.end());
                    if (firstOfSymbol.find("$") == firstOfSymbol.end()) {
                        break; // 非终结符且不包含 ε，停止
                    }
                }
            }
        }
        // 检查是否有冲突的产生式
        if (firstSet.size() < productions.size()) {
            // 如果 FIRST 集的大小小于产生式的数量，说明存在冲突
            // 例如：两个产生式的 FIRST 集有交集
            return false; // 存在冲突
        }
    }
    // 如果没有冲突，返回 true
    return true; 
}


void ParserTable::printParseTable() const {
    // 打印预测分析表
}

Grammar ParserTable::getGrammar() {
    return grammar;
}