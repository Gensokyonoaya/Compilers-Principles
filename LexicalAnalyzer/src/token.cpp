#include "token.h"
#include <sstream>

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
    oss << "Token(" << token_type_to_string(type) << ", \"" << value << "\")";
    return oss.str();
}

// 将 TokenType 转换为字符串
std::string Token::token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::SYMBOL_OF_END: return "SYMBOL_OF_END";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "UNKNOWN";
    }
}