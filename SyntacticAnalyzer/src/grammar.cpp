#include "grammar.h"
Grammar::Grammar(const std::string& grammarFile) {
    readGrammar(grammarFile);
    computeFirstSet(); // 计算 FIRST 集
    computeFollowSet(); // 计算 FOLLOW 集
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

void Grammar::computeFollowSet() const {
    // 初始化非终结符的 FOLLOW 集为空
    for (const auto& nonTerminal : non_terminals)
    {
        followSets[nonTerminal] = {};
    }
    followSets[startSymbol].insert("$"); // 将起始符号的 FOLLOW 集初始化为 { $ }
    
    bool changed = true;
    while (changed) {
        changed = false;

        // 遍历所有规则
        for (const auto& [lhs, productions] : rules) {
            for (const auto& production : productions) {
                for (size_t i = 0; i < production.size(); ++i) {
                    const std::string& symbol = production[i];
                    if (isNonTerminal(symbol)) {
                        // 检查后续符号
                        if (i + 1 < production.size()) {
                            const std::string& nextSymbol = production[i + 1];
                            if (isTerminal(nextSymbol)) {
                                // 如果后续符号是终结符，直接加入 FOLLOW 集
                                if (followSets[symbol].insert(nextSymbol).second) {
                                    changed = true;
                                }
                            } else if (isNonTerminal(nextSymbol)) {
                                // 如果后续符号是非终结符，将其 FIRST 集（不含 ε）加入 FOLLOW 集
                                const auto& firstSetOfNext = firstSets[nextSymbol];
                                for (const auto& sym : firstSetOfNext) {
                                    if (!isEpsilon(sym)) {
                                        if (followSets[symbol].insert(sym).second) {
                                            changed = true;
                                        }
                                    }
                                }
                                // 如果 FIRST 集包含 ε，将 FOLLOW(lhs) 加入 FOLLOW(symbol)
                                if (firstSetOfNext.find("$") != firstSetOfNext.end()) {
                                    for (const auto& followSym : followSets[lhs]) {
                                        if (followSets[symbol].insert(followSym).second) {
                                            changed = true;
                                        }
                                    }
                                }
                            }
                        } else {
                            // 如果是最后一个符号，将 FOLLOW(lhs) 加入 FOLLOW(symbol)
                            for (const auto& followSym : followSets[lhs]) {
                                if (followSets[symbol].insert(followSym).second) {
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
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