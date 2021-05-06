/*
 * @author Elio Yang
 * @email  jluelioyang2001@gamil.com
 * @date 2021/5/5
 */

#include "financial.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tools/time_related.h"
#include "../tools/quick_sort.h"

F_list *FI_LIST;
#define FI_INFO_LEN 71
//进来的时候初始化系统（在点单的模块里面）
void init_fi_sys()
{
        int fd = open(FINANCE_FILE, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
        if (fd == -1)
        {
                fprintf(stderr, "File %s open failed.\n", FINANCE_FILE);
                exit(EXIT_FAILURE);
        }
        FI_LIST = (F_list *)malloc(sizeof(*FI_LIST));
        FI_LIST->fd = fd;
        FI_LIST->cnt = 0;
        FI_LIST->dirty = 0;
        FI_LIST->head = (Fi_node *)malloc(sizeof(Fi_node));

        char info[FI_INFO_LEN];
        Fi_node *ptr = FI_LIST->head;
        ptr->next = NULL;
        while (read(fd, info, FI_INFO_LEN))
        {
                DealType t = -1;
                info[FI_INFO_LEN - 1] = '\0';

                Fi_node *node = (Fi_node *)malloc(sizeof(*node));
                char *name = strtok(info, " ");
                char *price = strtok(NULL, " ");
                char *type = strtok(NULL, " ");
                if (strcmp(type, "0") == 0)
                {
                        t = ORDER;
                }
                else if (strcmp(type, "1") == 0)
                {
                        t = INPUT;
                }
                else if (strcmp(type, "2") == 0)
                {
                        t = HR;
                }
                char *time = strtok(NULL, " ");

                strcpy(node->name, name);
                strcpy(node->price, price);
                node->type = t;
                strcpy(node->time, time);
                node->next = NULL;
                Fi_node *next = ptr->next;
                ptr->next = node;
                node->next = next;
                ptr = FI_LIST->head;
                ++FI_LIST->cnt;
        }
}

//加入一条信息
void update_finance_record(const char *name, const char *price, DealType type)
{
        Fi_node *node = (Fi_node *)malloc(sizeof(*node));

        char buf[32];
        time_t this_time = time(NULL);
        struct tm *t = localtime(&this_time);
        strftime(buf, 32, "%Y%m%d%H%M%S", t);

        const char *time = buf;
        strcpy(node->time, time);
        strcpy(node->name, name);
        strcpy(node->price, price);
        node->type = type;

        Fi_node *head = FI_LIST->head;
        node->next = head->next;
        head->next = node;
        ++FI_LIST->cnt;
        FI_LIST->dirty = 1;
}

//退出的时候输出到文件
void flush_fi_disk()
{
        if (FI_LIST->dirty == 0)
        {
                return;
        }
        close(FI_LIST->fd);
        FI_LIST->fd = -1;
        int fd;
        char cmd[100];
        int l = 0;
        char *filename = FINANCE_FILE;
        l += sprintf(cmd + l, "%s ", "rm");
        l += sprintf(cmd + l, "%s", filename);
        system(cmd);

        fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
        if (fd == -1)
        {
                fprintf(stderr, "File %s open failed, human resource system init failed\n", filename);
                exit(EXIT_FAILURE);
        }
        Fi_node *node = FI_LIST->head->next;
        while (node != NULL)
        {
                int len = 0;
                char info[FI_INFO_LEN];
                char *name = node->name;
                char *price = node->price;
                int t = (int)node->type;
                char *time = node->time;

                len += sprintf(info + len, "%s ", name);
                len += sprintf(info + len, "%s ", price);
                len += sprintf(info + len, "%d ", t);
                len += sprintf(info + len, "%s ", time);

                if (strlen(info) < (FI_INFO_LEN - 1))
                {
                        int diff = FI_INFO_LEN - 1 - strlen(info);
                        char buf[diff + 1];
                        for (int i = 0; i < diff; i++)
                        {
                                buf[i] = ' ';
                        }
                        buf[diff] = '\0';
                        len += sprintf(info + len, "%s", buf);
                        info[FI_INFO_LEN - 1] = '\n';
                }
                write(fd, info, FI_INFO_LEN);
                node = node->next;
        }
}

// FI_LIST->head->next 是第一个节点（有正常数据的）
void print_financial_recd()
{
        Fi_node *p;
        for (int i = 0; i < 3; i++)
        {
                p = FI_LIST->head->next;
                if (i == 0)
                        printf("***************ORDER_FINACANIAL_RECORD****************\n");
                if (i == 1)
                        printf("*************WAREHOUSE_FINACANIAL_RECORD**************\n");
                if (i == 2)
                        printf("****************HR_FINACANIAL_RECORD******************\n");

                while (p != NULL)
                {
                        if (p->type == i)
                                printf("*%-20s%-35s%-7s*\n", p->time, p->name, p->price);
                        p = p->next;
                }
        }
}
//打印某段日期
void query_from(const char *id1, const char *id2)
{
}

//时间作为订单号 用时间查询 时间是这个格式 20210404220834  年月日时分秒
static int cmp_up_name(const void *A, const void *B) {
        return strcmp(((Fi_node *) A)->name, ((Fi_node *) B)->name);
}

static int cmp_d_name(const void *A, const void *B) {
        return strcmp(((Fi_node *) B)->name, ((Fi_node *) A)->name);
}

static int cmp_up_time(const void *A, const void *B) {
        return strcmp(((Fi_node *) A)->time, ((Fi_node *) B)->time);
}

static int cmp_d_time(const void *A, const void *B) {
        return strcmp(((Fi_node *) B)->time, ((Fi_node *) A)->time);
}

static int cmp_up_price(const void *A, const void *B) {
        return strcmp(((Fi_node *) A)->price, ((Fi_node *) B)->price);
}

static int cmp_d_price(const void *A, const void *B) {
        return strcmp(((Fi_node *) B)->price, ((Fi_node *) A)->price);
}

static int (*cmp_table[][2])(const void *, const void *) ={
        {cmp_up_name, cmp_d_name},
        {cmp_up_time,  cmp_d_time},
        {cmp_up_price,  cmp_d_price},
};
void sort_data_by(DealType type, int choice,int direction)
{
        if (type == 0)
                printf("***************ORDER_FINACANIAL_RECORD****************\n");
        else if (type == 1)
                printf("*************WAREHOUSE_FINACANIAL_RECORD**************\n");
        else if (type == 2)
                printf("****************HR_FINACANIAL_RECORD******************\n");

        if (choice == 0)
                printf("(Sorted by name)\n");
        else if (type == 1)
                printf("(Sorted by order number)\n");
        else if (type == 2)
                printf("(Sorted by price)\n");

        Fi_node record[1000], *p = FI_LIST->head->next;
        int num = 0;
        while (p != NULL)
        {
                if (p->type == type)
                        record[num++] = *p;
                p = p->next;
        }
        int (*cmp)(const void*, void*)=cmp_table[choice][direction];
        sort(record, num, sizeof(Fi_node), cmp);
        for (int i = 0; i < num; i++)
                printf("*%-20s%-35s%-7s*\n", record[i].time, record[i].name, record[i].price);
        //先搞一个Fi_node的数组，把链表输入到数组里
        //按哪个字段排序（choice: 0是名字 1是订单号 2是价格
        // 写三个cmp函数(qsort的那种）
        //直接调用sort() 排序这个在tools/quick_sort.h 里面 用法跟 qsort一致
        // 不太清楚可以看 human_resource_system/hr_sys.c 629行 函数 void sort_by(Field field, int direction)的方法
}
