/*
 * input.c — 安全输入模块
 *
 * 所有交互式输入均经过本模块处理，以防御缓冲区溢出、非法字符注入、
 * 输入残留污染等常见 C 语言安全漏洞。
 *
 * 设计原则：
 *   1. 使用 fgets 替换原生 scanf，严格控制缓冲区长度
 *   2. 使用 strtol / strtof 进行严格的数值类型校验
 *   3. 所有数值均校验业务范围（硬上限/下限）
 *   4. 输入过长时自动清空 stdin 残留，避免污染后续输入
 */

#include "student.h"

/*
 * 清空标准输入缓冲区中的残留字符。
 * 当 fgets 读取的字符串末尾没有换行符时，
 * 说明缓冲区不足以容纳整行输入，需调用此函数丢弃剩余内容。
 */
void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/*
 * 安全读取一行非空字符串。
 * - 自动去除首尾空白字符
 * - 拒绝纯空白输入
 * - 输入超过 buf_size 时自动截断并清空 stdin
 *
 * 参数：
 *   prompt   提示字符串
 *   buf      输出缓冲区
 *   buf_size 缓冲区大小（含结尾 '\0'）
 *
 * 返回 0 成功，-1 遇到 EOF 或读取错误。
 */
int safe_get_string(const char *prompt, char *buf, size_t buf_size) {
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, (int)buf_size, stdin)) return -1;

        size_t len = strlen(buf);
        /* 末尾有换行说明输入未超过缓冲区，正常去除 */
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
            len--;
        } else {
            /* 输入超长，清空 stdin 中剩余字符 */
            flush_stdin();
        }

        if (len == 0) {
            printf("[!] 输入不能为空，请重新输入。\n");
            continue;
        }

        /* 去除首尾空白字符 */
        char *start = buf;
        while (*start && isspace((unsigned char)*start)) start++;
        if (start > buf) {
            memmove(buf, start, strlen(start) + 1);
            len = strlen(buf);
        }
        while (len > 0 && isspace((unsigned char)buf[len - 1])) {
            buf[--len] = '\0';
        }

        if (len == 0) {
            printf("[!] 输入不能为纯空白，请重新输入。\n");
            continue;
        }
        return 0;
    }
}

/*
 * 安全读取一行字符串（允许空输入）。
 * 用于修改场景：用户直接按回车表示"不修改该字段"。
 *
 * 其余行为与 safe_get_string 相同。
 */
int safe_get_string_allow_empty(const char *prompt, char *buf, size_t buf_size) {
    printf("%s", prompt);
    if (!fgets(buf, (int)buf_size, stdin)) return -1;

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        len--;
    } else {
        flush_stdin();
    }

    char *start = buf;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start > buf) {
        memmove(buf, start, strlen(start) + 1);
        len = strlen(buf);
    }
    while (len > 0 && isspace((unsigned char)buf[len - 1])) {
        buf[--len] = '\0';
    }

    /* 纯空白等价于空输入 */
    if (len == 0) {
        buf[0] = '\0';
    }
    return 0;
}

/*
 * 安全读取一个整数。
 * 使用 strtol 进行严格类型解析，拒绝任何非数字字符。
 * 同时校验 errno 溢出和业务范围 [min_val, max_val]。
 *
 * 返回 0 成功，将结果写入 *out；-1 失败。
 */
int safe_get_int(const char *prompt, int min_val, int max_val, int *out) {
    char buf[INPUT_BUF_LEN];
    long val;
    char *endptr;    /* 指向第一个未解析字符 */

    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) return -1;

        /* 检测并处理超长输入 */
        if (buf[strlen(buf) - 1] != '\n') flush_stdin();

        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';

        if (buf[0] == '\0') {
            printf("[!] 输入不能为空。\n");
            continue;
        }

        errno = 0;
        val = strtol(buf, &endptr, 10);

        /* 检查溢出 */
        if (errno == ERANGE || val < INT_MIN || val > INT_MAX) {
            printf("[!] 数值超出范围。\n");
            continue;
        }
        /* 检查是否完全解析（endptr 指向 '\0' 才算成功） */
        if (endptr == buf || *endptr != '\0') {
            printf("[!] 输入包含非数字字符，请输入整数。\n");
            continue;
        }
        /* 业务范围校验 */
        if (val < min_val || val > max_val) {
            printf("[!] 数值必须在 %d ~ %d 之间。\n", min_val, max_val);
            continue;
        }

        *out = (int)val;
        return 0;
    }
}

/*
 * 安全读取一个浮点数。
 * 使用 strtof 进行严格类型解析，支持小数点和科学计数法。
 * 同时校验 errno 溢出和业务范围 [min_val, max_val]。
 *
 * 返回 0 成功，将结果写入 *out；-1 失败。
 */
int safe_get_float(const char *prompt, float min_val, float max_val, float *out) {
    char buf[INPUT_BUF_LEN];
    float val;
    char *endptr;

    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) return -1;

        if (buf[strlen(buf) - 1] != '\n') flush_stdin();

        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';

        if (buf[0] == '\0') {
            printf("[!] 输入不能为空。\n");
            continue;
        }

        errno = 0;
        val = strtof(buf, &endptr);

        if (errno == ERANGE) {
            printf("[!] 数值超出范围。\n");
            continue;
        }
        if (endptr == buf || *endptr != '\0') {
            printf("[!] 输入包含非数字字符，请输入数字。\n");
            continue;
        }
        if (val < min_val || val > max_val) {
            printf("[!] 数值必须在 %.2f ~ %.2f 之间。\n", min_val, max_val);
            continue;
        }

        *out = val;
        return 0;
    }
}
