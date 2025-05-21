#ifndef PARSETREE_H
#define PARSETREE_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../../LexicalAnalyzer/include/token.h"

class ParseTreeNode {
public:
    ParseTreeNode(const std::string& sym);
    ParseTreeNode(const std::string& sym, const bool isLast);
    void printNode(int depth = 0, const std::string& prefix = "", bool isLast = true) const;
    void printNode(std::ostream& out, int depth = 0, const std::string& prefix = "", bool isLast = true) const;

    std::string symbol;
    std::vector<std::shared_ptr<ParseTreeNode>> Children;
    Token token; // 仅终结符节点有意义
    bool isLast;
};

#endif