#include "../include/token.h"
#include <sstream>

std::unordered_map<std::string, TokenCode> tokenCodeMap = {
    {"int", TokenCode::KW_INT}, // 关键字 int enum = 1
    {"void", TokenCode::KW_VOID}, // 关键字 void enum = 2
    {"return", TokenCode::KW_RETURN}, // 关键字 return enum = 3
    {"const", TokenCode::KW_CONST}, // 关键字 const enum = 4
    {"main", TokenCode::KW_MAIN}, // 关键字 main enum = 5
    {"float", TokenCode::KW_FLOAT}, // 关键字 float enum = 6
    {"if", TokenCode::KW_IF}, // 关键字 if enum = 7
    {"else", TokenCode::KW_ELSE}, // 关键字 else enum = 8

    {"+", TokenCode::OP_PLUS}, // 运算符 + enum = 30
    {"-", TokenCode::OP_MINUS}, // 运算符 - enum = 31
    {"*", TokenCode::OP_MULTIPLY}, // 运算符 * enum = 32
    {"/", TokenCode::OP_DIVIDE}, // 运算符 / enum = 33
    {"%", TokenCode::OP_PERCENT}, // 运算符 % enum = 34
    {"=", TokenCode::OP_ASSIGN}, // 运算符 = enum = 35
    {">", TokenCode::OP_GT}, // 运算符 > enum = 36
    {"<", TokenCode::OP_LT}, // 运算符 < enum = 37
    {"==", TokenCode::OP_EQUAL}, // 运算符 == enum = 38
    {"<=", TokenCode::OP_LEQ}, // 运算符 <= enum = 39
    {">=", TokenCode::OP_GEQ}, // 运算符 >= enum = 40
    {"!=", TokenCode::OP_NEQ}, // 运算符 != enum = 41
    {"&&", TokenCode::OP_AND}, // 运算符 && enum = 42
    {"||", TokenCode::OP_OR}, // 运算符 || enum = 43
    {"!", TokenCode::OP_NOT}, // 运算符 ! enum = 44

    {"(", TokenCode::SE_LPARENT}, // 界符 ( enum = 20
    {")", TokenCode::SE_RPARENT}, // 界符 ) enum = 21
    {"{", TokenCode::SE_LBRACES}, // 界符 { enum = 22
    {"}", TokenCode::SE_RBARCES}, // 界符 } enum = 23
    {";", TokenCode::SE_SEMICOLON}, // 界符 ; enum = 24
    {",", TokenCode::SE_COMMA}, // 界符 , enum = 25

    {"INT", TokenCode::INT_CONST}, // 整数常量 enum = 50
    {"FLOAT", TokenCode::FLOAT_CONST}, // 浮点数常量 enum = 51

    {"IDN", TokenCode::IDNTIFIER}, // 标识符 enum = 60

    {"EOF", TokenCode::END_OF_FILE} // 文件结束符, enum = 100
};

std::unordered_map<TokenCode, std::string> tokenCodeToStringMap = {
    {TokenCode::KW_INT, "int"},
    {TokenCode::KW_VOID, "void"},
    {TokenCode::KW_RETURN, "return"},
    {TokenCode::KW_CONST, "const"},
    {TokenCode::KW_MAIN, "main"},
    {TokenCode::KW_FLOAT, "float"},
    {TokenCode::KW_IF, "if"},
    {TokenCode::KW_ELSE, "else"},
    {TokenCode::OP_PLUS, "+"},
    {TokenCode::OP_MINUS, "-"},
    {TokenCode::OP_MULTIPLY, "*"},
    {TokenCode::OP_DIVIDE, "/"},
    {TokenCode::OP_PERCENT, "%"},
    {TokenCode::OP_ASSIGN, "="},
    {TokenCode::OP_GT, ">"},
    {TokenCode::OP_LT, "<"},
    {TokenCode::OP_EQUAL, "=="},
    {TokenCode::OP_LEQ, "<="},
    {TokenCode::OP_GEQ, ">="},
    {TokenCode::OP_NEQ, "!="},
    {TokenCode::OP_AND, "&&"},
    {TokenCode::OP_OR, "||"},
    {TokenCode::OP_NOT, "!"},
    {TokenCode::SE_LPARENT, "("},
    {TokenCode::SE_RPARENT, ")"},
    {TokenCode::SE_LBRACES, "{"},
    {TokenCode::SE_RBARCES, "}"},
    {TokenCode::SE_SEMICOLON, ";"},
    {TokenCode::SE_COMMA, ","},
    {TokenCode::INT_CONST, "INT"},
    {TokenCode::FLOAT_CONST, "FLOAT"},
    {TokenCode::IDNTIFIER, "IDN"},
    {TokenCode::END_OF_FILE, "EOF"}
};

// 构造函数
Token::Token(TokenType type, const std::string& value) : type(type), value(value) {
    // 初始化code
    if((type == TokenType::IDENTIFIER)) {
        code = tokenCodeMap["IDN"];
    } else if (type == TokenType::FLOAT) {
        code = tokenCodeMap["FLOAT"];
    } else if (type == TokenType::INTEGER) {
        code = tokenCodeMap["INT"];
    } else {
        if (tokenCodeMap.find(value) == tokenCodeMap.end()) {
            code = UNDEFINE; // 如果没有找到，设置为 0
        } else {
            code = tokenCodeMap[value]; // 获取 Token 的类别
        }
    }
}

// 获取 Token 类型
TokenType Token::get_type() const {
    return type;
}

// 获取 Token 值
std::string Token::get_value() const {
    return value;
}

TokenCode Token::get_code() const {
    return code;
}

// 将 Token 转换为字符串表示
std::string Token::to_string() const {
    std::ostringstream oss;
    int classes;
    if (tokenCodeMap.find(value) == tokenCodeMap.end()) {
        classes = 0; // 如果没有找到，设置为 0
    } else{
        classes = tokenCodeMap[value]; // 获取 Token 的类别
    }
    //oss << "Token(" << token_type_to_string(type) << ", \"" << value << "\")";
    oss << value << "\t" << "<" << token_type_to_string(type) << "," 
        << ((type == TokenType::IDENTIFIER) ? value : std::to_string(classes)) << ">";
    return oss.str();
}

// 将 TokenType 转换为字符串
std::string Token::token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD: return "KW";
        case TokenType::IDENTIFIER: return "IDN";
        case TokenType::OPERATOR: return "OP";
        case TokenType::INTEGER: return "INT";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::SYMBOL_OF_END: return "SE";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
}