#ifndef PARSER_TABLE_H
#define PARSER_TABLE_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include "../../LexicalAnalyzer/include/token.h"
#include "grammar.h"
#include "../../LexicalAnalyzer/include/utils.h"

using ParseTable = std::map<std::pair<std::string, std::string>, Production>; // 预测分析表类型
class ParserTable {
public:
    ParserTable();
    
    // 构建预测分析表
    void buildParseTable();

    Grammar getGrammar();

    // 获取预测分析表
    ParseTable getParseTable() const {
        return parseTable;
    }
    
    // 打印预测分析表
    void printParseTable() const;

private:
    ParseTable parseTable; // 预测分析表
    Grammar grammar; // 文法对象
};

#endif