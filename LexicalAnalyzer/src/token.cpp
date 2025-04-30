#include "token.h"
#include <sstream>

std::map<std::string, int> tokenCodeMap = {
    {"int", TokenCode::KW_INT},
    {"void", TokenCode::KW_VOID},
    {"return", TokenCode::KW_RETURN},
    {"const", TokenCode::KW_CONST},
    {"main", TokenCode::KW_MAIN},
    {"float", TokenCode::KW_FLOAT},
    {"if", TokenCode::KW_IF},
    {"else", TokenCode::KW_ELSE},

    {"+", TokenCode::OP_PLUS},
    {"-", TokenCode::OP_MINUS},
    {"*", TokenCode::OP_MULTIPLY},
    {"/", TokenCode::OP_DIVIDE},
    {"%", TokenCode::OP_PERCENT},
    {"=", TokenCode::OP_ASSIGN},
    {">", TokenCode::OP_GT},
    {"<", TokenCode::OP_LT},
    {"==", TokenCode::OP_EQUAL},
    {"<=", TokenCode::OP_LEQ},
    {">=", TokenCode::OP_GEQ},
    {"!=", TokenCode::OP_NEQ},
    {"&&", TokenCode::OP_AND},
    {"||", TokenCode::OP_OR},

    {"(", TokenCode::SE_LPARENT},
    {")", TokenCode::SE_RPARENT},
    {"{", TokenCode::SE_LBRACES},
    {"}", TokenCode::SE_RBARCES},
    {";", TokenCode::SE_SEMICOLON},
    {",", TokenCode::SE_COMMA}
};

// 构造函数
Token::Token(TokenType type, const std::string& value) : type(type), value(value) {}

// 获取 Token 类型
TokenType Token::get_type() const {
    return type;
}

// 获取 Token 值
std::string Token::get_value() const {
    return value;
}

// 将 Token 转换为字符串表示
std::string Token::to_string() const {
    std::ostringstream oss;
    int classes;
    classes = tokenCodeMap[value]; // 获取 Token 的类别
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
        default: return "UNKNOWN";
    }
}