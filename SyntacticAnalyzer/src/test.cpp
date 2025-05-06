#include "grammar.h"
#include <iostream>

int main() {
    // 指定 grammar 文件路径
    const std::string grammarFile = "d:/Homework/third/Compilers Principles/SyntacticAnalyzer/grammar.txt";

    // 创建 Grammar 对象并读取文法
    Grammar grammar(grammarFile);

    // 打印解析后的文法规则
    std::cout << "Parsed Grammar Rules:" << std::endl;
    for (const auto& [nonTerminal, productions] : grammar.getRules()) {
        std::cout << nonTerminal << " -> ";
        for (size_t i = 0; i < productions.size(); ++i) {
            for (const auto& symbol : productions[i]) {
                std::cout << symbol << " ";
            }
            if (i < productions.size() - 1) {
                std::cout << "| ";
            }
        }
        std::cout << std::endl;
    }

    // 打印终结符集合
    std::cout << "\nTerminals:" << std::endl;
    for (const auto& terminal : grammar.getTerminals()) {
        std::cout << terminal << std::endl;
    }

    // 打印非终结符集合
    std::cout << "\nNon-Terminals:" << std::endl;
    for (const auto& nonTerminal : grammar.getNonTerminals()) {
        std::cout << nonTerminal << std::endl;
    }

    // 测试是否正确识别终结符和非终结符
    std::cout << "\nTesting isTerminal and isNonTerminal:" << std::endl;
    std::vector<std::string> testSymbols = {"lorexpOp", "||", "lOrExp", "EOF"};
    for (const auto& symbol : testSymbols) {
        std::cout << "Symbol: " << symbol
                  << " | IsTerminal: " << grammar.isTerminal(symbol)
                  << " | IsNonTerminal: " << grammar.isNonTerminal(symbol)
                  << std::endl;
    }

    return 0;
}