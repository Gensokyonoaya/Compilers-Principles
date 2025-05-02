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

### 2. **语法分析器 (SyntacticAnalyzer)**
- 负责根据上下文无关文法对 Token 序列进行语法分析。
- 构建抽象语法树 (AST)。
- 支持语法错误检测。
- 符号表管理
  - 支持嵌套作用域符号表的构建和管理。
  - 提供符号的插入、查询、更新和删除功能。
  - 支持变量和函数的声明与定义。

### 3. **代码生成**
- 将抽象语法树翻译为 LLVM IR 代码。
- 支持基本的算术运算、条件分支和循环结构。

---

## 目录结构描述
    ├── ReadMe.md           // 帮助文档
    
    ├── 24-25+第二学期编译原理大作业要求.docx    // 大作业要求
    
    ├── LexicalAnalyzer             // 词法分析器
    
    │	├── include     // 词法分析器include文件夹

	│		├── lexer.h // 词法分析器头文件

	│		├── token.h // Token 定义头文件

	│		├── finite_automaton.h // 有限自动机头文件

	│		└── utils.h // 工具函数头文件

    │	├── src     // 词法分析器src文件夹

	│		├── lexer.cpp // 词法分析器实现

	│		├── token.cpp // Token 实现

	│		├── finite_automaton.cpp // 有限自动机实现

	│		├── utils.cpp // 工具函数实现

	│		└── test.cpp // 测试用例

	│	└── makefile	// 构建脚本

    ├── SyntacticAnalyzer             // 语法分析器

	│   ├── include     // 语法分析器include文件夹

	│		├── symbol_table.h // 符号表头文件

	│		├── utils.h // 工具函数头文件

	│   ├── src     // 语法分析器src文件夹
    
	│		├── symbol_table.cpp // 符号表实现 

	│		├── utils.cpp // 工具函数实现

    └──                 // 

---

## 使用方法
### 1. **环境要求**
- 编译器：`g++` (支持 C++17 标准)
- 构建工具：`make`
- 操作系统：Windows 或 Linux

### 2. **构建项目**
在 `LexicalAnalyzer` 目录下运行以下命令：
```bash
make
```
成功后会生成可执行文件 lexer.exe。
### 3. **运行词法分析器**
运行以下命令：
```bash
./lexer.exe
```
词法分析器会读取 test.c 文件中的代码，并输出生成的 Token 序列。

4. 清理构建文件
运行以下命令清理生成的中间文件和可执行文件：
```bash
make clean
```
---
