#include "lexer.h"
#include <iostream>

int main() {
    std::string source_code = R"(
        int main() {
            float x = 3.14;
            const int y = 42;
            if ((x == y) {
                x = 0;
            } else if (x != y) {
                if (x <= y) {
                    x = y;
                } else {
                    x = -1;
                }
            } 
            if (x >= 0) return x;
        }
    )";

    Lexer lexer(source_code);
    lexer.print_tokens();

    return 0;
}