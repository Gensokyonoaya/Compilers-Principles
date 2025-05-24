#include "syntaxer.h"

SyntaxAnalyzer::SyntaxAnalyzer(const std::vector<Token>& tokens)
    : tokens(tokens) {
    // 初始化预测分析表
    parserTable = ParserTable();
    DEBUG_INFO("Parser table initialized.");
    SyntaxAnalyzer::parse();
    DEBUG_INFO("Syntax analysis completed.");
    root = std::unique_ptr<TranslationUnitDecl>(static_cast<TranslationUnitDecl*>(buildAST(parseTree).release()));
    DEBUG_INFO("AST built successfully.");
}

void SyntaxAnalyzer::parse() {
    // 语法分析的实现
    // 预测分析法
    parseStack.push("EOF"); // 添加结束符
    parseStack.push(parserTable.getGrammar().getStartSymbol()); // 添加文法的开始符号

    parseTreeStack.push(std::make_shared<ParseTreeNode>("EOF")); // 添加结束符节点
    parseTreeStack.push(std::make_shared<ParseTreeNode>(parserTable.getGrammar().getStartSymbol())); // 添加根节点
    parseTree = parseTreeStack.top(); // 语法分析树的根节点

    size_t index = 0; // 当前分析的token索引
    Token currentToken = tokens[index]; // 当前token
    MAINdef = false; // 是否定义了main函数
    size_t step = 1; // 步骤编号

    while (!parseStack.empty()) {
        DEBUG_INFO("Current token: " + currentToken.to_string());
        std::string top = parseStack.top(); // 栈顶元素
        parseStack.pop(); // 弹出栈顶元素

        std::shared_ptr<ParseTreeNode> topNode = parseTreeStack.top(); // 获取栈顶节点
        parseTreeStack.pop(); // 弹出栈顶节点

        // 创建当前步骤
        std::string currentTokenStr = currentToken.get_value();
        AnalysisStep currentStep;
        currentStep.step = step;
        currentStep.stackTop = top;
        currentStep.lookahead = currentTokenStr;

        if (tokenCodeMap.find(top) != tokenCodeMap.end()) { // 如果栈顶元素是终结符
            TokenCode topCode = tokenCodeMap[top];

            if (currentToken.get_code() == TokenCode::UNDEFINE) {
                DEBUG_ERROR("Undefined token: " + currentToken.get_value());
                handleError(currentStep);
                return;
            }

            if (topCode == TokenCode::END_OF_FILE && currentToken.get_code() == TokenCode::END_OF_FILE) {
                // 语法分析成功
                DEBUG_INFO("Syntax analysis successful.");
                handleAccept(currentStep);
                return;
            }

            if (topCode == currentToken.get_code()) { // 如果栈顶元素和当前token匹配
                // 匹配终结符，挂载token
                topNode->token = currentToken;
                index++;
                if (index < tokens.size()) {
                    currentToken = tokens[index];
                }
                handleMove(currentStep);
            } else {
                DEBUG_ERROR("Syntax error: Expected " + top + ", but found " + currentToken.to_string());
                handleError(currentStep);
                return;
            }
        } else if (top !="compUnitItem" && 
                    parserTable.getParseTable().count({top, tokenCodeToStringMap[currentToken.get_code()]})) { 
                    // 如果栈顶元素是非终结符
            Production production = parserTable.getParseTable().at({top, tokenCodeToStringMap[currentToken.get_code()]});
            handleReduction(currentStep);

            std::vector<std::shared_ptr<ParseTreeNode>> children;
            for (auto it = production.rbegin(); it != production.rend(); ++it) {
                parseStack.push(*it); // 将产生式右部逆序压入栈中
                std::shared_ptr<ParseTreeNode> childNode;
                if (it == production.rbegin()) {
                    // 如果是第一个
                    childNode = std::make_shared<ParseTreeNode>(*it, true);
                } else {
                    // 如果不是第一个
                    childNode = std::make_shared<ParseTreeNode>(*it);
                }
                children.push_back(childNode);
                parseTreeStack.push(childNode); // 将子节点逆序压入语法分析树栈中
            }
            // 按产生式顺序挂子节点
            for (auto it = children.rbegin(); it != children.rend(); ++it) {
                topNode->Children.push_back(*it);
            }
        } else if (top == "$") {
            // 空产生式
            handleReduction(currentStep);
        } else {
            // 手动处理非 LL(1) 的文法规则
            if (top == "elseStmtOpt") {
                if (currentToken.get_code() == TokenCode::KW_ELSE) {
                    parseStack.push("stmt");
                    parseStack.push("else");

                    std::shared_ptr<ParseTreeNode> stmtNode = std::make_shared<ParseTreeNode>("stmt", true);
                    std::shared_ptr<ParseTreeNode> elseNode = std::make_shared<ParseTreeNode>("else");
                    topNode->Children.push_back(elseNode);
                    topNode->Children.push_back(stmtNode);
                    parseTreeStack.push(stmtNode);
                    parseTreeStack.push(elseNode);
                } else {
                    parseStack.push("$");

                    std::shared_ptr<ParseTreeNode> emptyNode = std::make_shared<ParseTreeNode>("$", true);
                    topNode->Children.push_back(emptyNode);
                    parseTreeStack.push(emptyNode);
                }
                handleReduction(currentStep);

            } else if (top == "compUnitItem") {
                //识别main函数，这里是分析compUnitItem -> decl | funcDef时走了varDecl，需要回溯到funcDef
                if (tryMatchFuncDef(index)) {
                    parseStack.push("funcDef");
                    handleReduction(currentStep);

                    std::shared_ptr<ParseTreeNode> funcDefNode = std::make_shared<ParseTreeNode>("funcDef");
                    topNode->Children.push_back(funcDefNode);
                    parseTreeStack.push(funcDefNode);
                } else if (currentToken.get_code() == TokenCode::KW_INT ||
                           currentToken.get_code() == TokenCode::KW_CONST ||
                           currentToken.get_code() == TokenCode::KW_FLOAT) 
                {
                    parseStack.push("decl");
                    handleReduction(currentStep);

                    std::shared_ptr<ParseTreeNode> declNode = std::make_shared<ParseTreeNode>("decl");
                    topNode->Children.push_back(declNode);
                    parseTreeStack.push(declNode);
                } else {
                    DEBUG_ERROR("Syntax error: Expected 'int', 'void', 'const', or 'float', but found " + currentToken.to_string());
                    handleError(currentStep);
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
                    handleReduction(currentStep);

                    std::shared_ptr<ParseTreeNode> blockNode = std::make_shared<ParseTreeNode>("block", true);
                    std::shared_ptr<ParseTreeNode> funcFParamsOptNode = std::make_shared<ParseTreeNode>("funcFParamsOpt");
                    std::shared_ptr<ParseTreeNode> lParentNode = std::make_shared<ParseTreeNode>("(");
                    std::shared_ptr<ParseTreeNode> rParentNode = std::make_shared<ParseTreeNode>(")");
                    std::shared_ptr<ParseTreeNode> identNode = std::make_shared<ParseTreeNode>("Ident");
                    std::shared_ptr<ParseTreeNode> funcTypeNode = std::make_shared<ParseTreeNode>("funcType");
                    topNode->Children.push_back(funcTypeNode);
                    topNode->Children.push_back(identNode);
                    topNode->Children.push_back(lParentNode);
                    topNode->Children.push_back(funcFParamsOptNode);
                    topNode->Children.push_back(rParentNode);
                    topNode->Children.push_back(blockNode);
                    parseTreeStack.push(blockNode);
                    parseTreeStack.push(rParentNode);
                    parseTreeStack.push(funcFParamsOptNode);
                    parseTreeStack.push(lParentNode);
                    parseTreeStack.push(identNode);
                    parseTreeStack.push(funcTypeNode);
                } else {
                    DEBUG_ERROR("Syntax error: Expected function definition, but found " + currentToken.to_string());
                    handleError(currentStep);
                    return;
                }

            } else if (top == "funcType") {
                //funcType -> int | void
                if (currentToken.get_code() == TokenCode::KW_INT) {
                    parseStack.push("int");
                    handleReduction(currentStep);

                    std::shared_ptr<ParseTreeNode> intNode = std::make_shared<ParseTreeNode>("int", true);
                    topNode->Children.push_back(intNode);
                    parseTreeStack.push(intNode);
                } else {
                    DEBUG_ERROR("Syntax error: Expected function type, but found " + currentToken.to_string());
                    handleError(currentStep);
                    return;
                }

            } else if (top == "Ident") {
                // Ident -> IDN
                if (currentToken.get_code() == TokenCode::KW_MAIN && !MAINdef) {
                    MAINdef = true; // 只允许一个main函数
                    parseStack.push("main");
                    handleReduction(currentStep);

                    std::shared_ptr<ParseTreeNode> mainNode = std::make_shared<ParseTreeNode>("main", true);
                    topNode->Children.push_back(mainNode);
                    parseTreeStack.push(mainNode);
                } else if (currentToken.get_code() == TokenCode::KW_MAIN && MAINdef) {
                    DEBUG_ERROR("Syntax error: Multiple definitions of main function");
                    handleError(currentStep);
                    return;
                } else {
                    DEBUG_ERROR("Syntax error: Expected identifier, but found " + currentToken.to_string());
                    handleError(currentStep);
                    return;
                }

            } else {
                DEBUG_ERROR("Syntax error: Unexpected token " + currentToken.to_string());
                handleError(currentStep);
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

std::unique_ptr<ASTNode> SyntaxAnalyzer::buildAST(const std::shared_ptr<ParseTreeNode>& node) {
    // 构建抽象语法树
    if (!node) return nullptr;

    // 跳过空产生式
    if (node->symbol == "$") return nullptr;

    // 跳过括号、分号等无语义节点
    if (node->symbol == "(" || node->symbol == ")" || node->symbol == ";" || node->symbol == ",") {
        return nullptr;
    }

    // 只处理你关心的节点
    if (node->symbol == "Program") {
        // ...生成 TranslationUnitDecl AST 节点...
        DEBUG_INFO("Building TranslationUnitDecl AST node");
        auto tu = std::make_unique<TranslationUnitDecl>();
        // 递归处理 compUnit
        for (const auto& child : node->Children) {
            if (child->symbol == "compUnit") {
                extractCompUnitItems(child, tu.get());
            }
        }

        return tu;

    } else if (node->symbol == "funcDef") {
        // ...生成 FunctionDecl AST 节点...
        DEBUG_INFO("Building FunctionDecl AST node");
        std::string funcName;
        std::unique_ptr<Type> returnType;
        std::vector<std::unique_ptr<ParmVarDecl>> params;
        std::unique_ptr<CompoundStmt> body;
        bool isDefined = false;

        for (const auto& child : node->Children) {
            if (child->symbol == "funcType") {
                // 处理返回类型
                if (child->Children[0]->symbol == "int") {
                    returnType = std::make_unique<BuiltinType>(BuiltinType::Int);
                } else if (child->Children[0]->symbol == "void") {
                    returnType = std::make_unique<BuiltinType>(BuiltinType::Void);
                }
            } else if (child->symbol == "Ident") {
                // 处理函数名
                funcName = child->Children[0]->token.get_value();
            } else if (child->symbol == "funcFParamsOpt" && !child->Children.empty()) {
                // 处理函数参数
                extractFuncParams(child->Children[0], params);
            } else if (child->symbol == "block") {
                // 处理函数体
                body = std::unique_ptr<CompoundStmt>(static_cast<CompoundStmt*>(buildAST(child).release()));
                isDefined = true; // 标记函数已定义
            }
        }

        auto funcDecl = std::make_unique<FunctionDecl>(funcName, std::move(returnType));
        for (auto& param : params) {
            funcDecl->addParam(std::move(param));
        }
        if (body) funcDecl->body = std::move(body);
        funcDecl->isDefined = isDefined; // 设置函数是否已定义
        return funcDecl;

    } else if (node->symbol == "block") {
        // ...生成 CompoundStmt AST 节点...
        DEBUG_INFO("Building CompoundStmt AST node");
        auto block = std::make_unique<CompoundStmt>();
        // 递归展开 blockItemList
        for (const auto& child : node->Children) {
            if (child->symbol == "blockItemList") {
                extractBlockItems(child, block.get());
            } else if (child->symbol == "blockItem") {
                auto stmt = buildAST(child);
                if (stmt) block->addStatement(std::unique_ptr<Stmt>(static_cast<Stmt*>(stmt.release())));
            }
        }

        return block;

    } else if (node->symbol == "blockItem") {
        // ...生成对应的 Stmt AST 节点...
        DEBUG_INFO("Building BlockItem AST node");
        if (node->Children[0]->symbol == "decl") {
            // 处理声明
            return buildAST(node->Children[0]);
        } else if (node->Children[0]->symbol == "stmt") {
            // 处理语句
            return buildAST(node->Children[0]);
        }
        
    } else if (node->symbol == "varDecl") {
        // ...生成 DeclStmt 节点...
        DEBUG_INFO("Building DeclStmt AST node");
        auto declStmt = std::make_unique<DeclStmt>();
        // 提取变量类型
        std::unique_ptr<BuiltinType> varType;
        for (const auto& child : node->Children) {
            if (child->symbol == "bType") {
                if (child->Children[0]->symbol == "int") {
                    varType = std::make_unique<BuiltinType>(BuiltinType::Int);
                } else if (child->Children[0]->symbol == "float") {
                    varType = std::make_unique<BuiltinType>(BuiltinType::Float);
                }
            } else if (child->symbol == "varDef") {
                // 处理第一个变量定义
                auto varDecl = processVarDef(child, varType.get());
                if (varDecl) {
                    declStmt->addDecl(std::move(varDecl));
                }
            } else if (child->symbol == "varDefList") {
                // 递归处理 varDefList
                processVarDefList(child, varType.get(), declStmt.get());
            }
        }

        return declStmt;

    } else if (node->symbol == "constDecl") {
        // ...生成 DeclStmt 节点...
        DEBUG_INFO("Building DeclStmt AST node (ConstDecl)");
        auto constDecl = std::make_unique<DeclStmt>();
        // 提取常量类型
        std::unique_ptr<BuiltinType> constType;
        for (const auto& child : node->Children) {
            if (child->symbol == "bType") {
                if (child->Children[0]->symbol == "int") {
                    constType = std::make_unique<BuiltinType>(BuiltinType::const_Int);
                } else if (child->Children[0]->symbol == "float") {
                    constType = std::make_unique<BuiltinType>(BuiltinType::const_Float);
                }
            } else if (child->symbol == "constDef") {
                // 处理第一个常量定义
                auto constDeclNode = processVarDef(child, constType.get());
                if (constDeclNode) {
                    constDecl->addDecl(std::move(constDeclNode));
                }
            } else if (child->symbol == "constDefList") {
                // 递归处理 constDefList
                processVarDefList(child, constType.get(), constDecl.get());
            }
        }

        return constDecl;

    } else if(node->symbol == "stmt") {
        DEBUG_INFO("Building Stmt AST node");
        if (node->Children[0]->symbol == "expStmt") {
            return buildAST(node->Children[0]);
        } else if (node->Children[0]->symbol == "block") {
            return buildAST(node->Children[0]);
        } else if (node->Children[0]->symbol == "ifStmt") {
            return buildAST(node->Children[0]);
        } else if (node->Children[0]->symbol == "return") {
            // 处理 return 语句
            auto returnStmt = std::make_unique<ReturnStmt>();
            if (node->Children[1]->symbol == "returnExpOpt" && node->Children[1]->Children[0]->symbol != "$") {
                // 处理 return 后的表达式
                auto expr = buildAST(node->Children[1]->Children[0]); // exp
                if (expr) {
                    returnStmt->setExpr(std::unique_ptr<Expr>(static_cast<Expr*>(expr.release())));
                }
            }
            return returnStmt;
        }

    } else if (node->symbol == "expStmt") {
        DEBUG_INFO("Building ExpStmt AST node");
        if (node->Children[0]->symbol == "exp") {
            // 表达式语句
            return buildAST(node->Children[0]);
        } else if (node->Children[0]->symbol == ";") {
            // 空语句，跳过
            return nullptr;
        }

    } else if (node->symbol == "ifStmt") {
        DEBUG_INFO("Building IfStmt AST node");
        auto condition = buildAST(node->Children[2]); // exp
        auto thenBranch = buildAST(node->Children[4]); // stmt
        std::unique_ptr<Stmt> elseBranch = nullptr;

        if (node->Children.size() > 5 && node->Children[5]->symbol == "elseStmtOpt" && node->Children[5]->Children[0]->symbol != "$") {
            // 处理 else 语句
            elseBranch = std::unique_ptr<Stmt>(static_cast<Stmt*>(buildAST(node->Children[5]->Children[1]).release())); // else stmt
        }

        return std::make_unique<IfStmt>(
            std::unique_ptr<Expr>(static_cast<Expr*>(condition.release())),
            std::unique_ptr<Stmt>(static_cast<Stmt*>(thenBranch.release())),
            std::move(elseBranch)
        );

    } else if (node->symbol == "assignExp") {
        DEBUG_INFO("Building AssignExp AST node");
        
        // 检查是否是 lVal lValSuffix
        if (node->Children[0]->symbol == "lVal" && node->Children[1]->symbol == "lValSuffix") {
            auto lValNode = node->Children[0];
            auto IdentNode = lValNode->Children[0];
            auto lValSuffixNode = node->Children[1];

            // 处理赋值表达式
            if (lValSuffixNode->Children[0]->symbol == "=") {
                auto lhs = buildAST(lValNode);
                auto rhs = buildAST(lValSuffixNode->Children[1]); // 右侧表达式
                return std::make_unique<BinaryOperator>(
                    "=",
                    std::unique_ptr<Expr>(static_cast<Expr*>(lhs.release())),
                    std::unique_ptr<Expr>(static_cast<Expr*>(rhs.release()))
                );
            }

            // 检查是否是函数调用
            auto postfixList = lValSuffixNode->Children[0];
            if (postfixList->Children[0]->symbol != "$") {
                // 处理函数调用
                auto funcCall = std::make_unique<CallExpr>(IdentNode->Children[0]->token.get_value());
                // 提取参数列表
                auto funcRParamsOptNode = postfixList->Children[1]; // funcRParamsOpt
                if (funcRParamsOptNode->Children[0]->symbol == "funcRParams") {
                    extractFuncRParams(funcRParamsOptNode->Children[0], funcCall.get());
                }

                return funcCall;
            }

            // 处理其他二元运算符
            auto lhs = buildAST(lValNode);
            auto Binary = processlValBinaryOperator(std::unique_ptr<Expr>(static_cast<Expr*>(lhs.release())), lValSuffixNode);
            if (Binary) {
                DEBUG_INFO("is Binary");
                return Binary;
            } else {
                // 处理变量引用
                DEBUG_INFO("is DeclRefExpr");
                return std::make_unique<DeclRefExpr>(IdentNode->Children[0]->token.get_value(), nullptr);
            }
        } else {
            // 递归处理 condExp
            return buildAST(node->Children[0]);
        }

    } else if (node->symbol == "primaryExp") {
        DEBUG_INFO("Building PrimaryExp AST node");
        if (node->Children[0]->symbol == "number") {
            // 处理数字常量
            return buildAST(node->Children[0]);
        } else if (node->Children[0]->symbol == "(") {
            // 处理括号表达式
            return buildAST(node->Children[1]); // 跳过括号
        }

    } else if (node->symbol == "number") {
        DEBUG_INFO("Building Number AST node");
        // 处理数字常量
        if (node->Children[0]->symbol == "IntConst") {
            auto IntConstNode = node->Children[0];
            int value = std::stoi(IntConstNode->Children[0]->token.get_value());
            return std::make_unique<IntegerLiteral>(value);
        } else if (node->Children[0]->symbol == "floatConst") {
            auto floatConstConstNode = node->Children[0];
            float value = std::stof(floatConstConstNode->Children[0]->token.get_value());
            return std::make_unique<FloatingLiteral>(value);
        }

    } else if (node->symbol == "lOrExpExp" || node -> symbol == "lAndExpExp"|| 
               node -> symbol == "lAndExpExp" || node -> symbol == "eqExpExp" ||
               node -> symbol == "relExpExp" || node -> symbol == "addExpExp" ||
               node -> symbol == "mulExpExp" || node -> symbol == "unaryExpExp") {
        DEBUG_INFO("Building ExpExp AST node");
        // 处理变量引用和函数调用以及二元运算符
        if (node->Children[0]->symbol == "lVal") {
            auto lValNode = node->Children[0];
            auto IdentNode = lValNode->Children[0];
            auto postfixExpListNode = node->Children[1];
            if (postfixExpListNode->Children[0]->symbol == "(") {
                // 处理函数调用
                auto funcCall = std::make_unique<CallExpr>(IdentNode->Children[0]->token.get_value());
                // 提取参数列表
                auto funcRParamsOptNode = postfixExpListNode->Children[1]; // funcRParamsOpt
                if (funcRParamsOptNode->Children[0]->symbol == "funcRParams") {
                    extractFuncRParams(funcRParamsOptNode->Children[0], funcCall.get());
                }
                return funcCall;
            }

            bool islVal = true;
            for (const auto& child : node->Children) {
                if (child->symbol != "lVal" && child->Children[0]->symbol != "$") {
                    islVal = false;
                    break;
                }
            }
            if(islVal) {
                // 处理变量引用
                return std::make_unique<DeclRefExpr>(IdentNode->Children[0]->token.get_value(), nullptr);
            }

            // 处理二元运算符
            auto lhs = buildAST(lValNode);
            return processlValBinaryOperator(std::unique_ptr<Expr>(static_cast<Expr*>(lhs.release())), node);

        }
        return buildAST(node->Children[0]); // 递归处理子节点

    } else if (node->symbol == "unaryExp") {
        DEBUG_INFO("Building UnaryExp AST node");
        // 处理一元运算符
        if (node->Children.size() == 2) { // 运算符 操作数
            auto opNode = node->Children[0];
            std::string op = opNode->Children[0]->token.get_value();
            auto subExpr = buildAST(node->Children[1]);
            return std::make_unique<UnaryOperator>(
                op,
                std::unique_ptr<Expr>(static_cast<Expr*>(subExpr.release()))
            );
        } else {
            // 递归处理子节点
            return buildAST(node->Children[0]);
        }

    } else if (node->symbol == "lOrExp" || node->symbol == "lAndExp" ||
               node->symbol == "eqExp" || node->symbol == "relExp" ||
               node->symbol == "addExp" || node->symbol == "mulExp") {
        DEBUG_INFO("Building Exp AST node");
        // 处理二元运算符
        auto List = node->Children[1];
        if (node->Children.size() == 2 && List->Children[0]->symbol != "$") { // 左操作数 运算符 右操作数
            auto opNode = List->Children[0];
            std::string op = opNode->Children[0]->token.get_value();
            auto lhs = buildAST(node->Children[0]);
            auto rhs = buildAST(List->Children[1]);
            auto llhs = std::make_unique<BinaryOperator>(
                op,
                std::unique_ptr<Expr>(static_cast<Expr*>(lhs.release())),
                std::unique_ptr<Expr>(static_cast<Expr*>(rhs.release()))
            );
            if (List->Children[2]->Children[0]->symbol != "$") {
                auto rrhs = List->Children[2];
                return processlValBinaryOperator(std::move(llhs), rrhs);
            }
            return llhs;
        }
        return buildAST(node->Children[0]); // 递归处理子节点

    } else if (node->symbol == "Ident") {
        DEBUG_INFO("Building Ident AST node");
        //处理变量名
        return std::make_unique<DeclRefExpr>(node->Children[0]->token.get_value(), nullptr);

    } else {
        // 其它节点，递归处理所有子节点，返回第一个非空AST
        DEBUG_INFO("Building other AST node: " + node->symbol);
        for (const auto& child : node->Children) {
            auto ast = buildAST(child);
            if (ast) return ast;
        }
        return nullptr;
    }
    DEBUG_INFO("No matching AST node found for: " + node->symbol);
    return nullptr; // 如果没有匹配的节点，返回空指针
}

void SyntaxAnalyzer::extractCompUnitItems(const std::shared_ptr<ParseTreeNode>& node, TranslationUnitDecl* tu) {
    if (!node) return;

    for (const auto& child : node->Children) {
        if (child->symbol == "compUnitItem") {
            // 处理 compUnitItem
            auto item = buildAST(child->Children[0]); // DeclStmt 或 funcDef
            if (item) {
                if (item->nodeType() == "DeclStmt") {
                    // 如果是 DeclStmt，添加到 TranslationUnitDecl
                    tu->addDeclStmt(std::unique_ptr<DeclStmt>(static_cast<DeclStmt*>(item.release())));
                } else if (item->nodeType() == "FunctionDecl") {
                    // 如果是 FunctionDecl，添加到 TranslationUnitDecl
                    tu->addFuncDecl(std::unique_ptr<FunctionDecl>(static_cast<FunctionDecl*>(item.release())));
                }
            }
        } else if (child->symbol == "compUnit") {
            // 递归处理嵌套的 compUnit
            extractCompUnitItems(child, tu);
        }
    }
}

void SyntaxAnalyzer::extractFuncParams(const std::shared_ptr<ParseTreeNode>& node, std::vector<std::unique_ptr<ParmVarDecl>>& params) {
    if (!node) return;

    for (const auto& child : node->Children) {
        if (child->symbol == "funcFParam") {
            // 处理单个参数
            auto param = processFuncFParam(child);
            if (param) {
                params.push_back(std::move(param));
            }
        } else if (child->symbol == "funcFParamsList") {
            // 递归处理参数列表
            extractFuncParams(child, params);
        }
    }
}

std::unique_ptr<ParmVarDecl> SyntaxAnalyzer::processFuncFParam(const std::shared_ptr<ParseTreeNode>& node) {
    if (!node) return nullptr;

    std::string paramName;
    std::unique_ptr<Type> paramType;

    for (const auto& child : node->Children) {
        if (child->symbol == "bType") {
            // 提取参数类型
            if (child->Children[0]->symbol == "int") {
                paramType = std::make_unique<BuiltinType>(BuiltinType::Int);
            } else if (child->Children[0]->symbol == "float") {
                paramType = std::make_unique<BuiltinType>(BuiltinType::Float);
            }
        } else if (child->symbol == "Ident") {
            // 提取参数名称
            paramName = child->Children[0]->token.get_value();
        }
    }

    // 创建 ParmVarDecl 节点
    return std::make_unique<ParmVarDecl>(paramName, std::move(paramType));
}

// 辅助函数：递归展开 blockItemList
void SyntaxAnalyzer::extractBlockItems(const std::shared_ptr<ParseTreeNode>& node, CompoundStmt* block) {
    if (!node) return;

    for (const auto& child : node->Children) {
        if (child->symbol == "blockItem") {
            // 处理 blockItem
            auto stmt = buildAST(child);
            if (stmt) block->addStatement(std::unique_ptr<Stmt>(static_cast<Stmt*>(stmt.release())));
        } else if (child->symbol == "blockItemList") {
            // 递归展开嵌套的 blockItemList
            extractBlockItems(child, block);
        }
    }
}

std::unique_ptr<VarDecl> SyntaxAnalyzer::processVarDef(const std::shared_ptr<ParseTreeNode>& node, BuiltinType* varType) {
    if (!node) return nullptr;

    std::string varName;
    std::unique_ptr<Expr> Value;

    for (const auto& child : node->Children) {
        if (child->symbol == "Ident") {
            // 提取变量名
            varName = child->Children[0]->token.get_value();
        } else if (child->symbol == "verDefSuffix") {
            // 提取初始值
            if (child->Children[0]->symbol != "$" && child->Children[0]->symbol == "=") {
                Value = std::unique_ptr<Expr>(static_cast<Expr*>(buildAST(child->Children[1]).release()));
            }
        } else if (child->symbol == "constInitVal") {
            // 提取常量表达式
            Value = std::unique_ptr<Expr>(static_cast<Expr*>(buildAST(child).release()));
        }
    }

    // 创建 VarDecl 节点
    auto varDecl = std::make_unique<VarDecl>(varName, std::make_unique<BuiltinType>(*varType));
    if (Value) {
        varDecl->initValue = std::move(Value);
    }

    return varDecl;
}

void SyntaxAnalyzer::processVarDefList(const std::shared_ptr<ParseTreeNode>& node, BuiltinType* varType, DeclStmt* declStmt) {
    if (!node) return;

    for (const auto& child : node->Children) {
        if (child->symbol == "varDef" || child->symbol == "constDef") {
            // 处理变量定义
            auto varDecl = processVarDef(child, varType);
            if (varDecl) {
                declStmt->addDecl(std::move(varDecl));
            }
        } else if (child->symbol == "varDefList" || child->symbol == "constDefList") {
            // 递归处理嵌套的 varDefList
            processVarDefList(child, varType, declStmt);
        }
    }
}



void SyntaxAnalyzer::extractFuncRParams(const std::shared_ptr<ParseTreeNode>& node, CallExpr* funcCall) {
    if (!node) return;

    for (const auto& child : node->Children) {
        if (child->symbol == "funcRParam") {
            // 提取单个参数
            auto arg = buildAST(child->Children[0]); // exp
            if (arg) {
                funcCall->addArgument(std::unique_ptr<Expr>(static_cast<Expr*>(arg.release())));
            }
        } else if (child->symbol == "funcRParamsList") {
            // 递归处理参数列表
            extractFuncRParams(child, funcCall);
        }
    }
}

std::unique_ptr<BinaryOperator> SyntaxAnalyzer::processlValBinaryOperator(std::unique_ptr<Expr> lhs, const std::shared_ptr<ParseTreeNode>& lValSuffixNode) {
    // 遍历 lValSuffix 的子节点，寻找二元运算符
    for (const auto& child : lValSuffixNode->Children) {
        if ((child->symbol == "addExpList" || child->symbol == "mulExpList" || child->symbol == "relExpList" ||
            child->symbol == "eqExpList" || child->symbol == "lAndExpList" || child->symbol == "lOrExpList") 
            && child->Children.size() > 1) {
            // 处理二元运算符列表
            //DEBUG_INFO("Processing binary operator list, found: " + child->symbol);
            auto opNode = child->Children[0];
            std::string op;
            op = opNode->Children[0]->token.get_value(); // 提取运算符
            auto llhs = std::make_unique<BinaryOperator>(op, std::move(lhs), nullptr);
            auto rhs = buildAST(child->Children[1]); // 提取右操作数
            llhs->set_rhs(std::unique_ptr<Expr>(static_cast<Expr*>(rhs.release())));
            auto List = child->Children[2];
            
            if (List->Children.size() > 1) {
                auto res = processlValBinaryOperator(std::move(llhs), List);
                if (res) {
                    return res;
                }
            }
            return llhs;
        }
    }
    // 如果没有二元运算符，返回左操作数
    //DEBUG_INFO("no BinaryOperator found, returning null");
    return nullptr;
}
