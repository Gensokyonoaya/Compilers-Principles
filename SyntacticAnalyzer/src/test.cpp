#include "parser_table.h"
#include <iostream>

int main() {

    // 创建 ParserTable 对象
    ParserTable parserTable;

    Grammar g = parserTable.getGrammar();
    g.printGrammar();
    g.printFirstSets();
    g.printFollowSets();
    g.printSelectSets();
    

    // 打印预测分析表
    //std::cout << "===== Predictive Parsing Table =====" << std::endl;
    //parserTable.printParseTable();

    // 检查是否是 LL(1) 文法
    if(g.isLL1Grammar()) {
        std::cout << "The grammar is LL(1)." << std::endl;
    } else {
        std::cout << "The grammar is not LL(1)." << std::endl;
    }

    return 0;
}