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

#ifndef ZEND_GLOBALS_H
#define ZEND_GLOBALS_H


#include <setjmp.h>

#include "zend_globals_macros.h"

#include "zend_stack.h"
#include "zend_ptr_stack.h"
#include "zend_hash.h"
#include "zend_llist.h"
#include "zend_objects.h"
#include "zend_objects_API.h"
#include "zend_modules.h"
#include "zend_float.h"
#include "zend_multibyte.h"
#include "zend_multiply.h"
#include "zend_arena.h"

/* Define ZTS if you want a thread-safe Zend */
/*#undef ZTS*/

#ifdef ZTS

BEGIN_EXTERN_C()
ZEND_API extern int compiler_globals_id;
ZEND_API extern int executor_globals_id;
END_EXTERN_C()

#endif

#define SYMTABLE_CACHE_SIZE 32


#include "zend_compile.h"

/* excpt.h on Digital Unix 4.0 defines function_table */
#undef function_table

#define ZEND_EARLY_BINDING_COMPILE_TIME 0
#define ZEND_EARLY_BINDING_DELAYED      1
#define ZEND_EARLY_BINDING_DELAYED_ALL  2

typedef struct _zend_vm_stack *zend_vm_stack;
typedef struct _zend_ini_entry zend_ini_entry;

/* zend编译器全局常量 */
struct _zend_compiler_globals {
	zend_stack loop_var_stack;

	zend_class_entry *active_class_entry;

	zend_string *compiled_filename;  		 	/* 编译的文件 */

	int zend_lineno; 							/* 行号 */

	zend_op_array *active_op_array;

	HashTable *function_table;					/* function symbol table */
	HashTable *class_table;						/* class table */

	HashTable filenames_table;

	HashTable *auto_globals;

	zend_bool parse_error;     					 /* 是否有语法解析错误 */
	zend_bool in_compilation;
	zend_bool short_tags;

	zend_bool unclean_shutdown;

	zend_bool ini_parser_unbuffered_errors;

	zend_llist open_files;

	struct _zend_ini_parser_param *ini_parser_param;

	uint32_t start_lineno;
	zend_bool increment_lineno;

	zend_string *doc_comment;

	uint32_t compiler_options; /* set of ZEND_COMPILE_* constants */

	HashTable const_filenames;

	zend_oparray_context context;
	zend_file_context file_context;

	zend_arena *arena;

	zend_string *empty_string;
	zend_string *one_char_string[256];

	HashTable interned_strings;

	const zend_encoding **script_encoding_list;
	size_t script_encoding_list_size;
	zend_bool multibyte;
	zend_bool detect_unicode;
	zend_bool encoding_declared;

	zend_ast *ast;
	zend_arena *ast_arena;

	zend_stack delayed_oplines_stack;

#ifdef ZTS
	zval **static_members_table;
	int last_static_member;
#endif
};

/* zend执行过程的全局符号表，EG宏操作对象 */
struct _zend_executor_globals {
	zval uninitialized_zval;
	zval error_zval;

	/* symbol table cache */
	zend_array *symtable_cache[SYMTABLE_CACHE_SIZE];
	zend_array **symtable_cache_limit;
	zend_array **symtable_cache_ptr;

	zend_array symbol_table;		/* main symbol table，全局变量Hash表，$_GET等 */

	HashTable included_files;	/* files already included，已经include了的文件 */

	JMP_BUF *bailout;

	int error_reporting;		// 保存错误码
	int exit_status;  			// 保存退出状态码

	HashTable *function_table;	/* function symbol table，已经编译好的函数表，包括内部函数和用户自定义函数等，函数调用从这里查找 */
	HashTable *class_table;		/* class table，全部已经编译好的类，new的时候从这里查找 */
	HashTable *zend_constants;	/* constants table，常量表 */

	zval          *vm_stack_top; //运行栈内存池，栈顶指针
	zval          *vm_stack_end; //运行栈内存池，栈底指针
	zend_vm_stack  vm_stack; // 运行栈内存池，空白内存池用于分配PHP运行过程一些结构(zend_execute_data)，局部变量就分配在上面

	struct _zend_execute_data *current_execute_data; // 指向当前正在运行的zend_execute_data
	zend_class_entry *scope;

	zend_long precision;

	int ticks_count;

	HashTable *in_autoload;
	zend_function *autoload_func; // 自动加载函数 __autoload()
	zend_bool full_tables_cleanup;

	/* for extended information support */
	zend_bool no_extensions;

#ifdef ZEND_WIN32
	zend_bool timed_out;
	OSVERSIONINFOEX windows_version_info;
#endif

	HashTable regular_list;
	HashTable persistent_list;

	int user_error_handler_error_reporting;
	zval user_error_handler;
	zval user_exception_handler;
	zend_stack user_error_handlers_error_reporting;
	zend_stack user_error_handlers;
	zend_stack user_exception_handlers;

	zend_error_handling_t  error_handling;
	zend_class_entry      *exception_class;

	/* timeout support */
	zend_long timeout_seconds;

	int lambda_count;

	HashTable *ini_directives; //php.ini 配置
	HashTable *modified_ini_directives;
	zend_ini_entry *error_reporting_ini_entry;

	zend_objects_store objects_store;
	zend_object *exception, *prev_exception;
	const zend_op *opline_before_exception;
	zend_op exception_op[3];

	struct _zend_module_entry *current_module;

	zend_bool active;
	zend_bool valid_symbol_table;

	zend_long assertions;

	uint32_t           ht_iterators_count;     /* number of allocatd slots */
	uint32_t           ht_iterators_used;      /* number of used slots */
	HashTableIterator *ht_iterators;
	HashTableIterator  ht_iterators_slots[16];

	void *saved_fpu_cw_ptr;
#if XPFPA_HAVE_CW
	XPFPA_CW_DATATYPE saved_fpu_cw;
#endif

	zend_function trampoline;
	zend_op       call_trampoline_op;

	void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};

struct _zend_ini_scanner_globals {
	zend_file_handle *yy_in;
	zend_file_handle *yy_out;

	unsigned int yy_leng;
	unsigned char *yy_start;
	unsigned char *yy_text;
	unsigned char *yy_cursor;
	unsigned char *yy_marker;
	unsigned char *yy_limit;
	int yy_state;
	zend_stack state_stack;

	char *filename;
	int lineno;

	/* Modes are: ZEND_INI_SCANNER_NORMAL, ZEND_INI_SCANNER_RAW, ZEND_INI_SCANNER_TYPED */
	int scanner_mode;
};

typedef enum {
	ON_TOKEN,
	ON_FEEDBACK,
	ON_STOP
} zend_php_scanner_event;

struct _zend_php_scanner_globals {
	zend_file_handle *yy_in;
	zend_file_handle *yy_out;

	unsigned int yy_leng;
	unsigned char *yy_start;
	unsigned char *yy_text;
	unsigned char *yy_cursor;
	unsigned char *yy_marker;
	unsigned char *yy_limit;
	int yy_state;
	zend_stack state_stack;
	zend_ptr_stack heredoc_label_stack;

	/* original (unfiltered) script */
	unsigned char *script_org;
	size_t script_org_size;

	/* filtered script */
	unsigned char *script_filtered;
	size_t script_filtered_size;

	/* input/output filters */
	zend_encoding_filter input_filter;
	zend_encoding_filter output_filter;
	const zend_encoding *script_encoding;

	/* initial string length after scanning to first variable */
	int scanned_string_len;

	/* hooks */
	void (* on_event)(zend_php_scanner_event event, int token, int line);
};

#endif /* ZEND_GLOBALS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
