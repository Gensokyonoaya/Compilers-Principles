#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

class ASTNode {
public:
    std::string value;
    std::vector<ASTNode*> children;
    
    ASTNode(const std::string& val) : value(val) {}
    void addChild(ASTNode* child) {
        children.push_back(child);
    }
};

#endif