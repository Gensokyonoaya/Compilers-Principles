#include "AST.h"

void TranslationUnitDecl::addDeclStmt(std::unique_ptr<DeclStmt> declStmt) {
    declStmts.push_back(std::move(declStmt));
}
void TranslationUnitDecl::addFuncDecl(std::unique_ptr<FunctionDecl> funcDecl) {
    funcDecls.push_back(std::move(funcDecl));
}

void TranslationUnitDecl::print(std::ostream& os, const std::string& prefix, bool isLast) const {
    os << prefix << (isLast ? "└── " : "├── ") << "TranslationUnitDecl\n";

    // 合并 declStmts 和 funcDecls 的打印逻辑
    size_t totalSize = declStmts.size() + funcDecls.size();
    for (size_t i = 0; i < declStmts.size(); ++i) {
        declStmts[i]->print(os, prefix + (isLast ? "    " : "│   "), (i == totalSize - 1));
    }
    for (size_t i = 0; i < funcDecls.size(); ++i) {
        funcDecls[i]->print(os, prefix + (isLast ? "    " : "│   "), (declStmts.size() + i == totalSize - 1));
    }
}