#! /bin/bash

echo "请选择编译环境"
echo "1. Linux"
echo "2. Windows"
echo "3. exit"

read -p "请输入选项(1/2/3):" choice

case $choice in
    1)
        echo "开始编译 Linux Snake Game"
        gcc -o SnakeGame.out main.c
        if [ $? -eq 0 ]; then
            cp SnakeGame.out ~/桌面
            echo "编译完成，文件已复制到桌面"
        else
            echo "编译失败，请检查代码！"
        fi
        find "$(pwd)" -name SnakeGame.out
        exit 0
        ;;
    2)
        echo "开始编译 Windows Snake Game"
        x86_64-w64-mingw32-gcc -o SnakeGame.exe main.c
        if [ $? -eq 0 ]; then
            echo "编译完成，文件路径为: $(pwd)/SnakeGame.exe"
        else
            echo "编译失败，请检查代码！"
        fi
        exit 0
        ;;
    3)
        echo "程序退出"
        exit 0
        ;;
    *)
        echo "无效的选项，请输入 1、2 或 3。"
        ;;
esac
