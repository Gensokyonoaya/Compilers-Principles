#include "lexer.h"
#include <iostream>

Lexer::Lexer(const std::string& source) : source_code(source), current_position(0) {
    current_char = source_code[current_position];
    initialize_automaton(); // 初始化有限自动机
    tokens = tokenize(); // 进行词法分析
}

void Lexer::initialize_automaton() {
    // 添加状态
    fa.add_state(0); // 初始状态
    fa.add_state(1, true, "IDENTIFIER"); // 标识符终止状态（包括关键字）
    fa.add_state(2, true, "INTEGER");    // 整数终止状态
    fa.add_state(3, true, "FLOAT");      // 浮点数终止状态
    fa.add_state(4, true, "SYMBOL_OF_END"); // 界符终止状态

    fa.add_state(6, true, "OPERATOR"); // 单字符运算符+-*/%终止状态
    fa.add_state(7, true, "OPERATOR"); // 双字符运算符终止状态

    fa.add_state(8, true, "OPERATOR"); // 运算符=状态
    fa.add_state(9, true, "OPERATOR"); // 运算符<状态
    fa.add_state(10, true, "OPERATOR"); // 运算符>状态
    fa.add_state(11, true, "OPERATOR"); // 运算符!状态
    fa.add_state(12, false, "OPERATOR_&"); // 运算符&状态
    fa.add_state(13, false, "OPERATOR_|"); // 运算符|状态

    // 添加转换规则
    for (char c = 'a'; c <= 'z'; ++c) {
        fa.add_transition(0, c, 1); // 标识符以字母开头
        fa.add_transition(1, c, 1); // 标识符后续字符
    }
    for (char c = 'A'; c <= 'Z'; ++c) {
        fa.add_transition(0, c, 1);
        fa.add_transition(1, c, 1);
    }
    fa.add_transition(1, '_', 1); // 标识符可以包含下划线

    for (char c = '0'; c <= '9'; ++c) {
        fa.add_transition(0, c, 2); // 数字以数字开头
        fa.add_transition(2, c, 2); // 整数后续字符
        fa.add_transition(3, c, 3); // 浮点数后续字符
        fa.add_transition(1, c, 1); // 标识符可以包含数字
    }
    fa.add_transition(2, '.', 3); // 整数后可以接小数点，进入浮点数状态

    fa.add_transition(0, '+', 6); // 运算符
    fa.add_transition(0, '-', 6); // 运算符
    fa.add_transition(0, '*', 6); // 运算符
    fa.add_transition(0, '/', 6); // 运算符
    fa.add_transition(0, '%', 6); // 运算符
    fa.add_transition(0, '=', 8); // 运算符=状态
    fa.add_transition(0, '<', 9); // 运算符<状态
    fa.add_transition(0, '>', 10); // 运算符>状态
    fa.add_transition(0, '!', 11); // 运算符!状态
    fa.add_transition(0, '&', 12); // 运算符&状态
    fa.add_transition(0, '|', 13); // 运算符|状态

    fa.add_transition(8, '=', 7); // 双字符运算符
    fa.add_transition(9, '=', 7); // 双字符运算符
    fa.add_transition(10, '=', 7); // 双字符运算符
    fa.add_transition(11, '=', 7); // 双字符运算符
    fa.add_transition(12, '&', 7); // 双字符运算符
    fa.add_transition(13, '|', 7); // 双字符运算符

    // 设置初始状态
    fa.set_start_state(0);

    // 确定化和最小化（可选）
    fa = fa.determinize();
    // 设置初始状态
    fa.set_start_state(0);
    fa = fa.minimize();
    // 设置初始状态
    //fa.print(); // 打印自动机状态和转换规则
    //std::cout << ">>>>>DFA has been initialized.<<<<<" << std::endl;
}

void Lexer::advance() {
    current_position++;
    if (current_position < source_code.size()) {
        current_char = source_code[current_position];
    } else {
        current_char = '\0'; // End of input
    }
}

void Lexer::skip_whitespace() {
    while (current_char == ' ' || current_char == '\t' || current_char == '\n') {
        advance();
    }
}

Token Lexer::get_next_token() {
    skip_whitespace();

    // 检查是否到达文件末尾
    if (current_char == '\0') {
        return Token(TokenType::END_OF_FILE, "EOF");
    }

    if (current_char == ',' || current_char == ';' || current_char == '(' ||
            current_char == ')' || current_char == '{' || current_char == '}' ) {
        std::string symbol(1, current_char); // 创建一个包含当前字符的字符串
        advance();
        return Token(TokenType::SYMBOL_OF_END, symbol); // 返回界符 Token
    }

    int current_state = fa.get_next_state(0, current_char); // 从初始状态开始
    int next_state = current_state;
    std::string lexeme;

    while (next_state != -1) {
        current_state = next_state; // 更新当前状态
        lexeme += current_char;
        advance();
        next_state = fa.get_next_state(current_state, current_char); // 获取下一个状态
    }

    // 检查 current_state 是否是终止状态
    if (fa.get_states().find(current_state) == fa.get_states().end() || 
        !fa.get_states().at(current_state).is_final) {
        DEBUG_ERROR("Invalid state for token: " + lexeme);
        // 如果没有匹配的状态，返回错误 Token
        return Token(TokenType::UNDEFINED, lexeme); // 返回错误 Token
    }

    if (fa.get_states().at(current_state).is_final) {
        std::string token_type = fa.get_states().at(current_state).token_type;
        if (token_type == "IDENTIFIER") {
            // 检查是否是关键字
            if (lexeme == "int" || lexeme == "void" || lexeme == "return" ||
                lexeme == "const" || lexeme == "main" || lexeme == "float" ||
                lexeme == "if" || lexeme == "else") {
                return Token(TokenType::KEYWORD, lexeme);
            }
            return Token(TokenType::IDENTIFIER, lexeme);
        } else if (token_type == "INTEGER") {
            return Token(TokenType::INTEGER, lexeme);
        } else if (token_type == "FLOAT") {
            return Token(TokenType::FLOAT, lexeme);
        } else if (token_type == "SYMBOL_OF_END") {
            return Token(TokenType::SYMBOL_OF_END, lexeme);
        } else if (token_type == "OPERATOR") {
            return Token(TokenType::OPERATOR, lexeme);
        }
    }

    // 如果没有匹配的状态，返回错误 Token
    DEBUG_ERROR("Unrecognized token: " + lexeme);
    return Token(TokenType::UNDEFINED, lexeme); // 返回错误 Token
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (current_char != '\0') {
        Token token = get_next_token(); // 获取下一个 Token
        if (token.get_type() == TokenType::END_OF_FILE) {
            tokens.push_back(token); // 添加文件结束符 Token
            break; // 到达输入末尾，停止分析
        }
        tokens.push_back(token);
    }
    if (tokens.back().get_type() != TokenType::END_OF_FILE) {
        tokens.push_back(Token(TokenType::END_OF_FILE, "EOF")); // 添加文件结束符 Token
    }
    return tokens;
}

void Lexer::print_tokens() {
    for (const Token& token : tokens) {
        std::cout << token.to_string() << std::endl; // 输出每个 Token 的字符串表示
    }
}

void Lexer::save_tokens_to_file(const std::string& filename) const {
    std::ofstream ofs(filename);
    if (!ofs) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }
    for (const Token& token : tokens) {
        ofs << token.to_string() << std::endl;
    }
    ofs.close();
}