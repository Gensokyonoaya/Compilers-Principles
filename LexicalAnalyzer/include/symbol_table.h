#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

// 符号表中的符号信息
struct Symbol {
    std::string name;  // 标识符名称
    std::string type;  // 标识符类型（如 int, float 等）
    int scope_level;   // 作用域级别
};

class SymbolTable {
public:
    // 插入符号到符号表
    bool insert(const std::string& name, const std::string& type, int scope_level);

    // 查找符号
    Symbol* lookup(const std::string& name);

    // 删除指定作用域的符号
    void remove_by_scope(int scope_level);

private:
    std::unordered_map<std::string, Symbol> table; // 符号表存储
};

#endif