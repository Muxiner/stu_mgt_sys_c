/*
 * display.c — 终端显示辅助模块
 *
 * 解决 C 语言 printf 在面对 UTF-8 多字节字符（如中文）时
 * 列对齐失效的问题。通过计算字符的实际终端显示宽度，
 * 实现中英文混合场景下的表格对齐输出。
 */

#include "student.h"

/*
 * 计算字符串在终端中的显示列宽。
 *
 * UTF-8 编码下，ASCII 字符占 1 列，CJK 字符占 2 列。
 * 优先使用 POSIX 标准库函数 mbstowcs + wcswidth 获取精确宽度；
 * 若转换失败（如 locale 不支持），回退到字节级估算：
 *   单字节字符（< 0x80）→ 1 列
 *   多字节字符（≥ 0x80）→ 2 列
 */
int str_display_width(const char *s) {
    /* 仅首次调用时设置 locale，避免重复开销 */
    static int locale_set = 0;
    if (!locale_set) {
        setlocale(LC_ALL, "");
        locale_set = 1;
    }

    wchar_t wbuf[256];
    size_t n = mbstowcs(wbuf, s, sizeof(wbuf) / sizeof(wchar_t));

    if (n == (size_t)-1) {
        /* 宽字符转换失败，使用字节级估算 */
        int w = 0;
        for (const char *p = s; *p; p++) {
            if ((unsigned char)*p < 0x80) {
                w++;
            } else {
                w += 2;
                /* 跳过 UTF-8 续字节 (0x80~0xBF)，用 p[1] 预查防越界 */
                while ((unsigned char)p[1] >= 0x80 && (unsigned char)p[1] < 0xC0)
                    p++;
            }
        }
        return w;
    }

    return wcswidth(wbuf, n);
}

/*
 * 按照显示列宽对齐打印字符串字段。
 *
 * 参数：
 *   s          要打印的字符串
 *   col_width  目标列宽（终端列数）
 *   left_align 1 = 左对齐，0 = 右对齐
 *
 * 若字符串显示宽度超过 col_width，不做截断（保持信息完整）。
 */
void print_field(const char *s, int col_width, int left_align) {
    int dw = str_display_width(s);
    int pad = col_width - dw;
    if (pad < 0) pad = 0;   /* 内容超宽时不截断 */

    if (left_align) {
        /* 左对齐：内容 + 填充空格 */
        printf("%s%*s", s, pad, "");
    } else {
        /* 右对齐：填充空格 + 内容 */
        printf("%*s%s", pad, "", s);
    }
}
