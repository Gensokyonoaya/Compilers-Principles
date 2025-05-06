#include "grammar.h"
#include <iostream>

int main() {
    // 指定 grammar 文件路径
    const std::string grammarFile = "d:/Homework/third/Compilers Principles/SyntacticAnalyzer/grammar.ini";

    // 创建 Grammar 对象并读取文法
    Grammar grammar(grammarFile);

    // 打印文法规则
    grammar.printGrammar();

    // 获取起始符号
    std::string startSymbol = grammar.getStartSymbol();
    std::cout << "Start Symbol: " << startSymbol << std::endl;

    // 获取终结符和非终结符集合
    std::set<std::string> terminals = grammar.getTerminals();
    std::set<std::string> nonTerminals = grammar.getNonTerminals();
    std::cout << "Terminals: ";
    for (const auto& t : terminals) {
        std::cout << t << " ";
    }
    std::cout << std::endl;
    std::cout << "Non-Terminals: ";
    for (const auto& nt : nonTerminals) {
        std::cout << nt << " ";
    }
    std::cout << std::endl;

    // 打印 FIRST 集
    grammar.printFirstSets();

    return 0;
}