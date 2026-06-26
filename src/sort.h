/*
 * sort.h — 排序模块公共头文件
 *
 * 声明排序入口函数。各排序算法实现位于 sort.c，均为模块内静态函数。
 * 使用者只需包含本头文件并调用 sort_students 即可。
 */

#ifndef SORT_H
#define SORT_H

#include "student.h"   /* Student 类型、INPUT_BUF_LEN、safe_get_* 等 */

/* 排序入口：交互式选择算法、字段、升降序，执行排序并显示结果和耗时 */
void sort_students(Student **head);

#endif /* SORT_H */
