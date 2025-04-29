#include "symbol_table.h"

// 插入符号到符号表
bool SymbolTable::insert(const std::string& name, const std::string& type, int scope_level) {
    // 如果符号已存在，则插入失败
    if (table.find(name) != table.end()) {
        return false;
    }
    // 插入新符号
    table[name] = {name, type, scope_level};
    return true;
}

// 查找符号
Symbol* SymbolTable::lookup(const std::string& name) {
    auto it = table.find(name);
    if (it != table.end()) {
        return &it->second;
    }
    return nullptr; // 未找到符号
}

// 删除指定作用域的符号
void SymbolTable::remove_by_scope(int scope_level) {
    for (auto it = table.begin(); it != table.end(); ) {
        if (it->second.scope_level == scope_level) {
            it = table.erase(it); // 删除符号
        } else {
            ++it;
        }
    }
}