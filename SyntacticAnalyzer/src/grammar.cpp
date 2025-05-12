#include "grammar.h"
Grammar::Grammar(const std::string& grammarFile) {
    readGrammar(grammarFile);
    computeFirstSet(); // 计算 FIRST 集
    computeFollowSet(); // 计算 FOLLOW 集
    computeSelectSet(); // 计算 SELECT 集
    analyzeIsLL1(); // 分析文法是否是 LL(1) 文法
    DEBUG_INFO("Grammar loaded successfully from " + grammarFile);
}

void Grammar::readGrammar(const std::string& grammarFile) {
    std::ifstream file(grammarFile);
    if (!file.is_open()) {
        DEBUG_ERROR("Could not open grammar file: " + grammarFile);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // 跳过空行和注释
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // 查找 "->" 分隔符
        size_t arrowPos = line.find("->");
        size_t tab = line.find("\t");
        if (arrowPos == std::string::npos) {
            continue;
        }
        if (tab == std::string::npos) {
            continue;
        }
        // 检查 "->" 前是否有制表符
        // 提取非终结符和产生式
        std::string nonTerminal = line.substr(tab + 1, arrowPos - tab - 1);
        std::string production = line.substr(arrowPos + 2);

        // 去掉多余的空格
        trim(nonTerminal);
        trim(production);

        // 去掉productions最后的分号
        if (production.back() == ';') {
            production.pop_back();
        }

        // 解析产生式
        std::vector<std::vector<std::string>> symbols = parseProduction(production);

        insertNonTerminal(nonTerminal);
        // 将解析结果存储到规则中
        rules[nonTerminal] = symbols;

        // 设置起始符号（仅设置第一条文法）
        if (startSymbol.empty()) {
            startSymbol = nonTerminal;
        }
    }

    file.close();
}

void Grammar::insertTerminal(const std::string& terminal) {
    terminals.insert(terminal);
}

void Grammar::insertNonTerminal(const std::string& nonterminal){
    non_terminals.insert(nonterminal);
}

bool Grammar::isNonTerminal(const std::string& symbol) const {
    return non_terminals.find(symbol) != non_terminals.end();
}

bool Grammar::isTerminal(const std::string& symbol) const {
    return terminals.find(symbol) != terminals.end();
}

bool Grammar::isEpsilon(const std::string& symbol) const {
    return symbol == "$";
}

std::set<std::string> Grammar::getTerminals() const {
    return terminals;
}
std::set<std::string> Grammar::getNonTerminals() const {
    return non_terminals;
}
std::string Grammar::getStartSymbol() const {
    return startSymbol;
}
const std::map<std::string, std::vector<std::vector<std::string>>>& Grammar::getRules() const {
    return rules;
}
std::vector<std::vector<std::string>> Grammar::getProductions(const std::string& nonTerminal) const {
    auto it = rules.find(nonTerminal);
    if (it != rules.end()) {
        return it->second;
    }
    return {}; // Return an empty vector if the non-terminal is not found
}
std::set<std::string> Grammar::getFirstSet(const std::string& nonTerminal) const {
    return firstSets[nonTerminal];
}
std::set<std::string> Grammar::getFollowSet(const std::string& nonTerminal) const {
    return followSets[nonTerminal];
}

std::vector<std::vector<std::string>> Grammar::parseProduction(const std::string& production) {
    std::vector<std::vector<std::string>> productions;
    std::string currentSegment;
    std::string terminal;
    bool inQuotes = false; // 标记是否在单引号内

    for (size_t i = 0; i < production.size(); ++i) {
        char c = production[i];

        if (c == '\'') {
            inQuotes = !inQuotes; // 切换单引号状态
        }
        if (c != '\'' && inQuotes) {
            terminal += c; // 在单引号内，添加字符到终结符
        }
        if(!inQuotes && !terminal.empty()) {
            // 处理终结符
            insertTerminal(terminal); // 添加终结符到集合
            terminal.clear(); // 清空终结符
        }

        if (c == '|' && !inQuotes) {
            // 遇到分隔符且不在单引号内，处理当前段
            productions.push_back(splitIntoSymbols(currentSegment));
            currentSegment.clear();
        } else {
            // 添加字符到当前段
            if(c != '\'') currentSegment += c; // 忽略单引号
        }
    }

    // 处理最后一段
    if (!currentSegment.empty()) {
        productions.push_back(splitIntoSymbols(currentSegment));
    }

    return productions;
}

void Grammar::printGrammar() const {
    for (const auto& rule : rules) {
        std::cout << rule.first << " -> ";
        for (const auto& production : rule.second) {
            for (const auto& symbol : production) {
                std::cout << symbol << " ";
            }
            if (&production != &rule.second.back()) {
                std::cout << "| ";
            }
        }
        std::cout << std::endl;
    }
}

void Grammar::computeFirstSet() const {
    // 初始化终结符的 FIRST 集合
    for (const auto& terminal : terminals) {
        firstSets[terminal] = {terminal};
    }

    // 初始化非终结符的 FIRST 集为空
    for (const auto& nonTerminal : non_terminals) {
        firstSets[nonTerminal] = {};
    }

    bool changed = true;
    while (changed) {
        changed = false;

        // 遍历所有规则
        for (const auto& [lhs, productions] : rules) {
            for (const auto& production : productions) {
                bool addEpsilon = true; // 标记当前产生式是否能推出 ε
                size_t beforeSize = firstSets[lhs].size();

                for (const auto& symbol : production) {
                    if (isTerminal(symbol)) {
                        // 如果是终结符，直接加入 FIRST 集
                        firstSets[lhs].insert(symbol);
                        addEpsilon = false; // 终结符不能推出 ε
                        break;
                    } else if (isNonTerminal(symbol)) {
                        // 如果是非终结符，将其 FIRST 集（不含 ε）加入当前非终结符的 FIRST 集
                        const auto& firstSetOfSymbol = firstSets[symbol];
                        for (const auto& sym : firstSetOfSymbol) {
                            if (!isEpsilon(sym)) {
                                firstSets[lhs].insert(sym);
                            }
                        }

                        // 如果该非终结符的 FIRST 集不包含 ε，则停止
                        if (firstSetOfSymbol.find("$") == firstSetOfSymbol.end()) {
                            addEpsilon = false;
                            break;
                        }
                    } else if (isEpsilon(symbol)) {
                        // 如果是 ε，直接跳过
                        addEpsilon = true;
                    }
                }

                // 如果产生式的所有符号都能推出 ε，则将 ε 加入 FIRST 集
                if (addEpsilon) {
                    firstSets[lhs].insert("$");
                }

                // 检查是否有更新
                if (firstSets[lhs].size() > beforeSize) {
                    changed = true;
                }
            }
        }
    }
}

void Grammar::printFirstSets() const {
    std::cout << "FIRST Sets:" << std::endl;
    for (const auto& [nonTerminal, firstSet] : firstSets) {
        std::cout << nonTerminal << ": { ";
        for (const auto& symbol : firstSet) {
            std::cout << symbol << " ";
        }
        std::cout << "}" << std::endl;
    }
}


template <typename T>
std::string join(const std::set<T>& elements, const std::string& delimiter) {
    std::ostringstream oss;
    for (auto it = elements.begin(); it != elements.end(); ++it) {
        if (it != elements.begin()) {
            oss << delimiter;
        }
        oss << *it;
    }
    return oss.str();
}

template <typename T>
std::string join(const std::vector<T>& elements, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) {
            oss << delimiter;
        }
        oss << elements[i];
    }
    return oss.str();
}

void Grammar::computeFollowSet() const {
    // 初始化非终结符的 FOLLOW 集为空
    for (const auto& nonTerminal : non_terminals)
    {
        followSets[nonTerminal] = {};
    }
    followSets[startSymbol].insert("EOF"); // 将起始符号的 FOLLOW 集初始化为 { EOF }
    DEBUG_INFO("Initialized FOLLOW set for start symbol: " + startSymbol + " -> { EOF }");
    
    bool changed = true;
    //size_t iteration = 0; // 用于记录迭代次数

    // 打开日志文件
    // std::ofstream logFile("follow_set_log.txt");
    // if (!logFile.is_open()) {
    //     DEBUG_ERROR("Failed to open log file for FOLLOW set computation.");
    //     return;
    // }

    // logFile << "FOLLOW Set Computation Log\n";
    // logFile << "===========================\n";

    while (changed) {
        changed = false;
        // logFile << "Iteration " << ++iteration << ":\n";

        // 遍历所有规则
        for (const auto& [lhs, productions] : rules) {
            for (const auto& production : productions) {
                for (size_t i = 0; i < production.size(); ++i) {
                    const std::string& symbol = production[i];
                    if (isNonTerminal(symbol)) {
                        // logFile << "Processing non-terminal: " << symbol << " in production: " 
                                // << lhs << " -> " << join(production, " ") << "\n";
                        // 检查后续符号
                        if (i + 1 < production.size()) {
                            bool addFollow = true;
                            size_t pos = i + 1;
                            while(pos < production.size() && addFollow){
                                const std::string& nextSymbol = production[pos];
                                if (isTerminal(nextSymbol)) {
                                    // 如果后续符号是终结符，直接加入 FOLLOW 集
                                    if (followSets[symbol].insert(nextSymbol).second) {
                                        changed = true;
                                        // logFile << "  Added terminal " << nextSymbol 
                                        //         << " to FOLLOW(" << symbol << ")\n";
                                    }
                                    break;
                                } else if (isNonTerminal(nextSymbol)) {
                                    // 如果后续符号是非终结符，将其 FIRST 集（不含 ε）加入 FOLLOW 集
                                    const auto& firstSetOfNext = firstSets[nextSymbol];
                                    for (const auto& sym : firstSetOfNext) {
                                        if (!isEpsilon(sym)) {
                                            if (followSets[symbol].insert(sym).second) {
                                                changed = true;
                                                // logFile << "  Added FIRST(" << nextSymbol 
                                                // << ") element " << sym 
                                                // << " to FOLLOW(" << symbol << ")\n";
                                            }
                                        }
                                    }
                                    // 如果 FIRST 集包不含 ε，
                                    if (firstSetOfNext.find("$") == firstSetOfNext.end()) {
                                        addFollow = false;
                                    }
                                }
                                pos++;
                            }
                        } else {
                            // 如果是最后一个符号，将 FOLLOW(lhs) 加入 FOLLOW(symbol)
                            for (const auto& followSym : followSets[lhs]) {
                                if (followSets[symbol].insert(followSym).second) {
                                    changed = true;
                                    // logFile << "  Added FOLLOW(" << lhs 
                                    // << ") element " << followSym 
                                    // << " to FOLLOW(" << symbol << ")\n";
                                }
                            }
                        }
                    }
                }
            }
        }
        // 输出当前 FOLLOW 集状态
        // logFile << "Current FOLLOW sets after iteration " << iteration << ":\n";
        // for (const auto& [nonTerminal, followSet] : followSets) {
        //     logFile << "  FOLLOW(" << nonTerminal << ") = { " 
        //             << join(followSet, ", ") << " }\n";
        // }
        // logFile << "-----------------------------------\n";
    }
    // logFile << "FOLLOW set computation completed.\n";
    // logFile.close();
    DEBUG_INFO("FOLLOW set computation log written to follow_set_log.txt");
}

void Grammar::printFollowSets() const {
    std::cout << "FOLLOW Sets:" << std::endl;
    for (const auto& [nonTerminal, followSet] : followSets) {
        std::cout << nonTerminal << ": { ";
        for (const auto& symbol : followSet) {
            std::cout << symbol << " ";
        }
        std::cout << "}" << std::endl;
    }
}

void Grammar::computeSelectSet() const {
    // 计算 SELECT 集
    for (const auto& [lhs, productions] : rules) {
        for (const auto& production : productions) {
            std::set<std::string> selectSet;
            bool addEpsilon = true; // 标记当前产生式是否能推出 ε
            std::pair<std::string, std::vector<std::string>> selectPair(lhs, production);
            for (const auto& symbol : production) {
                if (isTerminal(symbol)) {
                    selectSet.insert(symbol);
                    addEpsilon = false;
                    break; // 终结符，停止
                } else if (isNonTerminal(symbol)) {
                    const auto& firstOfSymbol = firstSets.at(symbol);
                    selectSet.insert(firstOfSymbol.begin(), firstOfSymbol.end());
                    if (firstOfSymbol.find("$") == firstOfSymbol.end()) {
                        addEpsilon = false; // 非终结符且不包含 ε，停止
                        break; // 非终结符且不包含 ε，停止
                    }
                } else if (isEpsilon(symbol)) {
                    // 如果是 ε，继续
                    continue;
                }
            }
            // 如果产生式的所有符号都能推出 ε，则将 FOLLOW 集加入 SELECT 集
            if (addEpsilon) {
                const auto& followSet = followSets.at(lhs);
                selectSet.insert(followSet.begin(), followSet.end());
            }
            // 将 SELECT 集添加到SelectSets中
            selectSets[selectPair] = selectSet;
        }
    }
}

void Grammar::printSelectSets() const {
    std::cout << "SELECT Sets:" << std::endl;
    for (const auto& [pair, selectSet] : selectSets) {
        std::cout << "SELECT(" << pair.first << " -> ";
        for (const auto& symbol : pair.second) {
            std::cout << symbol << " ";
        }
        std::cout << ") = { ";
        for (const auto& symbol : selectSet) {
            std::cout << symbol << " ";
        }
        std::cout << "}" << std::endl;
    }
    std::cout << std::endl;
}

void Grammar::analyzeIsLL1(){
    // 分析文法是否是 LL(1) 文法
    isLL1 = true; // 默认是 LL(1) 文法

    // 检查 SELECT 集是否有冲突
    for(auto& [lhs, _] : rules) {
        std::set<std::string> intersection;
        for(auto& [pair, selectSet] : selectSets) {
            if(pair.first == lhs) {
                // 检查 SELECT 集是否有交集
                for(auto& symbol : selectSet) {
                    if(intersection.find(symbol) != intersection.end()) {
                        isLL1 = false; // 发现冲突，标记为非 LL(1)
                        std::cout << "Conflict found in SELECT set for " << lhs 
                                  << ": " << join(intersection, ", ") 
                                  << " and " << symbol << std::endl;
                        break;
                    }
                    intersection.insert(symbol);
                }
            }
            if(!isLL1) {
                break; // 发现冲突，提前退出
            }
        }
        if(!isLL1) {
            break; // 发现冲突，提前退出
        }
    }
}