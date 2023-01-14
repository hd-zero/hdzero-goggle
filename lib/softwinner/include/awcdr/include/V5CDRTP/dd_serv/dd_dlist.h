#ifndef __LLIST_R_H__
#define __LLIST_R_H__

/*
* func:	   双链表（循环）,该版本可用于多线程环境
* author:  hudingshan@xiaoyi.com
* date:	   2018-06-04
*/

#include <pthread.h>

struct llist_node_st
{
	struct llist_node_st *prev;
	struct llist_node_st *next;
	char data[0];	// 用于存放结点内的数据
};

typedef struct 
{
	struct llist_node_st head;	// 链表头
	int data_size;  // 链表内结点数据大小
	int node_num;   // 链表中的结点数
	pthread_mutex_t mutex;	// 用于多线程环境加锁
} Llist;

// 链表中结点用于比较的函数指针
typedef int Llist_cmp(const void *key, const void *data);

// 对链表遍历时，对每个结点操作的函数指针
typedef void Llist_op(void *node_data, void *private_data);


// 创建一个循环双链表，返回链表的指针
// @data_size: 指链表中结点所占的空间，如 sizeof(struct xxx_st)
Llist *llist_create(int data_size);

// 销毁一个链表（和free()一样，p_llist不会被置空，但其空间已释放）
void llist_destroy(Llist *p_llist);

// 获取链表中的节点数
int llist_node_num(Llist *p_llist);

// 默认的插入结点函数，插到链表的前面
// @data: 为结点中要存储数据的地址
int llist_insert(Llist *p_llist, const void *data);

// 将结点插入到链表的末尾
// @data: 为结点中要存储数据的地址
int llist_insert_tail(Llist *p_llist, const void *data);

// 删除指定key的结点
int llist_delete(Llist *p_llist, const void *key, Llist_cmp *cmp);

// 查找制定key的结点
void *llist_find(Llist *p_llist, const void *key, Llist_cmp *cmp);

// 获取一个指定key的结点，保存在*data中，并把该结点从链表中删除掉
int llist_fetch(Llist *p_llist, void *data, const void *key, Llist_cmp *cmp);

// 根据指定的op函数，从头开始顺序遍历链表
void llist_travel(Llist *p_llist, Llist_op *op, void *private_data);


#endif 

