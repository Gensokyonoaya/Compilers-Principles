#ifndef SYNTAXER_H
#define SYNTAXER_H

#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include "../../LexicalAnalyzer/include/token.h"
#include "parser_table.h"
#include "parseTree.h"
#include "AST.h"

struct AnalysisStep {
    size_t step;             // 步骤序号
    std::string stackTop;    // 栈顶符号
    std::string lookahead;   // 面临输入符号
    std::string action;      // 执行动作
};

class SyntaxAnalyzer {
public:
    SyntaxAnalyzer(const std::vector<Token>& tokens);
    void parse();
    void extractCompUnitItems(const std::shared_ptr<ParseTreeNode>& node, TranslationUnitDecl* tu);
    void extractFuncParams(const std::shared_ptr<ParseTreeNode>& node, std::vector<std::unique_ptr<ParmVarDecl>>& params);
    std::unique_ptr<ParmVarDecl> processFuncFParam(const std::shared_ptr<ParseTreeNode>& node);
    void extractBlockItems(const std::shared_ptr<ParseTreeNode>& node, CompoundStmt* block);
    std::unique_ptr<VarDecl> processVarDef(const std::shared_ptr<ParseTreeNode>& node, BuiltinType* varType);
    void processVarDefList(const std::shared_ptr<ParseTreeNode>& node, BuiltinType* varType, DeclStmt* declStmt);
    void extractFuncRParams(const std::shared_ptr<ParseTreeNode>& node, CallExpr* funcCall);
    std::unique_ptr<BinaryOperator> processlValBinaryOperator(std::unique_ptr<Expr> lhs, const std::shared_ptr<ParseTreeNode>& lValSuffixNode);


    std::unique_ptr<ASTNode> buildAST(const std::shared_ptr<ParseTreeNode>& parseTreeRoot);

    void saveASTToFile(const std::string& filename) const {
        if (!root) {
            std::cerr << "AST is empty.\n";
            return;
        }

        std::ofstream ofs(filename);
        if (!ofs) {
            std::cerr << "Failed to open file: " << filename << "\n";
            return;
        }

        root->print(ofs);
        ofs.close();
    }

    std::shared_ptr<ParseTreeNode> getParseTree() const {
        // 解析语法树遍历接口
        return parseTree;
    }

    void printSteps() const {
        for (const auto& step : steps) {
            std::cout << step.step << "\t"
                      << step.stackTop << "#"
                      << step.lookahead << "\t"
                      << step.action << "\n";
        }
    }
    void save_steps_to_file(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "无法打开文件: " << filename << std::endl;
            return;
        }
        for (const auto& step : steps) {
            file << step.step << "\t"
                 << step.stackTop << "#"
                 << step.lookahead << "\t"
                 << step.action << "\n";
        }
        file.close();
    }

    bool tryMatchFuncDef(size_t& index);

    void updateStep(AnalysisStep& step, const std::string& action) {
        step.action = action;
        steps.push_back(step);
    }

    void handleError(AnalysisStep& step) {
        updateStep(step, "error");
    }

    void handleMove(AnalysisStep& step) {
        updateStep(step, "move");
    }

    void handleAccept(AnalysisStep& step) {
        updateStep(step, "accept");
    }

    void handleReduction(AnalysisStep& step) {
        updateStep(step, "reduction");
    }

private:
    bool MAINdef; // 是否定义了main函数

    std::shared_ptr<ParseTreeNode> parseTree; // 语法分析树的根节点

    std::stack<std::shared_ptr<ParseTreeNode>> parseTreeStack; // 语法分析树栈

    std::shared_ptr<ASTNode> root; // 语法树的根节点

    std::vector<Token> tokens; // 词法分析器生成的 Token 序列

    ParserTable parserTable; // 预测分析表

    std::stack<std::string> parseStack; // 语法分析栈

    std::vector<AnalysisStep> steps;  // 存储规约序列
};

# endif