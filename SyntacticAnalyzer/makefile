# 编译器
CXX := g++

# 编译选项
CXXFLAGS := -std=c++17 -Wall -Iinclude

# 目标文件夹
SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj

# 源文件和目标文件
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# 最终生成的可执行文件
TARGET := syntaxer.exe

# 默认目标
all: $(TARGET)

# 编译源文件生成目标文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 清理生成的文件
clean:
	@if exist $(OBJ_DIR) RD /S /Q $(OBJ_DIR)
	@if exist $(TARGET) del $(TARGET)
