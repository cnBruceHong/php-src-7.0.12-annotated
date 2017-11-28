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
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   |          Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_AST_H
#define ZEND_AST_H

#include "zend.h"

#define ZEND_AST_SPECIAL_SHIFT      6
#define ZEND_AST_IS_LIST_SHIFT      7
#define ZEND_AST_NUM_CHILDREN_SHIFT 8

/* AST的节点类型 */
enum _zend_ast_kind {
	/* special nodes */
	/* 特殊节点 */
	ZEND_AST_ZVAL = 1 << ZEND_AST_SPECIAL_SHIFT, //64
	ZEND_AST_ZNODE, //65

	/* declaration nodes */
	/* 声明节点 */
	ZEND_AST_FUNC_DECL, //66
	ZEND_AST_CLOSURE, //67
	ZEND_AST_METHOD, //68
	ZEND_AST_CLASS, //69

	/* list nodes */
	/* 列表节点 */
	ZEND_AST_ARG_LIST = 1 << ZEND_AST_IS_LIST_SHIFT, // 128
	ZEND_AST_LIST, //129
	ZEND_AST_ARRAY, //130
	ZEND_AST_ENCAPS_LIST, // 131
	ZEND_AST_EXPR_LIST, // 132
	ZEND_AST_STMT_LIST, // 133 特殊值，不代表任何语法，用来组织节点
	ZEND_AST_IF,
	ZEND_AST_SWITCH_LIST,
	ZEND_AST_CATCH_LIST,
	ZEND_AST_PARAM_LIST,
	ZEND_AST_CLOSURE_USES,
	ZEND_AST_PROP_DECL,
	ZEND_AST_CONST_DECL,
	ZEND_AST_CLASS_CONST_DECL,
	ZEND_AST_NAME_LIST,
	ZEND_AST_TRAIT_ADAPTATIONS,
	ZEND_AST_USE,

	/* 0 child nodes */
	/* 没有孩子节点的节点，ast叶节点 */
	ZEND_AST_MAGIC_CONST = 0 << ZEND_AST_NUM_CHILDREN_SHIFT,  // 0
	ZEND_AST_TYPE, // 1

	/* 1 child node */
	/* 有一个孩子节点的ast节点 */
	ZEND_AST_VAR = 1 << ZEND_AST_NUM_CHILDREN_SHIFT, // 256
	ZEND_AST_CONST,
	ZEND_AST_UNPACK,
	ZEND_AST_UNARY_PLUS,
	ZEND_AST_UNARY_MINUS, // 260
	ZEND_AST_CAST,
	ZEND_AST_EMPTY,
	ZEND_AST_ISSET,
	ZEND_AST_SILENCE,
	ZEND_AST_SHELL_EXEC,
	ZEND_AST_CLONE,
	ZEND_AST_EXIT,
	ZEND_AST_PRINT,
	ZEND_AST_INCLUDE_OR_EVAL,
	ZEND_AST_UNARY_OP, // 270
	ZEND_AST_PRE_INC,
	ZEND_AST_PRE_DEC,
	ZEND_AST_POST_INC,
	ZEND_AST_POST_DEC,
	ZEND_AST_YIELD_FROM,

	ZEND_AST_GLOBAL,
	ZEND_AST_UNSET,
	ZEND_AST_RETURN,
	ZEND_AST_LABEL,
	ZEND_AST_REF, // 280
	ZEND_AST_HALT_COMPILER,
	ZEND_AST_ECHO,
	ZEND_AST_THROW,
	ZEND_AST_GOTO,
	ZEND_AST_BREAK,
	ZEND_AST_CONTINUE, // 286

	/* 2 child nodes */
	/* 代表有两个孩子节点 */
	ZEND_AST_DIM = 2 << ZEND_AST_NUM_CHILDREN_SHIFT, // 512
	ZEND_AST_PROP,
	ZEND_AST_STATIC_PROP,
	ZEND_AST_CALL,
	ZEND_AST_CLASS_CONST,
	ZEND_AST_ASSIGN,
	ZEND_AST_ASSIGN_REF,
	ZEND_AST_ASSIGN_OP,
	ZEND_AST_BINARY_OP,
	ZEND_AST_GREATER,
	ZEND_AST_GREATER_EQUAL, // 522
	ZEND_AST_AND,
	ZEND_AST_OR,
	ZEND_AST_ARRAY_ELEM,
	ZEND_AST_NEW,
	ZEND_AST_INSTANCEOF,
	ZEND_AST_YIELD,
	ZEND_AST_COALESCE,

	ZEND_AST_STATIC,
	ZEND_AST_WHILE,
	ZEND_AST_DO_WHILE, // 532
	ZEND_AST_IF_ELEM,
	ZEND_AST_SWITCH,
	ZEND_AST_SWITCH_CASE,
	ZEND_AST_DECLARE,
	ZEND_AST_CONST_ELEM,
	ZEND_AST_USE_TRAIT,
	ZEND_AST_TRAIT_PRECEDENCE,
	ZEND_AST_METHOD_REFERENCE,
	ZEND_AST_NAMESPACE,
	ZEND_AST_USE_ELEM, // 542
	ZEND_AST_TRAIT_ALIAS,
	ZEND_AST_GROUP_USE,

	/* 3 child nodes */
	/* 三个孩子节点的 */
	ZEND_AST_METHOD_CALL = 3 << ZEND_AST_NUM_CHILDREN_SHIFT, // 768
	ZEND_AST_STATIC_CALL,
	ZEND_AST_CONDITIONAL,

	ZEND_AST_TRY,
	ZEND_AST_CATCH,
	ZEND_AST_PARAM,
	ZEND_AST_PROP_ELEM,

	/* 4 child nodes */
	/* 四个孩子节点 */
	ZEND_AST_FOR = 4 << ZEND_AST_NUM_CHILDREN_SHIFT, // 1024
	ZEND_AST_FOREACH,
};

typedef uint16_t zend_ast_kind;
typedef uint16_t zend_ast_attr;

/* 普通节点 */
struct _zend_ast {
	zend_ast_kind kind; /* Type of the node (ZEND_AST_* enum constant)，节点的类型 */
	zend_ast_attr attr; /* Additional attribute, use depending on node type，额外的属性 */
	uint32_t lineno;    /* Line number，行号 */
	zend_ast *child[1]; /* Array of children (using struct hack)，子节点数组 */
};

/* Same as zend_ast, but with children count, which is updated dynamically */
/* list节点，和zend_ast差不多，但是多了动态变化的子节点数 */
typedef struct _zend_ast_list {
	zend_ast_kind kind;
	zend_ast_attr attr;
	uint32_t lineno;
	uint32_t children; /* 动态变化 */
	zend_ast *child[1];
} zend_ast_list;

/* Lineno is stored in val.u2.lineno */
/* 数据节点，一般做叶子节点 */
typedef struct _zend_ast_zval {
	zend_ast_kind kind;
	zend_ast_attr attr;
	zval val;
} zend_ast_zval;

/* Separate structure for function and class declaration, as they need extra information. */
/* 这类节点用于函数和类的声明，它们需要携带额外的信息 */
typedef struct _zend_ast_decl {
	zend_ast_kind kind;
	zend_ast_attr attr; /* Unused - for structure compatibility */
	uint32_t start_lineno;
	uint32_t end_lineno;
	uint32_t flags;
	unsigned char *lex_pos;
	zend_string *doc_comment;
	zend_string *name;
	zend_ast *child[4];
} zend_ast_decl;

typedef void (*zend_ast_process_t)(zend_ast *ast);
extern ZEND_API zend_ast_process_t zend_ast_process;

ZEND_API zend_ast *zend_ast_create_zval_ex(zval *zv, zend_ast_attr attr);

ZEND_API zend_ast *zend_ast_create_ex(zend_ast_kind kind, zend_ast_attr attr, ...);
ZEND_API zend_ast *zend_ast_create(zend_ast_kind kind, ...);

ZEND_API zend_ast *zend_ast_create_decl(
	zend_ast_kind kind, uint32_t flags, uint32_t start_lineno, zend_string *doc_comment,
	zend_string *name, zend_ast *child0, zend_ast *child1, zend_ast *child2, zend_ast *child3
);

ZEND_API zend_ast *zend_ast_create_list(uint32_t init_children, zend_ast_kind kind, ...);
ZEND_API zend_ast *zend_ast_list_add(zend_ast *list, zend_ast *op);

ZEND_API int zend_ast_evaluate(zval *result, zend_ast *ast, zend_class_entry *scope);
ZEND_API zend_string *zend_ast_export(const char *prefix, zend_ast *ast, const char *suffix);

ZEND_API zend_ast *zend_ast_copy(zend_ast *ast);
ZEND_API void zend_ast_destroy(zend_ast *ast);
ZEND_API void zend_ast_destroy_and_free(zend_ast *ast);

typedef void (*zend_ast_apply_func)(zend_ast **ast_ptr);
ZEND_API void zend_ast_apply(zend_ast *ast, zend_ast_apply_func fn);

static zend_always_inline zend_bool zend_ast_is_list(zend_ast *ast) {
	return (ast->kind >> ZEND_AST_IS_LIST_SHIFT) & 1;
}
static zend_always_inline zend_ast_list *zend_ast_get_list(zend_ast *ast) {
	ZEND_ASSERT(zend_ast_is_list(ast));
	return (zend_ast_list *) ast;
}

static zend_always_inline zval *zend_ast_get_zval(zend_ast *ast) {
	ZEND_ASSERT(ast->kind == ZEND_AST_ZVAL);
	return &((zend_ast_zval *) ast)->val;
}
static zend_always_inline zend_string *zend_ast_get_str(zend_ast *ast) {
	zval *zv = zend_ast_get_zval(ast);
	ZEND_ASSERT(Z_TYPE_P(zv) == IS_STRING);
	return Z_STR_P(zv);
}

static zend_always_inline uint32_t zend_ast_get_num_children(zend_ast *ast) {
	ZEND_ASSERT(!zend_ast_is_list(ast));
	return ast->kind >> ZEND_AST_NUM_CHILDREN_SHIFT;
}
static zend_always_inline uint32_t zend_ast_get_lineno(zend_ast *ast) {
	if (ast->kind == ZEND_AST_ZVAL) {
		zval *zv = zend_ast_get_zval(ast);
		return zv->u2.lineno;
	} else {
		return ast->lineno;
	}
}

static zend_always_inline zend_ast *zend_ast_create_zval(zval *zv) {
	return zend_ast_create_zval_ex(zv, 0);
}
static zend_always_inline zend_ast *zend_ast_create_zval_from_str(zend_string *str) {
	zval zv;
	ZVAL_STR(&zv, str);
	return zend_ast_create_zval(&zv);
}
static zend_always_inline zend_ast *zend_ast_create_zval_from_long(zend_long lval) {
	zval zv;
	ZVAL_LONG(&zv, lval);
	return zend_ast_create_zval(&zv);
}

static zend_always_inline zend_ast *zend_ast_create_binary_op(uint32_t opcode, zend_ast *op0, zend_ast *op1) {
	return zend_ast_create_ex(ZEND_AST_BINARY_OP, opcode, op0, op1);
}
static zend_always_inline zend_ast *zend_ast_create_assign_op(uint32_t opcode, zend_ast *op0, zend_ast *op1) {
	return zend_ast_create_ex(ZEND_AST_ASSIGN_OP, opcode, op0, op1);
}
static zend_always_inline zend_ast *zend_ast_create_cast(uint32_t type, zend_ast *op0) {
	return zend_ast_create_ex(ZEND_AST_CAST, type, op0);
}

#endif
