/*
 * main.c — 程序入口与主控流程
 *
 * 负责：解析命令行参数（预留），加载数据，进入主菜单循环，
 *       调度各功能模块，并在退出前执行保存与内存释放。
 */

#include "student.h"

int main(void) {
    Student *head = NULL;   /* 链表头指针，是整个系统的数据根 */
    int choice;             /* 用户选择的菜单编号 */

    /* 启动提示 */
    printf("+----------------------------------------+\n");
    printf("|       学生信息管理系统 启动中...        |\n");
    printf("+----------------------------------------+\n");

    /* 启动时自动从文件加载已有数据 */
    if (load_from_file(&head) != 0) {
        printf("[!] 数据加载异常，将以空数据库启动。\n");
    }

    /*
     * 主循环：显示菜单 → 获取选项 → 调度对应功能模块
     * 输入 0 时退出循环
     */
    do {
        print_menu();

        /* 菜单选项限定在 0~8 */
        if (safe_get_int("请选择操作: ", 0, 8, &choice) != 0) {
            printf("[!] 输入错误，请重试。\n");
            continue;
        }

        switch (choice) {
        case 1:
            add_student(&head);       /* 添加：需传二级指针以修改 head */
            break;
        case 2:
            display_all(head);        /* 显示全部：只读，传一级指针 */
            break;
        case 3: {
            /* 按学号查找：显示单条记录 */
            int id;
            if (safe_get_int("请输入要查找的学号: ", MIN_ID, MAX_ID, &id) != 0) break;
            Student *s = search_by_id(head, id);
            if (s) {
                printf("\n");
                print_field("学号", COL_ID, 0); printf(" ");
                print_field("姓名", COL_NAME, 1); printf(" ");
                print_field("性别", COL_GENDER, 1); printf(" ");
                print_field("年龄", COL_AGE, 0); printf(" ");
                print_field("成绩", COL_SCORE, 0); printf("\n");

                int total = COL_ID + 1 + COL_NAME + 1 + COL_GENDER
                          + 1 + COL_AGE + 1 + COL_SCORE;
                for (int i = 0; i < total; i++) putchar('-');
                putchar('\n');

                char buf[64];
                snprintf(buf, sizeof(buf), "%d", s->id);
                print_field(buf, COL_ID, 0); printf(" ");
                print_field(s->name, COL_NAME, 1); printf(" ");
                print_field(s->gender, COL_GENDER, 1); printf(" ");
                snprintf(buf, sizeof(buf), "%d", s->age);
                print_field(buf, COL_AGE, 0); printf(" ");
                snprintf(buf, sizeof(buf), "%.2f", s->score);
                print_field(buf, COL_SCORE, 0); printf("\n");
            } else {
                printf("[!] 未找到学号为 %d 的学生。\n", id);
            }
            break;
        }
        case 4:
            delete_student(&head);
            break;
        case 5:
            modify_student(head);
            break;
        case 6:
            sort_students(&head);
            break;
        case 7:
            show_statistics(head);
            break;
        case 8:
            if (save_to_file(head) == 0) {
                printf("[OK] 数据已手动保存。\n");
            }
            break;
        case 0:
            printf("正在退出...\n");
            break;
        default:
            break;
        }
    } while (choice != 0);

    /* 退出前保存数据，保证持久化 */
    if (save_to_file(head) != 0) {
        printf("[!] 退出前保存失败，部分数据可能丢失。\n");
    }

    /* 释放链表中所有结点，并将 head 置 NULL */
    free_list(&head);

    printf("系统已安全退出。\n");
    return 0;
}
