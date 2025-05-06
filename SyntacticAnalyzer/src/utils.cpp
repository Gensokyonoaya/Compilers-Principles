#include "../include/utils.h"

// Helper function to trim leading and trailing whitespace from a string
void trim(std::string& str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), str.end());
}

std::vector<std::string> splitIntoSymbols(const std::string& segment) {
    std::vector<std::string> symbols;
    std::stringstream segmentStream(segment);
    std::string token;

    while (segmentStream >> token) {
        symbols.push_back(token);
    }

    return symbols;
}