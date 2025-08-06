# 🚀 Quick Start Guide

## Building and Running the C-- Compiler

### Prerequisites
- C++17 compatible compiler (g++, clang++)
- Make build system
- Any modern operating system (Windows/Linux/macOS)

### Build Commands
```bash
# Navigate to test directory
cd test

# Build using the new cross-platform Makefile
make -f Makefile.cross-platform

# Or use the traditional build method (Linux/macOS)
make

# Or build manually
mkdir -p obj
g++ -std=c++17 -Wall -I../SyntacticAnalyzer/include -I../LexicalAnalyzer/include \
    -c ../SyntacticAnalyzer/src/*.cpp ../LexicalAnalyzer/src/*.cpp test.cpp
g++ -std=c++17 -o syntaxer *.o
```

### Running the Compiler
```bash
# Run the compiler
./syntaxer

# When prompted, enter a filename from samples/ directory
# Example: test0.sy or example1_basic.sy
```

### Makefile Features
```bash
# Show all available targets
make -f Makefile.cross-platform help

# Build debug version
make -f Makefile.cross-platform debug

# Build optimized release version  
make -f Makefile.cross-platform release

# Run automated tests
make -f Makefile.cross-platform test

# Show project statistics
make -f Makefile.cross-platform stats

# Clean build artifacts
make -f Makefile.cross-platform clean

# Clean output files
make -f Makefile.cross-platform cleanout

# Complete cleanup
make -f Makefile.cross-platform distclean
```

### Sample Output
When you run the compiler on a C-- program, you'll get:
- **Tokens**: Lexical analysis results
- **Parse Tree**: Complete syntax tree
- **AST**: Abstract syntax tree
- **Analysis**: Parsing steps and logs

All outputs are saved in the `outputs/` directory with descriptive filenames.

### Troubleshooting
- Ensure you have C++17 support
- Check that all source files are present
- Use absolute paths if build fails
- Verify Make is installed and working

---

**Ready to showcase your compiler construction skills!** 🎯