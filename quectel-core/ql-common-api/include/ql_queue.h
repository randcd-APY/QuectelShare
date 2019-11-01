/*
** EPITECH PROJECT, 2019
** quecLocator
** File description:
** queue
*/

#ifndef QUEUE_H_
    #define QUEUE_H_

typedef struct mynode{
       struct mynode* next;
       void *elem;
}ql_node;

typedef struct{
   ql_node* item;
   unsigned int elem_size;   /*元素大小*/
   unsigned int length;  /*队列长度*/
   void (*freefn)(void *); /*自定义内存释放函数，释放外部动态申请的内存*/
}ql_queue;

/*初始化队列*/
void ql_queue_init(ql_queue *s, unsigned int size, void (*freefn)(void*));

/*判断队列为空*/
int ql_queue_empty(ql_queue *q);

/*求队列长度*/
int ql_queue_size(ql_queue *q);

/*获得队列头部元素*/
void* ql_queue_front(ql_queue *q);

/*获得队列尾部元素*/
void* ql_queue_back(ql_queue *q);

/*弹出（删除）队列头部元素*/
void ql_queue_pop(ql_queue *q);

/*元素入队列尾部*/
void ql_queue_push(ql_queue *q,void *value);

/*删除队列*/
void ql_queue_destory(ql_queue *q);

#endif /* !QUEUE_H_ */