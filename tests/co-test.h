//===============================================================
// COPY FROM: https://git.nju.edu.cn/jyy/os-workbench/-/raw/M2/libco/tests/co-test.h
//===============================================================

/* 
 * Here by copyright, credits attributed to wherever they belong.
 * jyy (nju.edu.cn)
 */

#include <co.h>
#include <list.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Queue_t {
    struct list_head list;
    int sz;
    int cap;
} Queue;

typedef struct Item_t {
    void *data;
    struct list_head link;
} Item;

static inline Queue* q_new() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) {
        fprintf(stderr, "New queue failure\n");
        exit(1);
    }
    queue->cap = 100;
    queue->sz = 0;
    INIT_LIST_HEAD(&queue->list);
    return queue;
}

static inline void q_free(Queue *queue) {
    Item *pos, *next;
    list_for_each_entry_safe(pos, next, &queue->list, link) {
        list_del(&pos->link);
        free(pos);
    }
    free(queue);
}

static inline int q_is_full(Queue *queue) {
    return queue->sz == queue->cap;
}

static inline int q_is_empty(Queue *queue) {
    return list_empty(&queue->list);
}

static inline void q_push(Queue *queue, Item *item) {
    if (q_is_full(queue)) {
        fprintf(stderr, "Push queue failure\n");
        return;
    }
    list_add_tail(&item->link, &queue->list);
    queue->sz += 1;
}

static inline Item* q_pop(Queue *queue) {
    if (q_is_empty(queue)) {
        return NULL;
    }

    Item *item = list_entry(queue->list.next, Item, link);
    list_del(&item->link);

    queue->sz -= 1;
    return item;
}
