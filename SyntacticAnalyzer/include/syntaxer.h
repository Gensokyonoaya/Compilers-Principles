#ifndef SYNTAXER_H
#define SYNTAXER_H

#include <vector>
#include <stack>
#include "../../LexicalAnalyzer/include/token.h"
#include "parser_table.h"
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
    void printSteps() const {
        for (const auto& step : steps) {
            std::cout << step.step << "\t"
                      << step.stackTop << "#"
                      << step.lookahead << "\t"
                      << step.action << "\n";
        }
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

    std::shared_ptr<ASTNode> root; // 语法树的根节点

    std::vector<Token> tokens; // 词法分析器生成的 Token 序列

    ParserTable parserTable; // 预测分析表

    std::stack<std::string> parseStack; // 语法分析栈

    std::vector<AnalysisStep> steps;  // 存储规约序列
};

#endif