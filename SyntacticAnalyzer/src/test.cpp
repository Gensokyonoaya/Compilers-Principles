#include "parser_table.h"
#include <iostream>

int main() {

    // 创建 ParserTable 对象
    ParserTable parserTable;

    Grammar g = parserTable.getGrammar();
    // g.printGrammar();
    // g.printFirstSets();
    // g.printFollowSets();
    // g.printSelectSets();
    
    // 打印终结符和非终结符
    std::cout << "Terminals: ";
    for (const auto& terminal : g.getTerminals()) {
        std::cout << terminal << " ";
    }
    

    // 打印预测分析表
    //parserTable.printParseTable();

    // 检查是否是 LL(1) 文法
    // if(g.isLL1Grammar()) {
    //     std::cout << "The grammar is LL(1)." << std::endl;
    // } else {
    //     std::cout << "The grammar is not LL(1)." << std::endl;
    // }

    return 0;
}