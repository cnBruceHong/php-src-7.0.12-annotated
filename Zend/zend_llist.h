/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_LLIST_H
#define ZEND_LLIST_H

/* Zend链表节点 */
typedef struct _zend_llist_element {
	struct _zend_llist_element *next; /* 指向下一个节点 */
	struct _zend_llist_element *prev; /* 指向前一个节点 */
	char data[1]; /* Needs to always be last in the struct */ /* 总是要存放结构中的最后一个元素 */
} zend_llist_element;

/* 函数指针代表的函数原型定义 */
typedef void (*llist_dtor_func_t)(void *);
typedef int (*llist_compare_func_t)(const zend_llist_element **, const zend_llist_element **);
typedef void (*llist_apply_with_args_func_t)(void *data, int num_args, va_list args);
typedef void (*llist_apply_with_arg_func_t)(void *data, void *arg);
typedef void (*llist_apply_func_t)(void *);

/* Zend链表结构 */
typedef struct _zend_llist {
	zend_llist_element *head; /* 指向头结点元素 */
	zend_llist_element *tail; /* 指向尾部节点元素 */
	size_t count; /* 链表长度，可以认为是元素个数 */
	size_t size; /* 每个节点元素占用的内存大小，用于在内存申请时计算申请量 */
	llist_dtor_func_t dtor; /* 针对链表节点元素的析构函数 */
	unsigned char persistent; /* 是否将内存真正归还操作系统，1：free() 0：efree() */
	zend_llist_element *traverse_ptr; /* 内部的遍历指针，指向遍历元素 */
} zend_llist;

typedef zend_llist_element* zend_llist_position; /* 指向链表节点的位置指针 */

/* API定义区 */
BEGIN_EXTERN_C()
ZEND_API void zend_llist_init(zend_llist *l, size_t size, llist_dtor_func_t dtor, unsigned char persistent);
ZEND_API void zend_llist_add_element(zend_llist *l, void *element);
ZEND_API void zend_llist_prepend_element(zend_llist *l, void *element);
ZEND_API void zend_llist_del_element(zend_llist *l, void *element, int (*compare)(void *element1, void *element2));
ZEND_API void zend_llist_destroy(zend_llist *l);
ZEND_API void zend_llist_clean(zend_llist *l);
ZEND_API void zend_llist_remove_tail(zend_llist *l);
ZEND_API void zend_llist_copy(zend_llist *dst, zend_llist *src);
ZEND_API void zend_llist_apply(zend_llist *l, llist_apply_func_t func);
ZEND_API void zend_llist_apply_with_del(zend_llist *l, int (*func)(void *data));
ZEND_API void zend_llist_apply_with_argument(zend_llist *l, llist_apply_with_arg_func_t func, void *arg);
ZEND_API void zend_llist_apply_with_arguments(zend_llist *l, llist_apply_with_args_func_t func, int num_args, ...);
ZEND_API size_t zend_llist_count(zend_llist *l);
ZEND_API void zend_llist_sort(zend_llist *l, llist_compare_func_t comp_func);

/* traversal 遍历方法API */
ZEND_API void *zend_llist_get_first_ex(zend_llist *l, zend_llist_position *pos);
ZEND_API void *zend_llist_get_last_ex(zend_llist *l, zend_llist_position *pos);
ZEND_API void *zend_llist_get_next_ex(zend_llist *l, zend_llist_position *pos);
ZEND_API void *zend_llist_get_prev_ex(zend_llist *l, zend_llist_position *pos);

#define zend_llist_get_first(l) zend_llist_get_first_ex(l, NULL)
#define zend_llist_get_last(l) zend_llist_get_last_ex(l, NULL)
#define zend_llist_get_next(l) zend_llist_get_next_ex(l, NULL)
#define zend_llist_get_prev(l) zend_llist_get_prev_ex(l, NULL)

END_EXTERN_C()

#endif /* ZEND_LLIST_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */