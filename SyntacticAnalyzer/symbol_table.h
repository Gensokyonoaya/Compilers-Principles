#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "utils.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

class Symbol {
public:
    std::string name; // 符号名称
    std::string type; // 符号类型
    bool isConst; // 是否为常量

    Symbol(const std::string &name, const std::string &type, bool isConst = false)
        : name(name), type(type), isConst(isConst) {}

    virtual ~Symbol() = default; // 虚析构函数
};

class Variable : public Symbol {
public:
    bool isDefined; // 是否已定义
    Variable(const std::string &name, const std::string &type, bool isConst = false, bool isDefined = false)
        : Symbol{name, type, isConst}, isDefined(isDefined) {}
    Variable(const std::string &name, const std::string &type, bool isDefined = false)  
        : Symbol{name, type}, isDefined(isDefined) {} // 默认非常量
};

class Function : public Symbol {
public:
    std::vector<std::shared_ptr<Variable>> parameters; // 函数参数列表
    bool isInitialized; // 是否已初始化
    Function(const std::string &name, const std::string &type, bool isConst = false, bool isInitialized = false)
        : Symbol{name, type, isConst}, isInitialized(isInitialized) {}
    Function(const std::string &name, const std::string &type, bool isInitialized = false)
        : Symbol{name, type}, isInitialized(isInitialized) {} // 默认非常量

    void addParameter(std::shared_ptr<Variable> param) {
        parameters.push_back(param); // 添加参数
    }
};

class LocalScope; // 前向声明

class Scope{
public:
    virtual ~Scope() = default;
    virtual bool isToplevel() const = 0; // 是否为顶层作用域
    virtual class ToplevelScope* toplevel() = 0; // 转换为顶层作用域
    virtual Scope* parent() const = 0;

    virtual Symbol* lookupSymbol(const std::string &name) = 0; // 查找符号
    virtual void printSymbolTable() const = 0; // 打印符号表

    void addChild(LocalScope* s); // 添加子作用域

private:
    std::vector<LocalScope*> children; // 子作用域列表
};

class ToplevelScope : public Scope {
public:
    ToplevelScope() = default;
    bool isToplevel() const override { return true; } // 是否为顶层作用域
    ToplevelScope* toplevel() override { return this; } // 转换为顶层作用域
    Scope* parent() const override { return nullptr; } // 顶层作用域没有父作用域

    Symbol* lookupSymbol(const std::string &name) override; // 查找符号
    bool addDeclareSymbol(std::shared_ptr<Symbol> symbol); // 添加声明符号
    bool addDefineSymbol(std::shared_ptr<Symbol> symbol); // 添加定义符号
    
    void printSymbolTable() const override; // 打印符号表

private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols; // 符号表
    std::vector<LocalScope*> children; // 子作用域列表
};

class LocalScope : public Scope {
public:
    LocalScope(Scope* parent); // 构造函数，传入父作用域指针
    bool isToplevel() const override { return false; } // 是否为顶层作用域
    ToplevelScope* toplevel() override { return parent_scope->toplevel(); } // 转换为顶层作用域
    Scope* parent() const override { return parent_scope; } // 返回父作用域

    Symbol* lookupSymbol(const std::string &name) override; // 查找符号
    bool addDeclareSymbol(std::shared_ptr<Symbol> symbol); // 添加声明符号
    bool addDefineSymbol(std::shared_ptr<Symbol> symbol); // 添加定义符号
    void printSymbolTable() const override; // 打印符号表
private:
    Scope* parent_scope; // 父作用域指针
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols; // 符号表
    std::vector<LocalScope*> children; // 子作用域列表
};

#endif