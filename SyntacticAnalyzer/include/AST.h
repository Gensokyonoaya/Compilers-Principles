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
    virtual std::string nodeType() const { return "ASTNode"; }

    // 打印节点信息到输出流
    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const {
        os << prefix;
        os << (isLast ? "└── " : "├── ") << nodeType() << "\n";
    }
};

// Base class for Types
class Type {
public:
    virtual ~Type() = default;
    virtual std::string nodeType() const { return "Type"; }
};

// Base class for Declarations
class Decl : public ASTNode {
public:
    virtual ~Decl() = default;
    virtual std::string nodeType() const override { return "Decl"; }
};

// Base class for Declarations that can hold multiple declarations (e.g., functions, classes)
class DeclContext : public Decl {
public:
    virtual ~DeclContext() = default;
    virtual std::string nodeType() const override { return "DeclContext"; }
    
    // 指向外层声明上下文
    DeclContext* declaredContext;

    // 用于存储多个声明的容器
    std::vector<std::unique_ptr<Decl>> declarations;
    virtual void addDecl(std::unique_ptr<Decl> decl) {
        declarations.push_back(std::move(decl));
    }
};

// Base class for Statements
class Stmt : public ASTNode {
public:
    virtual ~Stmt() = default;
    virtual std::string nodeType() const override { return "Stmt"; }
};

// ----------- Concrete Implementations -----------

// Type: BuiltinType (e.g., int, float)
class BuiltinType : public Type {
public:
    virtual ~BuiltinType() = default;
    enum Kind { Int, Float, Void, const_Int, const_Float };
    Kind kind;
    BuiltinType(Kind kind) : kind(kind) {}

    // 根据 kind 返回具体的类型名称
    virtual std::string nodeType() const override {
        switch (kind) {
            case Int: return "int";
            case Float: return "float";
            case Void: return "void";
            case const_Int: return "const int";
            case const_Float: return "const float";
            default: return "unknown";
        }
    }
};

// Statement: Expression Statement (e.g., a = 1 + 2;)
class Expr : public Stmt {
public:
    virtual ~Expr() = default;
    std::string expr;
    Expr(std::string expr) : expr(std::move(expr)) {}
    virtual std::string nodeType() const override { return "Expr"; }
};

class TranslationUnitDecl : public DeclContext {
public:
    virtual ~TranslationUnitDecl() = default;
    // 这里没有其他成员变量，因为它只是一个容器类
    TranslationUnitDecl() {
        // 可以初始化其他成员，确保它是根节点的结构
    }
    virtual std::string nodeType() const override { return "TranslationUnitDecl"; }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "TranslationUnitDecl\n";
        for (size_t i = 0; i < declarations.size(); ++i) {
            declarations[i]->print(os, prefix + (isLast ? "    " : "│   "), i == declarations.size() - 1);
        }
    }
};

// Declaration: Variable Declaration (e.g., int a;)
class VarDecl : public Decl {
public:
    virtual ~VarDecl() = default;
    std::string name;
    std::unique_ptr<Type> type;

    VarDecl(std::string name, std::unique_ptr<Type> type)
        : name(std::move(name)), type(std::move(type)) {}
    virtual std::string nodeType() const override { return "VarDecl"; }
    // 变量的初始值
    std::unique_ptr<Expr> initValue;  // 初始化值

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "VarDecl: " << name;
        if (type) {
            os << " (Type: " << type->nodeType() << ")";
        }
        os << "\n";

        if (initValue) {
            initValue->print(os, prefix + (isLast ? "    " : "│   "), true);
        }
    }
};

class ParmVarDecl : public VarDecl {
public:
    virtual ~ParmVarDecl() = default;
    bool isVariadic;  // 是否是可变参数

    ParmVarDecl(std::string name, std::unique_ptr<Type> type, bool isVariadic = false)
        : VarDecl(std::move(name), std::move(type)), isVariadic(isVariadic) {}
    virtual std::string nodeType() const override { return "ParmVarDecl"; }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "ParmVarDecl: " << name << "\n";
        if (type) {
            os << prefix + (isLast ? "    " : "│   ") << "└── Type: " << type->nodeType() << "\n";
        }
    }
};


class CompoundStmt : public Stmt {
    //用来表示由多个语句组成的代码块
public:
    virtual ~CompoundStmt() = default;
    //statements 向量，保存其包含的多个语句
    std::vector<std::unique_ptr<Stmt>> statements;

    void addStatement(std::unique_ptr<Stmt> stmt) {
        statements.push_back(std::move(stmt));
    }
    virtual std::string nodeType() const override { return "CompoundStmt"; }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "CompoundStmt\n";
        for (size_t i = 0; i < statements.size(); ++i) {
            statements[i]->print(os, prefix + (isLast ? "    " : "│   "), i == statements.size() - 1);
        }
    }
};


// Declaration: Function Declaration (e.g., int add(int a, int b))
class FunctionDecl : public DeclContext {
public:
    virtual ~FunctionDecl() = default;
    std::string name;
    std::unique_ptr<Type> returnType;
    std::vector<std::unique_ptr<ParmVarDecl>> params;  // 参数列表
    std::unique_ptr<CompoundStmt> body;  // 函数体（如果已定义）
    bool isDefined;  // 是否已经定义
    FunctionDecl(std::string name, std::unique_ptr<Type> returnType, 
                 bool isDefined = false)
        : name(std::move(name)), returnType(std::move(returnType)),
          isDefined(isDefined) {}
    virtual std::string nodeType() const override { return "FunctionDecl"; }
    void addParam(std::unique_ptr<ParmVarDecl> param) {
        params.push_back(std::move(param));
    }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "FunctionDecl: " << name;
        if (returnType) {
            os << " (ReturnType: " << returnType->nodeType() << ")";
        }
        os << "\n";

        for (size_t i = 0; i < params.size(); ++i) {
            params[i]->print(os, prefix + (isLast ? "    " : "│   "), i == params.size() - 1 && !body);
        }
        if (body) {
            body->print(os, prefix + (isLast ? "    " : "│   "), true);
        }
    }
};

class DeclStmt : public Stmt {
    //表示一个变量声明语句
public:
    virtual ~DeclStmt() = default;
    //保存一个 VarDecl（变量声明），例如 int a;
    std::vector<std::unique_ptr<Decl>> declarations;

    void addDecl(std::unique_ptr<Decl> decl) {
        declarations.push_back(std::move(decl));
    }
    virtual std::string nodeType() const override { return "DeclStmt"; }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "DeclStmt\n";
        for (size_t i = 0; i < declarations.size(); ++i) {
            declarations[i]->print(os, prefix + (isLast ? "    " : "│   "), i == declarations.size() - 1);
        }
    }
};




class ReturnStmt : public Stmt {
    //表示一个返回语句，例如 return a + b;
public:
    virtual ~ReturnStmt() = default;
    std::unique_ptr<Expr> expr;

    ReturnStmt() : expr(nullptr) {}  // 默认构造函数
    ReturnStmt(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}
    virtual std::string nodeType() const override { return "ReturnStmt"; }
    void setExpr(std::unique_ptr<Expr> expr) {
        this->expr = std::move(expr);
    }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "ReturnStmt\n";
        if (expr) {
            expr->print(os, prefix + (isLast ? "    " : "│   "), true);
        }
    }
};

// Statement: If Statement (e.g., if (a > b) { return 1; })
class IfStmt : public Stmt {
public:
    virtual ~IfStmt() = default;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
    virtual std::string nodeType() const override { return "IfStmt"; }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "IfStmt\n";

        // 打印条件表达式
        if (condition) {
            condition->print(os, prefix + (isLast ? "    " : "│   "), thenBranch == nullptr && elseBranch == nullptr);
        }

        // 打印 then 分支
        if (thenBranch) {
            thenBranch->print(os, prefix + (isLast ? "    " : "│   "), elseBranch == nullptr);
        }

        // 打印 else 分支
        if (elseBranch) {
            elseBranch->print(os, prefix + (isLast ? "    " : "│   "), true);
        }
    }
};

class FloatingLiteral : public Expr {
    //表示浮点数的字面量，例如 3.14
public:
    virtual ~FloatingLiteral() = default;
    float value;

    FloatingLiteral(float value) : Expr("FloatingLiteral"), value(value) {}
    virtual std::string nodeType() const override { return "FloatingLiteral"; }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "FloatingLiteral: " << value << "\n";
    }
};

class IntegerLiteral : public Expr {
    //表示整数的字面量，例如 42
public:
    virtual ~IntegerLiteral() = default;
    int value;

    IntegerLiteral(int value) : Expr("IntegerLiteral"), value(value) {}
    virtual std::string nodeType() const override { return "IntegerLiteral"; }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "IntegerLiteral: " << value << "\n";
    }
};

class BinaryOperator : public Expr {
    //表示一个二元运算符，例如 a + b、x - y
    //op 是运算符的字符串表示，例如 "+"、"-"、"*"、"/"
    //lhs 和 rhs 分别是运算符的左操作数和右操作数
public:
    virtual ~BinaryOperator() = default;
    std::string op;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;

    BinaryOperator(std::string op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : Expr("BinaryOperator"), op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    virtual std::string nodeType() const override { return "BinaryOperator"; }

    void set_rhs(std::unique_ptr<Expr> rhs) {
        this->rhs = std::move(rhs);
    }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "BinaryOperator: " << op << "\n";
        if (lhs) lhs->print(os, prefix + (isLast ? "    " : "│   "), rhs == nullptr);
        if (rhs) rhs->print(os, prefix + (isLast ? "    " : "│   "), true);
    }
};

class DeclRefExpr : public Expr {
    //表示一个对声明的引用，例如变量 a 或函数 add() 的引用。
public:
    virtual ~DeclRefExpr() = default;
    std::string name;
    std::unique_ptr<Decl> decl;

    DeclRefExpr(std::string name, std::unique_ptr<Decl> decl)
        : Expr("DeclRefExpr"), name(std::move(name)), decl(std::move(decl)) {}
    virtual std::string nodeType() const override { return "DeclRefExpr"; }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "DeclRefExpr: " << name << "\n";
    }
};

class UnaryOperator : public Expr {
    //表示一个一元运算符，例如 -a 或 !b
public:
    virtual ~UnaryOperator() = default;
    std::string op;
    std::unique_ptr<Expr> subExpr;

    UnaryOperator(std::string op, std::unique_ptr<Expr> subExpr)
        : Expr("UnaryOperator"), op(std::move(op)), subExpr(std::move(subExpr)) {}
    virtual std::string nodeType() const override { return "UnaryOperator"; }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "UnaryOperator: " << op << "\n";
        if (subExpr) subExpr->print(os, prefix + (isLast ? "    " : "│   "), true);
    }
};

class CallExpr : public Expr {
public:
    virtual ~CallExpr() = default;
    std::string callee;  // 被调用的函数名
    std::vector<std::unique_ptr<Expr>> args;  // 参数列表

    CallExpr(std::string callee)
        : Expr("CallExpr"), callee(std::move(callee)) {}
    virtual std::string nodeType() const override { return "CallExpr"; }

    void addArgument(std::unique_ptr<Expr> arg) {
        args.push_back(std::move(arg));
    }

    virtual void print(std::ostream& os, const std::string& prefix = "", bool isLast = true) const override {
        os << prefix << (isLast ? "└── " : "├── ") << "CallExpr: " << callee << "\n";
        for (size_t i = 0; i < args.size(); ++i) {
            args[i]->print(os, prefix + (isLast ? "    " : "│   "), i == args.size() - 1);
        }
    }
};

#endif