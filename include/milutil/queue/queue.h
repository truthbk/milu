#ifndef _MILU_QUEUE_H
#define _MILU_QUEUE_H

#include <pthread.h>

#include "list/list.h"

struct queue {
    struct list_head _list;
    pthread_mutex_t _mutex;

};

static inline void INIT_QUEUE(struct queue * q) {
    INIT_LIST_HEAD(&q->_list);
    pthread_mutex_init(&q->_mutex, NULL);
};

static inline int empty_queue(struct queue * q) {
    return list_empty(&q->_list);
};

static inline struct list_head * get_fifo(struct queue * q) {
    struct list_head * elem = NULL;

    pthread_mutex_lock(&q->_mutex);
    if(empty_queue(q)) {
        pthread_mutex_unlock(&q->_mutex);
        return NULL;
    }

    elem = q->_list.next;
    list_del(elem);
    pthread_mutex_unlock(&q->_mutex);

    return elem;
};

static inline void put_fifo(struct queue * q, struct list_head * e) {

    pthread_mutex_lock(&q->_mutex);

    list_add_tail(e, &q->_list);
    pthread_mutex_unlock(&q->_mutex);

    return;
};

#endif
