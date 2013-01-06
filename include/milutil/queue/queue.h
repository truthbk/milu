#ifndef _MILU_QUEUE_H
#define _MILU_QUEUE_H

#include <pthread.h>

#include "list/list.h"

struct queue {
    struct list_head _list;
    pthread_mutex_t _mutex;

}

static inline void INIT_QUEUE(struct queue * q) {
    q->_list = LIST_HEAD_INIT(q->_list);
    q->_mutex = PTHREAD_MUTEX_INITIALIZER;
}

static inline int empty_queue(struct queue * q) {
    return list_empty(&q->_q);
}

static inline list_head * get_fifo(struct queue * q) {
    list_head * elem = NULL;

    pthread_mutex_lock(&q->_mutex);
    if(empty_queue(q)) {
        pthread_mutex_unlock(&q->_mutex);
        return NULL;
    }

    elem = q->_list.next;
    list_del(elem);
    pthread_mutex_unlock(&q->_mutex);

    return elem;
}

static inline void put_fifo(struct queue * q, list_head * e) {

    pthread_mutex_lock(&q->_mutex);

    list_add_tail(e, &q->_list);
    pthread_mutex_unlock(&q->_mutex);

    return elem;
}

#endif
