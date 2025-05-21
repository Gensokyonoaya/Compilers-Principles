#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <unordered_map>

enum class TokenType {
    KEYWORD,        // 关键字
    IDENTIFIER,     // 标识符
    OPERATOR,       // 运算符
    INTEGER,        // 整数
    FLOAT,          // 浮点数
    SYMBOL_OF_END,  // 界符
    UNDEFINED,      // 未定义符
    END_OF_FILE  // 文件结束符
};

// enum TokenCode {
//     UNDEFINE = 0, // 未定义

//     KW_INT = 1,
//     KW_VOID,
//     KW_RETURN,
//     KW_CONST,
//     KW_MAIN,
//     KW_FLOAT,
//     KW_IF,
//     KW_ELSE,

//     SE_LPARENT = 20, // (
//     SE_RPARENT,      // )
//     SE_LBRACES,      // {
//     SE_RBARCES,      // }
//     SE_SEMICOLON,   // ;
//     SE_COMMA,       // ,

//     OP_PLUS = 30,
//     OP_MINUS,
//     OP_MULTIPLY,
//     OP_DIVIDE,
//     OP_PERCENT,
//     OP_ASSIGN,
//     OP_GT,
//     OP_LT,
//     OP_EQUAL,
//     OP_LEQ,
//     OP_GEQ,
//     OP_NEQ,
//     OP_AND,
//     OP_OR,
//     OP_NOT,

//     INT_CONST = 50, // 整数常量
//     FLOAT_CONST,    // 浮点数常量

//     IDNTIFIER = 60, // 标识符

//     END_OF_FILE = 100, // 文件结束符
// };

enum TokenCode
{
    UNDEFINE = 0, // undefined

    // KW_关键字
    KW_INT,
    KW_VOID,
    KW_RETURN,
    KW_CONST,
    KW_MAIN,
    KW_FLOAT,
    KW_IF,
    KW_ELSE,

    // OP_运算符
    OP_PLUS,     // +
    OP_MINUS,    // -
    OP_MULTIPLY, // *
    OP_DIVIDE,   // /
    OP_PERCENT,  // %
    OP_ASSIGN,   // =
    OP_GT,       // >
    OP_LT,       // <
    OP_EQUAL,    // ==
    OP_LEQ,      // <=
    OP_GEQ,      // >=
    OP_NEQ,      // !=
    OP_AND,      // &&
    OP_OR,       // ||
    OP_NOT,      // !

    // SE_界符
    SE_LPARENT,   // (
    SE_RPARENT,   // )
    SE_LBRACES,   // {
    SE_RBARCES,   // }
    SE_SEMICOLON, // ;
    SE_COMMA,     // ,

    // IDN_标识符
    IDNTIFIER,

    // INT_整数
    INT_CONST,

    // FLOAT_浮点数
    FLOAT_CONST,
    
    END_OF_FILE
};

// 声明 tokenCodeMap
extern std::unordered_map<std::string, TokenCode> tokenCodeMap;
extern std::unordered_map<TokenCode, std::string> tokenCodeToStringMap;

class Token {
public:
    Token();
    Token(TokenType type, const std::string& value);

    TokenType get_type() const;
    std::string get_value() const;
    TokenCode get_code() const;

    // 将 Token 转换为字符串表示
    std::string to_string() const;

private:
    TokenType type;
    std::string value;
    TokenCode code;
    // 将 TokenType 转换为字符串
    static std::string token_type_to_string(TokenType type);
};

#endif