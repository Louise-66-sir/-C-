# 408 数据结构 C 源码与自测

配套仓库根目录的 `ds408-notes.ipynb`、`notes.md` 与 `index.html` 阅读版。只阅读笔记不需要编译器。

代码采用 Visual Studio Community 常见格式：4 空格缩进、大括号另起一行、每条语句独立成行。目录内附 .clang-format，可用于保持同样的排版。

## 文件

| 文件 | 内容 |
| --- | --- |
| common.h | 标准头文件、容量、INF、内存申请与交换辅助函数 |
| 01_linear.c | 顺序表、链表、快慢指针、主元素、双链表 |
| 02_stack_queue.c | 顺序/共享/链栈、循环/链队列、括号、后缀求值、数组 |
| 03_string.c | BF、前缀函数 pi 版 KMP、nextval 版、重叠计数 |
| 04_tree.c | 遍历、树应用、重建、线索树、森林、Huffman |
| 05_graph.c | 图存储、遍历、并查集、MST、最短路、拓扑与关键时间 |
| 06_search.c | 顺序/折半/分块、BST、AVL插删、B/B+查找、散列 |
| 07_sort.c | 内部排序、快速选择、逆序对、k路归并内存模拟 |
| 08_exercises.c | 12 道原创综合练习的答案实现 |
| 验证说明.md | 实际测试环境、检查结果和范围 |

## 可选运行方式

把 common.h 与所有 .c 文件放在同一文件夹。**每个 .c 自带 main，需分别编译**，不要把多个文件一起链接成同一个程序。

GCC：

```text
gcc -std=c11 -O2 01_linear.c -o linear
```

macOS/Linux 执行 `./linear`；Windows 执行 `linear.exe`（若编译器生成该名字）。也可使用 Clang 将 gcc 换成 clang。

Windows 的 Visual Studio 开发者终端：

```text
cl /std:c11 /utf-8 /W4 /D_CRT_SECURE_NO_WARNINGS 01_linear.c
01_linear.exe
```

将文件名换成其他章节即可运行该章。正常通过会打印对应的 `PASS`。main 中包含断言自测，因此**运行自测时不要定义 NDEBUG**。改写练习时建议新建自己的 main 或备份原测试入口。

## 约定

全部算法使用 C11。数组/串/图顶点从 0 编号；默认链表带头结点；空树高度 0。具体函数前提、容量、输出空间、输入所有权与整数范围请看注释和 Notebook 的 00.2。静态教学容量 CAP=256、MAXV=64，可自行调整并同步核对局部数组。

B/B+ 树仅包含查找和手工构造示例，红黑树等复杂维护规则在笔记中讲解，没有伪装成完整插删库。07_sort 的外部排序部分是内存归并模拟，不读取磁盘文件。
