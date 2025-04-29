#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    KEYWORD,        // 关键字
    IDENTIFIER,     // 标识符
    OPERATOR,       // 运算符
    INTEGER,        // 整数
    FLOAT,          // 浮点数
    SYMBOL_OF_END,  // 界符
    END_OF_FILE     // 文件结束符
};

class Token {
public:
    Token(TokenType type, const std::string& value);

    TokenType get_type() const;
    std::string get_value() const;

    // 将 Token 转换为字符串表示
    std::string to_string() const;

private:
    TokenType type;
    std::string value;

    // 将 TokenType 转换为字符串
    static std::string token_type_to_string(TokenType type);
};

#endif