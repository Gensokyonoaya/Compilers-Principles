#include "grammar.h"
Grammar::Grammar(const std::string& grammarFile) {
    readGrammar(grammarFile);
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
    return symbol == "$" || symbol == "ε";
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
            std::cout << "| ";
        }
        std::cout << std::endl;
    }
}

void Grammar::computeFirstSet() const {
    // 初始化每个终结符的 FIRST 集合为自身
    for (const auto& terminal : terminals) {
        firstSets[terminal].insert(terminal);
    }

    // 初始化每个非终结符的 FIRST 集合为空集合
    for (const auto& nonTerminal : non_terminals) {
        firstSets[nonTerminal] = std::set<std::string>();
    }

    // 计算 FIRST 集合
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& rule : rules) {
            const std::string& nonTerminal = rule.first;
            const auto& productions = rule.second;

            for (const auto& production : productions) {
                for (const auto& symbol : production) {
                    if (isTerminal(symbol)) {
                        // 若X∈Vn ，且有产生式X→a，a∈Vt，则a∈FIRST(X)
                        if (firstSets[nonTerminal].insert(symbol).second) {
                            changed = true;
                        }
                        break; // 终止当前产生式的处理
                    } else if (isNonTerminal(symbol)) {
                        // 如果是非终结符，加入其 FIRST 集合
                        const auto& firstSet = getFirstSet(symbol);
                        size_t oldSize = firstSets[nonTerminal].size();
                        firstSets[nonTerminal].insert(firstSet.begin(), firstSet.end());
                        if (firstSets[nonTerminal].size() > oldSize) {
                            changed = true;
                        }
                        if (firstSet.find("ε") == firstSet.end()) {
                            break; // 如果 FIRST 集合不包含 ε，终止当前产生式的处理
                        }
                    } else if (isEpsilon(symbol)) {
                        // 如果是 ε，加入 FIRST 集合
                        if (firstSets[nonTerminal].insert("$").second) {
                            changed = true;
                        }
                        break; // 终止当前产生式的处理
                    } else {
                        DEBUG_ERROR("Unknown symbol: " + symbol);
                    }
                }
            }
        }
    }
}