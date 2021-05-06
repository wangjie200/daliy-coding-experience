/*
 * @author Elio Yang
 * @email  jluelioyang2001@gamil.com
 * @date 2021/5/5
 */


#ifndef CDESIGN_FINANCIAL_H
#define CDESIGN_FINANCIAL_H
#include <stdint.h>
typedef enum {
        ORDER,
        INPUT,
        HR
}DealType;

typedef struct fi {
        char name[64];
        char price[32];
        char time[32];
        DealType type;
        struct fi* next;
}Fi_node;

typedef struct fi_info{
        int fd;
        Fi_node *head;
        uint32_t cnt;
        int dirty;
}F_list;
#define FINANCE_FILE "financial.txt"

extern void init_fi_sys();
extern void update_finance_record(const char *name,const char *price,DealType type);
extern void print_financial_recd();
extern void query();


#endif //CDESIGN_FINANCIAL_H
