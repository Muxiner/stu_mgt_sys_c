/*
 * hash.h — 学号哈希表
 *
 * 为 Student 链表提供 O(1) 平均时间的 ID 查找。
 * 采用链地址法，冲突结点通过 Student->hash_next 串联。
 * 表容量随数据量自动扩容，保持低负载因子。
 */
#ifndef HASH_H
#define HASH_H

#include "student.h"

void     hash_init(void);
void     hash_destroy(void);
Student* hash_find(int id);
void     hash_insert(Student *node);
void     hash_remove(int id);

#endif /* HASH_H */
