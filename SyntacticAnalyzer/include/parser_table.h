#ifndef PARSER_TABLE_H
#define PARSER_TABLE_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include "../include/token.h"

class ParserTable {
public:
    ParserTable();
    
    // 构建预测分析表
    void buildParseTable();
    
    // 打印预测分析表
    void printParseTable() const;


private:
    std::map<std::string, std::map<std::string, std::vector<std::string>>> parseTable;
};

#endif