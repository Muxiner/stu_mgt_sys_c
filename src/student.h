/*
 * student.h — 学生信息管理系统 公共头文件
 *
 * 本文件声明了所有模块共享的数据结构、宏常量和函数接口。
 * 各功能模块通过包含本文件获得所需的类型和函数声明。
 */

#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <wchar.h>
#include <locale.h>

/* ============================================================
 * 字段长度与缓冲区大小
 * ============================================================ */
#define NAME_LEN      32     /* 姓名字段最大字节数 */
#define GENDER_LEN    8      /* 性别字段最大字节数 */
#define INPUT_BUF_LEN 256    /* 通用输入缓冲区大小   */

/* ============================================================
 * 业务数据取值范围
 * ============================================================ */
#define MIN_ID        1          /* 学号最小值 */
#define MAX_ID        1999999999 /* 学号最大值（支持10位长学号） */
#define MIN_AGE       0          /* 年龄最小值 */
#define MAX_AGE       150        /* 年龄最大值 */
#define MIN_SCORE     0.0        /* 成绩最小值 */
#define MAX_SCORE     100.0      /* 成绩最大值 */

/* ============================================================
 * 持久化文件名
 * ============================================================ */
#define DATA_FILE     "students.txt"  /* 正式数据文件   */
#define BACKUP_FILE   "students.bak"  /* 备份文件       */
#define TEMP_FILE     "students.tmp"  /* 写入时临时文件 */

/* ============================================================
 * 表格显示列宽（以终端列数为单位，已考虑 CJK 字符宽度）
 * ============================================================ */
#define COL_ID     10  /* 学号列宽 */
#define COL_NAME   12  /* 姓名列宽 */
#define COL_GENDER 8   /* 性别列宽 */
#define COL_AGE    8   /* 年龄列宽 */
#define COL_SCORE  10  /* 成绩列宽 */

/* ============================================================
 * 数据结构：单向链表结点 + 哈希链
 *
 * 每个结点存储一名学生的完整信息。
 * next 组成单向链表（用于遍历显示、排序、释放）。
 * hash_next 用于哈希表冲突链（提供 O(1) ID 查找）。
 * 链表不设最大容量上限，内存按需动态分配。
 * ============================================================ */
typedef struct Student {
    int    id;                  /* 学号（主键，唯一） */
    char   name[NAME_LEN];      /* 姓名               */
    char   gender[GENDER_LEN];  /* 性别（"男"/"女"） */
    int    age;                 /* 年龄               */
    float  score;               /* 成绩（保留两位小数）*/
    struct Student *next;       /* 链表后继           */
    struct Student *hash_next;  /* 哈希表冲突链       */
} Student;

/* ============================================================
 * 函数声明 — 按功能模块分组
 * ============================================================ */

/* ---- 显示辅助 (display.c) ---- */
int  str_display_width(const char *s);
void print_field(const char *s, int col_width, int left_align);

/* ---- 安全输入 (input.c) ---- */
void flush_stdin(void);
int  safe_get_string(const char *prompt, char *buf, size_t buf_size);
int  safe_get_string_allow_empty(const char *prompt, char *buf, size_t buf_size);
int  safe_get_int(const char *prompt, int min_val, int max_val, int *out);
int  safe_get_float(const char *prompt, float min_val, float max_val, float *out);

/* ---- 链表操作 (student.c) ---- */
Student* create_node(void);
int       add_student(Student **head);
void      display_all(const Student *head);
Student*  search_by_id(int id);
int       delete_student(Student **head);
int       modify_student(Student *head);
void      show_statistics(const Student *head);
void      free_list(Student **head);

/* ---- 文件持久化 (fileio.c) ---- */
int load_from_file(Student **head);
int save_to_file(const Student *head);

/* ---- 脏数据标记 (student.c) ---- */
void mark_data_dirty(void);
void mark_data_clean(void);
int  is_data_dirty(void);

/* ---- 哈希表 (hash.c) ---- */
void     hash_init(void);
void     hash_destroy(void);
Student* hash_find(int id);
void     hash_insert(Student *node);
void     hash_remove(int id);

/* ---- 菜单 (menu.c) ---- */
void print_menu(void);

#endif /* STUDENT_H */
