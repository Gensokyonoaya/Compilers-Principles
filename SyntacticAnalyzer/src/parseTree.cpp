#include "parseTree.h"

ParseTreeNode::ParseTreeNode(const std::string& sym) : symbol(sym), token(Token()), isLast(false) {}
ParseTreeNode::ParseTreeNode(const std::string& sym, const bool isLast) : symbol(sym), token(Token()), isLast(isLast) {}
void ParseTreeNode::printNode(int depth, const std::string& prefix, bool isLast) const {
    std::cout << prefix;
    if (depth > 0) {
        std::cout << (isLast ? "└── " : "├── ");
    }
    std::cout << symbol;
    if (token.get_code() != TokenCode::UNDEFINE) {
        std::cout << " (" << token.get_value() << ")";
    }
    std::cout << std::endl;

    for (size_t i = 0; i < Children.size(); ++i) {
        bool last = (i == Children.size() - 1);
        Children[i]->printNode(depth + 1, prefix + (depth > 0 ? (isLast ? "    " : "│   ") : ""), last);
    }
}

void ParseTreeNode::printNode(std::ostream& out, int depth, const std::string& prefix, bool isLast) const {
    out << prefix;
    if (depth > 0) {
        out << (isLast ? "└── " : "├── ");
    }
    out << symbol;
    if (token.get_code() != TokenCode::UNDEFINE) {
        out << " (" << token.get_value() << ")";
    }
    out << std::endl;

    for (size_t i = 0; i < Children.size(); ++i) {
        bool last = (i == Children.size() - 1);
        Children[i]->printNode(out, depth + 1, prefix + (depth > 0 ? (isLast ? "    " : "│   ") : ""), last);
    }
}