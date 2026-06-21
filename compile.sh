#!/bin/bash

# ============================================================
# 编译脚本 —— 自动检测编译器，优先使用 CMake
# ============================================================

set -euo pipefail

SRC="src/main.c src/menu.c src/student.c src/input.c src/fileio.c src/display.c"
TARGET="studentms"
BUILD_DIR="build"

echo "========================================"
echo "  学生信息管理系统 —— 编译脚本"
echo "========================================"

# ---- 检测 CMake ----
if command -v cmake &>/dev/null; then
    echo "[INFO] 检测到 CMake: $(cmake --version 2>/dev/null | head -1)"

    mkdir -p "$BUILD_DIR" && cd "$BUILD_DIR"

    if cmake .. && make; then
        cp -f "$TARGET" "../$TARGET" 2>/dev/null || cp -f "$TARGET"* "../$TARGET" 2>/dev/null || true
        cd ..
        echo ""
        echo "[OK] CMake 编译成功 -> ./$TARGET"
        exit 0
    else
        cd ..
        echo "[WARN] CMake 编译失败，回退到直接编译..."
    fi
fi

# ---- 检测编译器（无 CMake 时） ----
CC_BIN=""
CFLAGS="-Wall -Wextra -std=c99 -O2"

if command -v gcc &>/dev/null; then
    CC_BIN="gcc"
    echo "[INFO] 检测到编译器: GCC ($($CC_BIN --version | head -1))"
elif command -v clang &>/dev/null; then
    CC_BIN="clang"
    echo "[INFO] 检测到编译器: Clang ($($CC_BIN --version | head -1))"
elif command -v cc &>/dev/null; then
    CC_BIN="cc"
    echo "[INFO] 检测到编译器: cc"
else
    echo "[ERR] 未检测到可用编译器 (gcc/clang/cc)，请先安装编译器。"
    echo "      macOS:  xcode-select --install"
    echo "      Linux:  sudo apt install gcc    /  sudo yum install gcc"
    echo "      Windows: 安装 MinGW 或 Visual Studio"
    exit 1
fi

# ---- 编译 ----
echo "[INFO] 编译命令: $CC_BIN $CFLAGS -o $TARGET $SRC"
if $CC_BIN $CFLAGS -o "$TARGET" "$SRC"; then
    echo ""
    echo "[OK] 编译成功 -> ./$TARGET"
else
    echo "[ERR] 编译失败。"
    exit 1
fi
