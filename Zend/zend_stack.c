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

#include "zend.h"
#include "zend_stack.h"

/* 返回stack,并返回一个指向第n个元素地址的指针，n = 0 是首地址 ... */
#define ZEND_STACK_ELEMENT(stack, n) ((void *)((char *) (stack)->elements + (stack)->size * (n)))

/* 初始化一个zend堆栈，堆栈是用连续内存空间存储 */
ZEND_API int zend_stack_init(zend_stack *stack, int size)
{
	stack->size = size; 	/* 堆栈一个节点的大小 */
	stack->top = 0;			/* 当前堆栈大小 */
	stack->max = 0;			/* 堆栈容量 */
	stack->elements = NULL; /* 初始化指向堆栈链表的指针 */
	return SUCCESS;
}

/* 元素入栈 */
ZEND_API int zend_stack_push(zend_stack *stack, const void *element)
{
	/* We need to allocate more memory */
	/* 当前堆栈大小超过了容量，需要申请更多内存 */
	if (stack->top >= stack->max) {
		stack->max += STACK_BLOCK_SIZE; /* 每次多申请16个堆栈元素空间 */
		stack->elements = safe_erealloc(stack->elements, stack->size, stack->max, 0); /* 内存申请操作 */
	}
	/* 将新元素通过内存拷贝到堆栈链表内存空间 */
	memcpy(ZEND_STACK_ELEMENT(stack, stack->top), element, stack->size);
	return stack->top++; /* 栈顶计数器+1 */
}

/* 返回栈顶元素的地址，不出栈 */
ZEND_API void *zend_stack_top(const zend_stack *stack)
{
	if (stack->top > 0) {
		/* 判断堆栈计数器，大于0代表有数据 */
		/* 注意，堆栈计数器是从1开始计数，需要 stack->top - 1 */
		return ZEND_STACK_ELEMENT(stack, stack->top - 1);
	} else {
		/* 堆栈为空什么都不做 */
		return NULL;
	}
}

/* 栈顶出栈，但是不释放内存 */
ZEND_API int zend_stack_del_top(zend_stack *stack)
{
	--stack->top; /* 简单地将栈顶计数器-1，不释放内存 */
	return SUCCESS;
}

/* 返回栈顶元素的内容，不出栈 */
ZEND_API int zend_stack_int_top(const zend_stack *stack)
{
	/* 获取栈顶元素内存地址 */
	int *e = zend_stack_top(stack);
	if (e) {
		/* 地址存在，取内容返回 */
		return *e;
	} else {
		return FAILURE;
	}
}

/* 检查堆栈是否为空 */
ZEND_API int zend_stack_is_empty(const zend_stack *stack)
{
	/* 简单的判断栈顶计数器是否等于0，时间复杂度为O(1) */
	return stack->top == 0;
}

/* 清空堆栈，并释放内存 */
ZEND_API int zend_stack_destroy(zend_stack *stack)
{
	if (stack->elements) {
		efree(stack->elements);
		stack->elements = NULL;
	}

	return SUCCESS;
}


ZEND_API void *zend_stack_base(const zend_stack *stack)
{
	return stack->elements;
}

/* 获取堆栈计数器，时间复杂度O(1) */
ZEND_API int zend_stack_count(const zend_stack *stack)
{
	return stack->top;
}


ZEND_API void zend_stack_apply(zend_stack *stack, int type, int (*apply_function)(void *element))
{
	int i;

	switch (type) {
		case ZEND_STACK_APPLY_TOPDOWN:
			for (i=stack->top-1; i>=0; i--) {
				if (apply_function(ZEND_STACK_ELEMENT(stack, i))) {
					break;
				}
			}
			break;
		case ZEND_STACK_APPLY_BOTTOMUP:
			for (i=0; i<stack->top; i++) {
				if (apply_function(ZEND_STACK_ELEMENT(stack, i))) {
					break;
				}
			}
			break;
	}
}


ZEND_API void zend_stack_apply_with_argument(zend_stack *stack, int type, int (*apply_function)(void *element, void *arg), void *arg)
{
	int i;

	switch (type) {
		case ZEND_STACK_APPLY_TOPDOWN:
			for (i=stack->top-1; i>=0; i--) {
				if (apply_function(ZEND_STACK_ELEMENT(stack, i), arg)) {
					break;
				}
			}
			break;
		case ZEND_STACK_APPLY_BOTTOMUP:
			for (i=0; i<stack->top; i++) {
				if (apply_function(ZEND_STACK_ELEMENT(stack, i), arg)) {
					break;
				}
			}
			break;
	}
}

ZEND_API void zend_stack_clean(zend_stack *stack, void (*func)(void *), zend_bool free_elements)
{
	int i;

	if (func) {
		for (i = 0; i < stack->top; i++) {
			func(ZEND_STACK_ELEMENT(stack, i));
		}
	}
	if (free_elements) {
		if (stack->elements) {
			efree(stack->elements);
			stack->elements = NULL;
		}
		stack->top = stack->max = 0;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
