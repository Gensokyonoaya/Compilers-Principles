#include "syntaxer.h"
#include "../../LexicalAnalyzer/include/lexer.h"
#include <iostream>

int main() {

    std::string source_code = R"(
        int main() {
            float x = 3.14;
            const int y = 42;
            if (x == y) {
                x = 0;
            } else if (x != y) {
                if (x <= y) {
                    x = y;
                } else {
                    if (x == -1)
                        if (y == 0)
                            x = 0;
                        else
                            x = 1;
                }
            } else {
                x = 1;
            }
            if (x >= 0) return x;
        }
    )";

    Lexer lexer(source_code);

    SyntaxAnalyzer syntaxAnalyzer(lexer.get_tokens());
    syntaxAnalyzer.printSteps();

    // 创建 ParserTable 对象
    //ParserTable parserTable;

    //Grammar g = parserTable.getGrammar();
    // g.printGrammar();
    //g.printFirstSets();
    //g.printFollowSets();
    //g.printSelectSets();
    
    // 打印终结符和非终结符
    // std::cout << "Terminals: ";
    // for (const auto& terminal : g.getTerminals()) {
    //     std::cout << terminal << " ";
    // }
    

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