#include "syntaxer.h"

SyntaxAnalyzer::SyntaxAnalyzer(const std::vector<Token>& tokens)
    : tokens(tokens) {
    // 初始化预测分析表
    parserTable = ParserTable();
    SyntaxAnalyzer::parse();
}

void SyntaxAnalyzer::parse() {
    // 语法分析的实现
    // 预测分析法
    parseStack.push("EOF"); // 添加结束符
    parseStack.push(parserTable.getGrammar().getStartSymbol()); // 添加文法的开始符号
    size_t index = 0; // 当前分析的token索引
    Token currentToken = tokens[index]; // 当前token

    bool MAINdef = false; // 是否定义了main函数
    size_t step = 1; // 步骤编号

    while (!parseStack.empty()) {
        std::string top = parseStack.top(); // 栈顶元素
        parseStack.pop(); // 弹出栈顶元素

        std::string currentTokenStr = currentToken.get_value();

        // 创建当前步骤
        AnalysisStep currentStep;
        currentStep.step = step;
        currentStep.stackTop = top;
        currentStep.lookahead = currentTokenStr;

        if (tokenCodeMap.find(top) != tokenCodeMap.end()) { // 如果栈顶元素是终结符
            TokenCode topCode = tokenCodeMap[top];

            if (currentToken.get_code() == TokenCode::UNDEFINE) {
                DEBUG_ERROR("Undefined token: " + currentToken.get_value());
                currentStep.action = "error";
                steps.push_back(currentStep);
                return;
            }

            if (topCode == TokenCode::END_OF_FILE && currentToken.get_code() == TokenCode::END_OF_FILE) {
                // 语法分析成功
                DEBUG_INFO("Syntax analysis successful.");
                currentStep.action = "accept";
                steps.push_back(currentStep);
                return;
            }

            if (topCode == currentToken.get_code()) { // 如果栈顶元素和当前token匹配
                DEBUG_INFO("Matched token: " + currentToken.to_string());
                index++;
                if (index < tokens.size()) {
                    currentToken = tokens[index];
                }
                currentStep.action = "move";
                steps.push_back(currentStep);
            } else {
                DEBUG_ERROR("Syntax error: Expected " + top + ", but found " + currentToken.to_string());
                currentStep.action = "error";
                steps.push_back(currentStep);
                return;
            }
        } else if (top !="compUnitItem" && 
                    parserTable.getParseTable().count({top, tokenCodeToStringMap[currentToken.get_code()]})) { 
                    // 如果栈顶元素是非终结符
            Production production = parserTable.getParseTable().at({top, tokenCodeToStringMap[currentToken.get_code()]});
            
            currentStep.action = "reduction";
            steps.push_back(currentStep);
            
            for (auto it = production.rbegin(); it != production.rend(); ++it) {
                parseStack.push(*it); // 将产生式右部逆序压入栈中
            }
        } else if (top == "$") {
            // 空产生式
            currentStep.action = "reduction";
            steps.push_back(currentStep);
        } else {
            // 手动处理非 LL(1) 的文法规则
            if (top == "elseStmtOpt") {
                if (currentToken.get_code() == TokenCode::KW_ELSE) {
                    parseStack.push("stmt");
                    parseStack.push("else");
                    currentStep.action = "reduction";
                    steps.push_back(currentStep);
                } else {
                    parseStack.push("$");
                    currentStep.action = "reduction";
                    steps.push_back(currentStep);
                }

            } else if (top == "compUnitItem") {
                //识别main函数，这里是分析compUnitItem -> decl | funcDef时走了varDecl，需要回溯到funcDef
                if (tryMatchFuncDef(index)) {
                    parseStack.push("funcDef");
                    currentStep.action = "reduction";
                    steps.push_back(currentStep);
                } else if (currentToken.get_code() == TokenCode::KW_INT ||
                           currentToken.get_code() == TokenCode::KW_CONST ||
                           currentToken.get_code() == TokenCode::KW_FLOAT) 
                {
                    parseStack.push("decl");
                    currentStep.action = "reduction";
                    steps.push_back(currentStep);
                } else {
                    DEBUG_ERROR("Syntax error: Expected 'int', 'void', 'const', or 'float', but found " + currentToken.to_string());
                    currentStep.action = "error";
                    steps.push_back(currentStep);
                    return;
                }

            } else if (top == "funcDef") {
                //funcDef -> funcType Ident '(' funcFParamsOpt ')' block;
                if (currentToken.get_code() == TokenCode::KW_INT) {
                    parseStack.push("block");
                    parseStack.push(")");
                    parseStack.push("funcFParamsOpt");
                    parseStack.push("(");
                    parseStack.push("Ident");
                    parseStack.push("funcType");
                    currentStep.action = "reduction";
                    steps.push_back(currentStep);
                } else {
                    DEBUG_ERROR("Syntax error: Expected function definition, but found " + currentToken.to_string());
                    currentStep.action = "error";
                    steps.push_back(currentStep);
                    return;
                }

            } else if (top == "funcType") {
                //funcType -> int | void
                if (currentToken.get_code() == TokenCode::KW_INT) {
                    parseStack.push("int");
                    currentStep.action = "reduction";
                    steps.push_back(currentStep);
                } 
                // else if (currentToken.get_code() == TokenCode::KW_VOID) {
                //     parseStack.push("void");
                //     currentStep.action = "reduction";
                //     steps.push_back(currentStep);}
                else {
                    DEBUG_ERROR("Syntax error: Expected function type, but found " + currentToken.to_string());
                    currentStep.action = "error";
                    steps.push_back(currentStep);
                    return;
                }

            } else if (top == "Ident") {
                // Ident -> IDN
                if (currentToken.get_code() == TokenCode::KW_MAIN && !MAINdef) {
                    MAINdef = true; // 只允许一个main函数
                    parseStack.push("main");
                    currentStep.action = "reduction";
                    steps.push_back(currentStep);
                } else if (currentToken.get_code() == TokenCode::KW_MAIN && MAINdef) {
                    DEBUG_ERROR("Syntax error: Multiple definitions of main function");
                    currentStep.action = "error";
                    steps.push_back(currentStep);
                    return;
                } else {
                    DEBUG_ERROR("Syntax error: Expected identifier, but found " + currentToken.to_string());
                    currentStep.action = "error";
                    steps.push_back(currentStep);
                    return;
                }

            } else {
                DEBUG_ERROR("Syntax error: Unexpected token " + currentToken.to_string());
                currentStep.action = "error";
                steps.push_back(currentStep);
                return;
            }
        }
        // 增加步骤编号并继续分析
        step++;
    }
}

bool SyntaxAnalyzer::tryMatchFuncDef(size_t& index) {
    size_t originalIndex = index; // 保存当前索引以便回溯

    // 检查 (int | void) Ident '(' 的模式
    if (index + 2 < tokens.size() &&
        (tokens[index].get_code() == TokenCode::KW_INT || tokens[index].get_code() == TokenCode::KW_VOID) &&
        (tokens[index + 1].get_code() == TokenCode::IDNTIFIER || tokens[index + 1].get_code() == TokenCode::KW_MAIN) &&
        tokens[index + 2].get_code() == TokenCode::SE_LPARENT) {
        return true; // 符合 funcDef 的模式
    }

    index = originalIndex; // 回溯
    return false;
}
