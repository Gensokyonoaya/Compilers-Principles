#include "finite_automaton.h"
#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <algorithm>

// 构造函数
FiniteAutomaton::FiniteAutomaton() : start_state(-1) {}

// 添加状态
void FiniteAutomaton::add_state(int id, bool is_final, const std::string& token_type) {
    states[id] = {id, is_final, token_type};
}

// 添加转换规则
void FiniteAutomaton::add_transition(int from, char input, int to) {
    transitions[from][input] = to;
}

// 设置初始状态
void FiniteAutomaton::set_start_state(int id) {
    start_state = id;
}

// 获取 DFA 的下一个状态
int FiniteAutomaton::get_next_state(int current_state, char input) const {
    if (transitions.find(current_state) == transitions.end()) {
        //DEBUG_ERROR("State " + std::to_string(current_state) + " not found in transitions.");
        return -1;
    }
    if (transitions.at(current_state).find(input) == transitions.at(current_state).end()) {
        //DEBUG_ERROR("Input '" + std::string(1, input) + "' not found for state " + std::to_string(current_state) + ".");
        return -1;
    }
    return transitions.at(current_state).at(input);
}

// 确定化（NFA -> DFA）
FiniteAutomaton FiniteAutomaton::determinize() const {
    FiniteAutomaton dfa;
    std::map<std::set<int>, int> state_mapping; // NFA 状态集合到 DFA 状态的映射
    std::queue<std::set<int>> worklist;
    int next_state_id = 0;

    // 初始状态
    std::set<int> start_set = {start_state};
    worklist.push(start_set);
    state_mapping[start_set] = next_state_id++;

	// 检查初始状态是否包含终止状态
    bool is_final = false;
    std::string token_type;
    for (int state : start_set) {
        if (states.find(state) != states.end() && states.at(state).is_final) {
            is_final = true;
            token_type = states.at(state).token_type; // 使用第一个终止状态的 token_type
            break;
        }
    }
    dfa.add_state(state_mapping[start_set], is_final, token_type);

    // 确定化过程
    while (!worklist.empty()) {
        std::set<int> current_set = worklist.front();
        worklist.pop();
        int current_dfa_state = state_mapping[current_set];

        // 遍历所有可能的输入字符
        std::map<char, std::set<int>> transitions_for_input;
        for (int state : current_set) {
            if (transitions.count(state)) {
                for (const auto& [input, next_state] : transitions.at(state)) {
                    transitions_for_input[input].insert(next_state);
                }
            }
        }

		// 为每个输入字符创建新的 DFA 状态
		for (const auto& [input, next_set] : transitions_for_input) {
			if (!state_mapping.count(next_set)) {
				state_mapping[next_set] = next_state_id++;

				// 检查新状态是否包含终止状态
				bool is_final = false;
				std::string token_type;
				for (int state : next_set) {
					if (states.find(state) != states.end() && states.at(state).is_final) {
						is_final = true;
						token_type = states.at(state).token_type; // 使用第一个终止状态的 token_type
						break;
					}
				}
				dfa.add_state(state_mapping[next_set], is_final, token_type);
				worklist.push(next_set);
			}
			dfa.add_transition(current_dfa_state, input, state_mapping[next_set]);
		}
    }

    return dfa;
}

// 最小化 DFA
FiniteAutomaton FiniteAutomaton::minimize() const {
    FiniteAutomaton minimized_dfa;
    if (states.empty()) return minimized_dfa;

    std::unordered_set<int> final_states;
    std::unordered_set<int> non_final_states;

    // 1. 将状态分成终止和非终止两类
    for (const auto& [id, state] : states) {
        if (state.is_final) {
            final_states.insert(id);
        } else {
            non_final_states.insert(id);
        }
    }

    // 确保初始状态被包含在某个分组中
    if (states.find(start_state) != states.end()) {
        if (states.at(start_state).is_final) {
            final_states.insert(start_state);
        } else {
            non_final_states.insert(start_state);
        }
    }

    std::vector<std::unordered_set<int>> partitions;
    if (!non_final_states.empty()) partitions.push_back(non_final_states);
    if (!final_states.empty()) partitions.push_back(final_states);
    

    std::vector<std::unordered_set<int>> worklist = partitions;

    // 2. 开始细分
    while (!worklist.empty()) {
        auto A = worklist.back();
        worklist.pop_back();

        std::unordered_set<char> alphabet;
        // 收集所有可能的输入字符
        for (const auto& [from, trans] : transitions) {
            for (const auto& [input, to] : trans) {
                alphabet.insert(input);
            }
        }

        for (char c : alphabet) {
            // 找到所有通过字符 c 可以到达集合 A 的状态
            std::unordered_set<int> X;
            for (const auto& [from, trans] : transitions) {
                if (trans.count(c) && A.count(trans.at(c))) {
                    X.insert(from);
                }
            }

            // 对每个分组 Y 进行检查
            std::vector<std::unordered_set<int>> new_partitions;
            for (auto& Y : partitions) {
                std::unordered_set<int> intersection;
                std::unordered_set<int> difference;

                for (int state : Y) {
                    if (X.count(state)) {
                        intersection.insert(state);
                    } else {
                        difference.insert(state);
                    }
                }

                if (!intersection.empty() && !difference.empty()) {
                    // Y 被分裂了
                    new_partitions.push_back(intersection);
                    new_partitions.push_back(difference);

                    // 如果 Y 在工作队列里，用 intersection 和 difference 替换
                    auto it = std::find(worklist.begin(), worklist.end(), Y);
                    if (it != worklist.end()) {
                        worklist.erase(it);
                        worklist.push_back(intersection);
                        worklist.push_back(difference);
                    } else {
                        // 将较小的集合加入工作队列
                        if (intersection.size() <= difference.size()) {
                            worklist.push_back(intersection);
                        } else {
                            worklist.push_back(difference);
                        }
                    }
                } else {
                    new_partitions.push_back(Y);
                }
            }
            partitions = std::move(new_partitions);
        }
    }

    // 3. 建立新的最小化DFA
    std::unordered_map<int, int> old_to_new_state;
    int new_state_id = 0;
    for (const auto& group : partitions) {
        for (int state : group) {
            old_to_new_state[state] = new_state_id;
        }

        // 确定新状态是否为终止状态，并设置 token_type
        int representative = *group.begin();
        if (states.find(representative) != states.end()) {
            bool is_final = states.at(representative).is_final;
            std::string token_type;

            // 如果是终止状态，选择第一个终止状态的 token_type
            for (int state : group) {
                if (states.find(state) != states.end() && states.at(state).is_final) {
                    token_type = states.at(state).token_type;
                    break;
                }
            }

            minimized_dfa.add_state(new_state_id, is_final, token_type);
        }
        ++new_state_id;
    }

    // 4. 添加新的转移关系
    for (const auto& [from, trans] : transitions) {
        for (const auto& [input, to] : trans) {
            int new_from = old_to_new_state[from];
            int new_to = old_to_new_state[to];
            minimized_dfa.add_transition(new_from, input, new_to);
        }
    }

    // 5. 设置新的初始状态并确保其编号为 0
    if (old_to_new_state.find(start_state) != old_to_new_state.end()) {
        int original_start_state = old_to_new_state.at(start_state);

        // 如果初始状态已经是 0，则无需调整
        if (original_start_state != 0) {
            // 重新映射状态编号，将原始初始状态映射为 0
            std::unordered_map<int, int> remapped_states;
            for (const auto& [old_state, new_state] : old_to_new_state) {
                if (new_state == original_start_state) {
                    remapped_states[old_state] = 0;
                } else if (new_state == 0) {
                    remapped_states[old_state] = original_start_state;
                } else {
                    remapped_states[old_state] = new_state;
                }
            }

            // 更新最小化 DFA 的状态和转换
            FiniteAutomaton remapped_dfa;
            for (const auto& [old_state, new_state] : remapped_states) {
                const auto& state = states.at(old_state);
                remapped_dfa.add_state(new_state, state.is_final, state.token_type);
            }
            for (const auto& [from, trans] : transitions) {
                for (const auto& [input, to] : trans) {
                    int new_from = remapped_states[from];
                    int new_to = remapped_states[to];
                    remapped_dfa.add_transition(new_from, input, new_to);
                }
            }
            remapped_dfa.set_start_state(0);
            return remapped_dfa;
        } else {
            minimized_dfa.set_start_state(0);
        }
    } else {
        DEBUG_ERROR("Start state not found in minimized DFA.");
    }

    return minimized_dfa;
}

// 打印自动机
void FiniteAutomaton::print() const {
    std::cout << "Start State: " << start_state << "\n";
    for (const auto& [id, state] : states) {
        std::cout << "State " << id 
                  << (state.is_final ? " (final)" : "") 
                  << (state.is_final ? ", token_type=\"" + state.token_type + "\"" : "")
                  << "\n";
    }
    for (const auto& [from, trans] : transitions) {
        for (const auto& [input, to] : trans) {
            std::cout << "  " << from << " --" << input << "--> " << to << "\n";
        }
    }
}