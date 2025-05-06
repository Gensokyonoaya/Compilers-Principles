#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cctype>
#include "utils.h"

class Grammar {
public:
    // Constructor
    Grammar(const std::string& grammarFile);

    // Destructor
    ~Grammar() = default;

    void insertTerminal(const std::string& terminal); // Insert a terminal into the grammar

    void insertNonTerminal(const std::string& nonTerminal); // Insert a non-terminal into the grammar

    // Function to parse a production string into symbols
    std::vector<std::vector<std::string>> parseProduction(const std::string& production);

    // Function to get the first set of a non-terminal
    std::set<std::string> getFirstSet(const std::string& nonTerminal) const;

    // Function to get the follow set of a non-terminal
    std::set<std::string> getFollowSet(const std::string& nonTerminal) const;

    // Function to print the grammar rules
    void printGrammar() const;

    // Function to print the first sets
    void printFirstSets() const;

    // Function to print the follow sets
    void printFollowSets() const;

    // Function to check if a string is a terminal
    bool isTerminal(const std::string& symbol) const;

    // Function to check if a string is a non-terminal
    bool isNonTerminal(const std::string& symbol) const;

    bool isEpsilon(const std::string& symbol) const; // Check if a string is epsilon

    // Function to get the productions for a non-terminal
    std::vector<std::vector<std::string>> getProductions(const std::string& nonTerminal) const;

    std::set<std::string> getTerminals() const; // Get the set of terminals
    std::set<std::string> getNonTerminals() const; // Get the set of non-terminals

    // Function to get the start symbol of the grammar
    std::string getStartSymbol() const;

    // Function to get the grammar rules
    const std::map<std::string, std::vector<std::vector<std::string>>>& getRules() const;

private:
    // Function to read the grammar from a file
    void readGrammar(const std::string& grammarFile);

    // Function to compute the first set of a production
    void computeFirstSet() const;

    // Function to compute the follow set of a non-terminal
    void computeFollowSet() const;

    // Grammar rules
    std::map<std::string, std::vector<std::vector<std::string>>> rules;

    // Set of terminals
    std::set<std::string> terminals;

    // Set of non-terminals
    std::set<std::string> non_terminals;

    // Start symbol
    std::string startSymbol;

    // First sets
    mutable std::map<std::string, std::set<std::string>> firstSets;

    // Follow sets
    mutable std::map<std::string, std::set<std::string>> followSets;
};


#endif