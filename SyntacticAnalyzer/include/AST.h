#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include "../../LexicalAnalyzer/include/utils.h"

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Base class for Types
class Type {
public:
    virtual ~Type() = default;
};

// Base class for Declarations
class Decl : public ASTNode {
public:
    virtual ~Decl() = default;
};

// Base class for Declarations that can hold multiple declarations (e.g., functions, classes)
class DeclContext : public Decl {
public:
    std::vector<std::unique_ptr<Decl>> declarations;
    virtual void addDecl(std::unique_ptr<Decl> decl) {
        declarations.push_back(std::move(decl));
    }
    virtual ~DeclContext() = default;
};

// Base class for Statements
class Stmt : public ASTNode {
public:
    virtual ~Stmt() = default;
};

// ----------- Concrete Implementations -----------

// Type: BuiltinType (e.g., int, float)
class BuiltinType : public Type {
public:
    enum Kind { Int, Float, Void };
    Kind kind;
    BuiltinType(Kind kind) : kind(kind) {}
};

class TranslationUnitDecl : public DeclContext {
public:
    // 这里没有其他成员变量，因为它只是一个容器类
    TranslationUnitDecl() {
        // 可以初始化其他成员，确保它是根节点的结构
    }

};

// Declaration: Variable Declaration (e.g., int a;)
class VarDecl : public Decl {
public:
    std::string name;
    std::unique_ptr<Type> type;
    VarDecl(std::string name, std::unique_ptr<Type> type)
        : name(std::move(name)), type(std::move(type)) {}
};

class ParmVarDecl : public VarDecl {
public:
    bool isVariadic;  // 是否是可变参数

    ParmVarDecl(std::string name, std::unique_ptr<Type> type, bool isVariadic = false)
        : VarDecl(std::move(name), std::move(type)), isVariadic(isVariadic) {}

};

// Declaration: Function Declaration (e.g., int add(int a, int b))
class FunctionDecl : public DeclContext {
public:
    std::string name;
    std::unique_ptr<Type> returnType;
    FunctionDecl(std::string name, std::unique_ptr<Type> returnType)
        : name(std::move(name)), returnType(std::move(returnType)) {}
};

class CompoundStmt : public Stmt {
    //用来表示由多个语句组成的代码块
public:
    //statements 向量，保存其包含的多个语句
    std::vector<std::unique_ptr<Stmt>> statements;

    void addStatement(std::unique_ptr<Stmt> stmt) {
        statements.push_back(std::move(stmt));
    }
};

class DeclStmt : public Stmt {
    //表示一个变量声明语句
public:
    //保存一个 VarDecl（变量声明），例如 int a;
    std::unique_ptr<VarDecl> decl;

    DeclStmt(std::unique_ptr<VarDecl> decl) : decl(std::move(decl)) {}

};

// Statement: Expression Statement (e.g., a = 1 + 2;)
class Expr : public Stmt {
public:
    std::string expr;
    Expr(std::string expr) : expr(std::move(expr)) {}
};

class ReturnStmt : public Stmt {
    //表示一个返回语句，例如 return a + b;
public:
    std::unique_ptr<Expr> expr;

    ReturnStmt(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}

};

// Statement: If Statement (e.g., if (a > b) { return 1; })
class IfStmt : public Stmt {
public:
    std::string condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    IfStmt(std::string condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
};

class FloatingLiteral : public Expr {
    //表示浮点数的字面量，例如 3.14
public:
    float value;

    FloatingLiteral(float value) : Expr("FloatingLiteral"), value(value) {}

};

class IntegerLiteral : public Expr {
    //表示整数的字面量，例如 42
public:
    int value;

    IntegerLiteral(int value) : Expr("IntegerLiteral"), value(value) {}

};

class BinaryOperator : public Expr {
    //表示一个二元运算符，例如 a + b、x - y
    //op 是运算符的字符串表示，例如 "+"、"-"、"*"、"/"
    //lhs 和 rhs 分别是运算符的左操作数和右操作数
public:
    std::string op;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;

    BinaryOperator(std::string op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : Expr("BinaryOperator"), op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

};

class DeclRefExpr : public Expr {
    //表示一个对声明的引用，例如变量 a 或函数 add() 的引用。
public:
    std::string name;
    std::unique_ptr<Decl> decl;

    DeclRefExpr(std::string name, std::unique_ptr<Decl> decl)
        : Expr("DeclRefExpr"), name(std::move(name)), decl(std::move(decl)) {}

};

class UnaryOperator : public Expr {
    //表示一个一元运算符，例如 -a 或 !b
public:
    std::string op;
    std::unique_ptr<Expr> subExpr;

    UnaryOperator(std::string op, std::unique_ptr<Expr> subExpr)
        : Expr("UnaryOperator"), op(std::move(op)), subExpr(std::move(subExpr)) {}


};

class CallExpr : public Expr {
public:
    std::string callee;  // 被调用的函数名
    std::vector<std::unique_ptr<Expr>> args;  // 参数列表

    CallExpr(std::string callee, std::vector<std::unique_ptr<Expr>> args)
        : Expr("CallExpr"), callee(std::move(callee)), args(std::move(args)) {}


};

#endif