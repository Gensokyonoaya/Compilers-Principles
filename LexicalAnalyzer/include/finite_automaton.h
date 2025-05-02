#ifndef FINITE_AUTOMATON_H
#define FINITE_AUTOMATON_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include "utils.h"

// 状态类型
struct State {
    int id; // 状态编号
    bool is_final; // 是否是终止状态
    std::string token_type; // 终止状态对应的 Token 类型
};

// 有限自动机类
class FiniteAutomaton {
public:
    FiniteAutomaton();

    // 添加状态
    void add_state(int id, bool is_final = false, const std::string& token_type = "");

    // 添加转换规则
    void add_transition(int from, char input, int to);

    // 设置初始状态
    void set_start_state(int id);

    // 获取 DFA 的下一个状态
    int get_next_state(int current_state, char input) const;

    // 确定化和最小化
    FiniteAutomaton determinize() const;
    FiniteAutomaton minimize() const;

    // 打印自动机
    void print() const;

    const std::unordered_map<int, State>& get_states() const {
        return states;
    }

private:
    int start_state; // 初始状态
    std::unordered_map<int, State> states; // 状态集合
    std::unordered_map<int, std::unordered_map<char, int>> transitions; // 转换规则
};

#endif