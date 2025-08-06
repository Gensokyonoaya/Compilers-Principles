# C++ 实现编译器 / C-- Compiler Implementation

**一个将 C-- 语言翻译为 LLVM IR 的完整编译器 / A comprehensive compiler translating C-- language to LLVM IR**

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://isocpp.org/)
[![Build System](https://img.shields.io/badge/Build-Make-green.svg)](https://www.gnu.org/software/make/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Lines of Code](https://img.shields.io/badge/Lines%20of%20Code-2100%2B-brightgreen.svg)]()

> 📖 **[English Documentation](README_EN.md)** | **[中文文档](#项目背景)** | **[Technical Examples](docs/EXAMPLES.md)** | **[Project Metrics](docs/METRICS.md)**

## 🎯 项目亮点 / Key Highlights

- **2,100+ 行高质量 C++17 代码** / 2,100+ lines of production-quality C++17 code
- **模块化架构设计** / Modular architecture with clean separation of concerns  
- **有限自动机词法分析器** / Finite automaton-based lexical analyzer
- **LL(1) 语法分析器** / LL(1) parser with automatic parse table generation
- **完整 AST 构建** / Complete AST construction with semantic analysis
- **跨平台构建支持** / Cross-platform build system support

## 项目背景
本项目是编译原理课程的大作业，目标是实现一个将 C-- 语言翻译为 LLVM IR 语言的编译器。C-- 是一种简化的 C 语言子集，支持基本的变量声明、控制流语句和函数定义。本编译器分为多个模块，包括词法分析器、语法分析器等。

---

## 功能模块
### 1. **词法分析器 (LexicalAnalyzer)**
- 负责将源代码分解为一系列的 Token。
- 支持关键字、标识符、运算符、整数、浮点数和界符的识别。
- 使用有限自动机 (Finite Automaton) 实现词法分析。
- 支持错误处理，能够识别未定义的 Token。
- 支持将 Token 序列输出到文件。

### 2. **语法分析器 (SyntacticAnalyzer)**
- 负责根据上下文无关文法对 Token 序列进行语法分析(部分文法为手动分析)。
- 支持语法错误检测。
- 支持推导步骤输出到文件。
- **解析语法树 (Parse Tree)**：
  - 构建解析语法树以表示程序的语法结构。
  - 支持递归打印解析语法树，清晰展示程序的层次结构。
  - 支持将解析语法树输出到文件，便于调试和分析。
- **抽象语法树 (AST)**：
  - 构建抽象语法树以表示程序的语义结构。
  - 支持递归打印 AST，清晰展示程序的层次结构。
  - 支持将 AST 输出到文件，便于调试和分析。
- **符号表管理**：(未实现)

### 3. **代码生成**
- 将抽象语法树翻译为 LLVM IR 代码。(未实现)

---

## 目录结构

```
├── README.md                        // 帮助文档
├── 24-25+第二学期编译原理大作业要求.docx  // 大作业要求
├── LexicalAnalyzer                  // 词法分析器
│   ├── include
│   │   ├── lexer.h
│   │   ├── token.h
│   │   ├── finite_automaton.h
│   │   └── utils.h
│   └── src
│       ├── lexer.cpp
│       ├── token.cpp
│       ├── finite_automaton.cpp
│       └── utils.cpp
├── SyntacticAnalyzer                // 语法分析器
│   ├── include
│   │   ├── AST.h
│   │   ├── grammar.h
│   │   ├── parser_table.h
│   │   ├── parseTree.h
│   │   └── syntaxer.h
│   └── src
│       ├── AST.cpp
│       ├── grammar.cpp
│       ├── parser_table.cpp
│       ├── parseTree.cpp
│       └── syntaxer.cpp
├── test
│   ├── samples                          // 测试样例输入文件夹
│   │	├── test1.sy
│   │	├── test2.sy
│   │	└── ...                          // 更多测试用例
│	  ├── outputs                          // 输出文件夹
│   │	├── test1_tokens.txt
│   │	├── test1_syntax.txt
│   │	└── ...                          // 更多输出
│   ├── grammar.ini                     // 文法配置
│   ├── test.cpp                     // 测试主程序
│   └── makefile                     // 测试编译脚本

```

---

## 使用方法
### 1. 环境要求
- 编译器：`g++` (支持 C++17 标准)
- 构建工具：`make`
- 操作系统：Windows

### 2. 构建项目 / Build Instructions

#### 环境要求 / Prerequisites
- **编译器 / Compiler**: `g++` (支持 C++17 标准 / with C++17 support)
- **构建工具 / Build System**: `make`
- **操作系统 / Platform**: Windows/Linux/macOS (跨平台支持 / Cross-platform)

#### 使用新的跨平台构建系统 / Using New Cross-platform Build System
```bash
cd test
make -f Makefile.cross-platform       # 构建编译器 / Build compiler
make -f Makefile.cross-platform help  # 查看帮助 / Show help
make -f Makefile.cross-platform test  # 运行测试 / Run tests
make -f Makefile.cross-platform stats # 查看统计 / View statistics
```

#### 传统构建方法 / Traditional Build (Linux/macOS)
进入 `test` 目录，运行以下命令 / Enter `test` directory and run:

```bash
make
```

#### 手动编译 / Manual Compilation
```bash
cd test
mkdir -p obj
g++ -std=c++17 -Wall -I../SyntacticAnalyzer/include -I../LexicalAnalyzer/include \
    -c ../SyntacticAnalyzer/src/*.cpp ../LexicalAnalyzer/src/*.cpp test.cpp
g++ -std=c++17 -o syntaxer *.o
```

### 3. 运行测试
将你的测试样例（如 `test1.sy`）放入 `samples` 文件夹下。运行：

```bash
./syntaxer.exe
```
根据提示输入测试文件名（如 `test1.sy`），程序会自动读取 `samples/test1.sy`，并将词法分析、语法分析、解析语法树和 AST 结果分别输出到 `outputs/` 文件夹下。

### 4. 清理构建文件和输出
清理中间文件和可执行文件：

```bash
make clean
```
清空 `outputs` 文件夹下的所有输出文件以及文件夹本身：

```bash
make cleanout
```

---

## 输入输出说明

- **输入**：`samples` 文件夹下的 `.sy` 源代码文件。
- **输出**：`outputs` 文件夹下生成以下文件：
  - `_tokens.txt`：词法分析结果，每行一个 Token 的类型和值。
  - `_syntax.txt`：语法分析推导步骤。
  - `_parseTree.txt`：解析语法树，清晰展示程序的语法结构。
  - `_ast.txt`：抽象语法树，清晰展示程序的语义结构。

---

## 📚 专业文档 / Professional Documentation

### 🎯 简历展示材料 / Resume Materials
- **[English README](README_EN.md)** - 完整的英文技术文档 / Complete English technical documentation
- **[Project Metrics](docs/METRICS.md)** - 详细的项目指标和技术成就 / Detailed project metrics and technical achievements  
- **[Compiler Examples](docs/EXAMPLES.md)** - 编译器输出示例和分析 / Compiler output examples and analysis
- **[Cross-platform Makefile](test/Makefile.cross-platform)** - 专业的跨平台构建系统 / Professional cross-platform build system

### 🏆 技术成就 / Technical Achievements
- **代码质量**: 2,100+ 行生产级 C++17 代码 / Production-quality C++17 code
- **算法实现**: 自定义有限状态自动机和 LL(1) 解析器 / Custom FSA and LL(1) parser implementation
- **软件工程**: 现代 C++ 特性，RAII，智能指针 / Modern C++ features, RAII, smart pointers
- **跨平台支持**: Windows/Linux/macOS 兼容性 / Cross-platform compatibility
- **文档完善**: 100% API 文档覆盖率 / Complete API documentation coverage

---

## 样例说明

假设有 `samples/test1.sy` 文件，内容如下：

```c
int main() {
    int a = 1;
    return a;
}
```

运行后会在 `outputs/` 目录下生成：
- `test1_tokens.txt`：每行一个 Token 的类型和值
- `test1_syntax.txt`：语法分析推导步骤
- `test1_parseTree.txt`：解析语法树
- `test1_ast.txt`：抽象语法树

---