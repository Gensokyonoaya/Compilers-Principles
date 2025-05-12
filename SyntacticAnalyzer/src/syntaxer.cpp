#include "syntaxer.h"

SyntaxAnalyzer::SyntaxAnalyzer() {
    // 初始化语法分析器
    parserTable = ParserTable();
}

void SyntaxAnalyzer::parse(const std::vector<Token>& tokens) {
    // 语法分析的实现
    // 预测分析法
    parseStack.push("EOF"); // 添加结束符
    parseStack.push(parserTable.getGrammar().getStartSymbol()); // 添加文法的开始符号
    size_t index = 0; // 当前分析的token索引
    Token currentToken = tokens[index]; // 当前token
    // while (!parseStack.empty()) {
    //     std::string top = parseStack.top(); // 栈顶元素
    //     parseStack.pop(); // 弹出栈顶元素

    //     if (top == currentToken.getType()) { // 如果栈顶元素是终结符
    //         if (top == "EOF") {
    //             // 语法分析成功
    //             std::cout << "Syntax analysis successful!" << std::endl;
    //             return;
    //         }
    //         index++;
    //         if (index < tokens.size()) {
    //             currentToken = tokens[index];
    //         }
    //     } else if (parserTable.getParseTable().count({top, currentToken.getType()})) { // 如果栈顶元素是非终结符
    //         Production production = parserTable.getParseTable().at({top, currentToken.getType()});
    //         for (auto it = production.rbegin(); it != production.rend(); ++it) {
    //             parseStack.push(*it); // 将产生式右部逆序压入栈中
    //         }
    //     } else {
    //         std::cerr << "Syntax error at token: " << currentToken.getValue() << std::endl;
    //         return;
    //     }
    // }
}