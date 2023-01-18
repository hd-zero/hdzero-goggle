#ifndef _ARAN_QUEUE_H_
#define _ARAN_QUEUE_H_

#include "dd_dlist.h"

typedef Llist Queue;

// 创建一个队列，返回Queue指针
// @data_size: 指队列中元素所占的空间，如 sizeof(struct xxx_st)
Queue *queue_create(int data_size);

// 销毁一个队列（和free()一样，p_queue不会被置空，但其空间已释放）
void queue_destroy(Queue *p_queue);

// 入队，成功返回0，失败-1.
// @data: 为要插入的数据
int queue_en(Queue *p_queue, const void *data);

// 出队，成功返回0，失败-1（队列空）.
// @data: 用于接收出队的数据
int queue_de(Queue *p_queue, void *data);

// 获取队列中的元素个数
int queue_node_num(Queue *p_queue);

// 判断队列是否为空
// 返回值: 1-empty, 0-not_empty
int queue_is_empty(Queue *p_queue);


#endif /* _ARAN_QUEUE_H_ */

