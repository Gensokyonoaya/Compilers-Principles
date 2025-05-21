# C++实现一个编译器

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
- **符号表管理**：(暂未支持)
  - 支持嵌套作用域符号表的构建和管理。
  - 提供符号的插入、查询、更新和删除功能。
  - 支持变量和函数的声明与定义。

### 3. **代码生成**
- 将抽象语法树翻译为 LLVM IR 代码。(暂未实现)

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
│   │   ├── symbol_table.h
│   │   ├── grammar.h
│   │   ├── parser_table.h
│   │   ├── parseTree.h
│   │   └── syntaxer.h
│   ├── src
│   │   ├── AST.cpp
│   │   ├── symbol_table.cpp
│   │   ├── grammar.cpp
│   │   ├── parser_table.cpp
│   │   ├── parseTree.cpp
│   │   └── syntaxer.cpp
│   └── makefile                     // 编译脚本
├── test
│   ├── samples                          // 测试样例输入文件夹
│   │	├── test1.sy
│   │	├── test2.sy
│   │	└── ...                          // 更多测试用例
│	├── outputs                          // 输出文件夹
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

### 2. 构建项目
进入 `test` 目录，运行以下命令：

```bash
make
```
编译成功后会生成可执行文件 `syntaxer.exe`。

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
- 

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