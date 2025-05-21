#include "../SyntacticAnalyzer/include/syntaxer.h"
#include "../LexicalAnalyzer/include/lexer.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {

    std::string filename;
    std::cout << "Please put filename (eg.: test1.sy): ";
    std::cin >> filename;
    std::string filepath = "./samples/" + filename;
    // 去掉扩展名
    std::string basename = filename.substr(0, filename.find_last_of('.'));
    // 拼接输出文件名，保存到 outputs 文件夹下
    std::string output_dir = "outputs";
    if (!fs::exists(output_dir)) {
        fs::create_directory(output_dir);
    }
    std::string token_output = output_dir + "/" + basename + "_tokens.txt";
    std::string syntax_output = output_dir + "/" + basename + "_syntax.txt";
    std::string parseTree_output = output_dir + "/" + basename + "_parseTree.txt";

    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Can not open file: " << filepath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source_code = buffer.str();

    Lexer lexer(source_code);

    //lexer.print_tokens(); // 输出 Token 序列
    lexer.save_tokens_to_file(token_output); // 保存 Token 序列到文件

    SyntaxAnalyzer syntaxAnalyzer(lexer.get_tokens());
    //syntaxAnalyzer.printSteps(); // 打印推导步骤
    DEBUG_INFO("Syntax analysis steps:");
    syntaxAnalyzer.save_steps_to_file(syntax_output); // 保存推导步骤到文件
    DEBUG_INFO("Syntax analysis tree:");
    std::ofstream parseTreeFile(parseTree_output);
    if (!parseTreeFile) {
        std::cerr << "无法打开文件: " << parseTree_output << std::endl;
        return 1;
    }
    syntaxAnalyzer.getParseTree()->printNode(parseTreeFile); // 打印语法树到文件
    parseTreeFile.close();
    //syntaxAnalyzer.getParseTree()->printNode(); // 打印语法树
    DEBUG_INFO("AST:");
    std::string ast_output = output_dir + "/" + basename + "_ast.txt";
    syntaxAnalyzer.saveASTToFile(ast_output); // 保存 AST 到文件

    // 创建 ParserTable 对象
    //ParserTable parserTable;

    //Grammar g = parserTable.getGrammar();
    //g.printGrammar();
    //g.printFirstSets();
    //g.printFollowSets();
    //g.printSelectSets();
    
    // 打印终结符和非终结符
    // std::cout << "Terminals: ";
    // for (const auto& terminal : g.getTerminals()) {
    //     std::cout << terminal << " ";
    // }
    

    // 打印预测分析表
    //parserTable.printParseTable();

    // 检查是否是 LL(1) 文法
    // if(g.isLL1Grammar()) {
    //     std::cout << "The grammar is LL(1)." << std::endl;
    // } else {
    //     std::cout << "The grammar is not LL(1)." << std::endl;
    // }

    return 0;
}