#include "symbol_table.h"
#include <iostream>

void Scope::addChild(LocalScope* s) {
    this->children.push_back(s); // 添加子作用域
}

Symbol* ToplevelScope::lookupSymbol(const std::string &name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.get(); // 返回符号的指针
    }
    DEBUG_ERROR("Symbol '" << name << "' not found in the top-level scope.");
    return nullptr; // 未找到符号
}

bool ToplevelScope::addDeclareSymbol(std::shared_ptr<Symbol> symbol) {
    auto it = symbols.find(symbol->name);
    if (it != symbols.end()) {
        DEBUG_ERROR("Symbol '" << symbol->name << "' already exists in the top-level scope.");
        return false; // 符号已存在
    }
    symbols[symbol->name] = symbol; // 插入符号
    return true;
}

bool ToplevelScope::addDefineSymbol(std::shared_ptr<Symbol> symbol) {
    auto it = symbols.find(symbol->name);
    if (it != symbols.end() && it->second->isDefined) {
        DEBUG_ERROR("Symbol '" << symbol->name << "' already defined in the top-level scope.");
        return false; // 符号已定义
    }
    if(it != symbols.end()) {
        it->second->isDefined = true; // 更新已定义标志
        return true; // 符号已存在，更新定义状态
    }
    symbols[symbol->name] = symbol; // 插入符号
    return true;
}

void ToplevelScope::printSymbolTable() const {
    std::cout << "Top-level Scope Symbol Table:" << std::endl;
    for (const auto& pair : symbols) {
        const auto& symbol = pair.second;
        std::cout << "Name: " << symbol->name 
                  << ", Type: " << symbol->type 
                  << ", IsConst: " << (symbol->isConst ? "true" : "false") 
                  << ", IsFunction: " << (symbol->isFunction ? "true" : "false") 
                  << ", IsDefined: " << (symbol->isDefined ? "true" : "false") 
                  << std::endl;
    }
}

LocalScope::LocalScope(Scope* parent) : parent_scope(parent) {
    if (parent_scope == nullptr) {
        DEBUG_ERROR("Parent scope cannot be null.");
    }
    parent_scope->addChild(this); // 将当前作用域添加到父作用域的子作用域列表中
}

Symbol* LocalScope::lookupSymbol(const std::string &name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.get(); // 返回符号的指针
    }
    return parent_scope->lookupSymbol(name); // 向上查找父作用域
}

bool LocalScope::addDeclareSymbol(std::shared_ptr<Symbol> symbol) {
    auto it = lookupSymbol(symbol->name);
    if (it != nullptr) {
        DEBUG_ERROR("Symbol '" << symbol->name << "' already exists in the local or higher scope.");
        return false; // 符号已存在
    }
    symbols[symbol->name] = symbol; // 插入符号
    return true; // 成功添加符号
}
bool LocalScope::addDefineSymbol(std::shared_ptr<Symbol> symbol) {
    auto it = lookupSymbol(symbol->name);
    if (it != nullptr && it->isDefined) {
        DEBUG_ERROR("Symbol '" << symbol->name << "' already defined in the local or higher scope.");
        return false; // 符号已定义
    }
    if(it != nullptr) {
        it->isDefined = true; // 更新已定义标志
        return true; // 符号已存在，更新定义状态
    }
    symbols[symbol->name] = symbol; // 插入符号
}