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
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id: $ */

#ifndef ZEND_STRING_H
#define ZEND_STRING_H

#include "zend.h"

BEGIN_EXTERN_C()

ZEND_API extern zend_string *(*zend_new_interned_string)(zend_string *str);
ZEND_API extern void (*zend_interned_strings_snapshot)(void);
ZEND_API extern void (*zend_interned_strings_restore)(void);

ZEND_API zend_ulong zend_hash_func(const char *str, size_t len);
void zend_interned_strings_init(void);
void zend_interned_strings_dtor(void);

END_EXTERN_C()

/* Shortcuts */
/* 简写 */
#define ZSTR_VAL(zstr)  (zstr)->val 					// 取值
#define ZSTR_LEN(zstr)  (zstr)->len					    // 取字符串长度
#define ZSTR_H(zstr)    (zstr)->h 						// 返回hash值
#define ZSTR_HASH(zstr) zend_string_hash_val(zstr) 		// 获取hash值，先尝试从h中拿，拿不到就计算

/* Compatibility macros */

#define IS_INTERNED(s)	ZSTR_IS_INTERNED(s)
#define STR_EMPTY_ALLOC()	ZSTR_EMPTY_ALLOC()
#define _STR_HEADER_SIZE _ZSTR_HEADER_SIZE
#define STR_ALLOCA_ALLOC(str, _len, use_heap) ZSTR_ALLOCA_ALLOC(str, _len, use_heap)
#define STR_ALLOCA_INIT(str, s, len, use_heap) ZSTR_ALLOCA_INIT(str, s, len, use_heap)
#define STR_ALLOCA_FREE(str, use_heap) ZSTR_ALLOCA_FREE(str, use_heap)

/*---*/

#define ZSTR_IS_INTERNED(s)					(GC_FLAGS(s) & IS_STR_INTERNED)

#define ZSTR_EMPTY_ALLOC()				CG(empty_string)

#define _ZSTR_HEADER_SIZE XtOffsetOf(zend_string, val)

#define _ZSTR_STRUCT_SIZE(len) (_ZSTR_HEADER_SIZE + len + 1)

#define ZSTR_ALLOCA_ALLOC(str, _len, use_heap) do { \
	(str) = (zend_string *)do_alloca(ZEND_MM_ALIGNED_SIZE_EX(_ZSTR_STRUCT_SIZE(_len), 8), (use_heap)); \
	GC_REFCOUNT(str) = 1; \
	GC_TYPE_INFO(str) = IS_STRING; \
	zend_string_forget_hash_val(str); \
	ZSTR_LEN(str) = _len; \
} while (0)

#define ZSTR_ALLOCA_INIT(str, s, len, use_heap) do { \
	ZSTR_ALLOCA_ALLOC(str, len, use_heap); \
	memcpy(ZSTR_VAL(str), (s), (len)); \
	ZSTR_VAL(str)[(len)] = '\0'; \
} while (0)

#define ZSTR_ALLOCA_FREE(str, use_heap) free_alloca(str, use_heap)

/*---*/

/* 返回zend_string的hash值，如果hash值不存在，则调用hash函数生成 */
static zend_always_inline zend_ulong zend_string_hash_val(zend_string *s)
{
	if (!ZSTR_H(s)) {
		ZSTR_H(s) = zend_hash_func(ZSTR_VAL(s), ZSTR_LEN(s));
	}
	return ZSTR_H(s);
}

/* 把字符串的哈希值设置为0 */
static zend_always_inline void zend_string_forget_hash_val(zend_string *s)
{
	/* s->h = 0 */
	ZSTR_H(s) = 0;
}

/* 获取字符串的引用计数 */
static zend_always_inline uint32_t zend_string_refcount(const zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		/* 如果不是内部字符串，返回引用计数 */
		return GC_REFCOUNT(s);
	}
	return 1;
}

/* 增加字符串的引用计数 */
static zend_always_inline uint32_t zend_string_addref(zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		return ++GC_REFCOUNT(s);
	}
	return 1;
}

/* 对一个zend_string进行引用计数减一操作 */
static zend_always_inline uint32_t zend_string_delref(zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		/* 如果不是内部字符串，s->gc.refcount自减一 */
		return --GC_REFCOUNT(s);
	}
	return 1;
}

/* 申请一个zend字符串 */
static zend_always_inline zend_string *zend_string_alloc(size_t len, int persistent)
{
	zend_string *ret = (zend_string *)pemalloc(ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);

	/* 新字符串gc的引用计数值默认为1 ret->gc.refcount = 1 */
	GC_REFCOUNT(ret) = 1;
#if 1
	/* optimized single assignment */
	/* 设置数据类型 */
	GC_TYPE_INFO(ret) = IS_STRING | ((persistent ? IS_STR_PERSISTENT : 0) << 8);
#else
	GC_TYPE(ret) = IS_STRING;
	GC_FLAGS(ret) = (persistent ? IS_STR_PERSISTENT : 0);
	GC_INFO(ret) = 0;
#endif
	zend_string_forget_hash_val(ret);
	ZSTR_LEN(ret) = len; /* ret->len = len */
	return ret;
}

static zend_always_inline zend_string *zend_string_safe_alloc(size_t n, size_t m, size_t l, int persistent)
{
	zend_string *ret = (zend_string *)safe_pemalloc(n, m, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(l)), persistent);

	GC_REFCOUNT(ret) = 1;
#if 1
	/* optimized single assignment */
	GC_TYPE_INFO(ret) = IS_STRING | ((persistent ? IS_STR_PERSISTENT : 0) << 8);
#else
	GC_TYPE(ret) = IS_STRING;
	GC_FLAGS(ret) = (persistent ? IS_STR_PERSISTENT : 0);
	GC_INFO(ret) = 0;
#endif
	zend_string_forget_hash_val(ret);
	ZSTR_LEN(ret) = (n * m) + l;
	return ret;
}

static zend_always_inline zend_string *zend_string_init(const char *str, size_t len, int persistent)
{
	zend_string *ret = zend_string_alloc(len, persistent);

	memcpy(ZSTR_VAL(ret), str, len);
	ZSTR_VAL(ret)[len] = '\0';
	return ret;
}

/* 字符串复制 */
static zend_always_inline zend_string *zend_string_copy(zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		/* 如果不是内部字符串，对s的引用计数+1 */
		GC_REFCOUNT(s)++;
	}
	/* 直接返回s，实际上并没有申请新的内存，对象复用了 */
	return s;
}

/* 与copy函数不同，这个函数重新申请内存并复制了 */
static zend_always_inline zend_string *zend_string_dup(zend_string *s, int persistent)
{
	if (ZSTR_IS_INTERNED(s)) {
		/* 如果是内部字符串，直接返回 */
		return s;
	} else {
		return zend_string_init(ZSTR_VAL(s), ZSTR_LEN(s), persistent);
	}
}

static zend_always_inline zend_string *zend_string_realloc(zend_string *s, size_t len, int persistent)
{
	zend_string *ret;

	if (!ZSTR_IS_INTERNED(s)) {
		if (EXPECTED(GC_REFCOUNT(s) == 1)) {
			ret = (zend_string *)perealloc(s, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);
			ZSTR_LEN(ret) = len;
			zend_string_forget_hash_val(ret);
			return ret;
		} else {
			GC_REFCOUNT(s)--;
		}
	}
	ret = zend_string_alloc(len, persistent);
	memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), MIN(len, ZSTR_LEN(s)) + 1);
	return ret;
}

static zend_always_inline zend_string *zend_string_extend(zend_string *s, size_t len, int persistent)
{
	zend_string *ret;

	ZEND_ASSERT(len >= ZSTR_LEN(s));
	if (!ZSTR_IS_INTERNED(s)) {
		if (EXPECTED(GC_REFCOUNT(s) == 1)) {
			ret = (zend_string *)perealloc(s, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);
			ZSTR_LEN(ret) = len;
			zend_string_forget_hash_val(ret);
			return ret;
		} else {
			GC_REFCOUNT(s)--;
		}
	}
	ret = zend_string_alloc(len, persistent);
	memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), ZSTR_LEN(s) + 1);
	return ret;
}

static zend_always_inline zend_string *zend_string_truncate(zend_string *s, size_t len, int persistent)
{
	zend_string *ret;

	ZEND_ASSERT(len <= ZSTR_LEN(s));
	if (!ZSTR_IS_INTERNED(s)) {
		if (EXPECTED(GC_REFCOUNT(s) == 1)) {
			ret = (zend_string *)perealloc(s, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);
			ZSTR_LEN(ret) = len;
			zend_string_forget_hash_val(ret);
			return ret;
		} else {
			GC_REFCOUNT(s)--;
		}
	}
	ret = zend_string_alloc(len, persistent);
	memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), len + 1);
	return ret;
}

static zend_always_inline zend_string *zend_string_safe_realloc(zend_string *s, size_t n, size_t m, size_t l, int persistent)
{
	zend_string *ret;

	if (!ZSTR_IS_INTERNED(s)) {
		if (GC_REFCOUNT(s) == 1) {
			ret = (zend_string *)safe_perealloc(s, n, m, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(l)), persistent);
			ZSTR_LEN(ret) = (n * m) + l;
			zend_string_forget_hash_val(ret);
			return ret;
		} else {
			GC_REFCOUNT(s)--;
		}
	}
	ret = zend_string_safe_alloc(n, m, l, persistent);
	memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), MIN((n * m) + l, ZSTR_LEN(s)) + 1);
	return ret;
}

static zend_always_inline void zend_string_free(zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		ZEND_ASSERT(GC_REFCOUNT(s) <= 1);
		pefree(s, GC_FLAGS(s) & IS_STR_PERSISTENT);
	}
}

/* 释放掉zend_string */
static zend_always_inline void zend_string_release(zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		/* 如果不是内部字符串，如果自减一后没有任何引用，直接调用pefree释放 */
		if (--GC_REFCOUNT(s) == 0) {
			pefree(s, GC_FLAGS(s) & IS_STR_PERSISTENT);
		}
	}
}


static zend_always_inline zend_bool zend_string_equals(zend_string *s1, zend_string *s2)
{
	return s1 == s2 || (ZSTR_LEN(s1) == ZSTR_LEN(s2) && !memcmp(ZSTR_VAL(s1), ZSTR_VAL(s2), ZSTR_LEN(s1)));
}

#define zend_string_equals_ci(s1, s2) \
	(ZSTR_LEN(s1) == ZSTR_LEN(s2) && !zend_binary_strcasecmp(ZSTR_VAL(s1), ZSTR_LEN(s1), ZSTR_VAL(s2), ZSTR_LEN(s2)))

#define zend_string_equals_literal_ci(str, c) \
	(ZSTR_LEN(str) == sizeof(c) - 1 && !zend_binary_strcasecmp(ZSTR_VAL(str), ZSTR_LEN(str), (c), sizeof(c) - 1))

#define zend_string_equals_literal(str, literal) \
	(ZSTR_LEN(str) == sizeof(literal)-1 && !memcmp(ZSTR_VAL(str), literal, sizeof(literal) - 1))

/*
 * DJBX33A (Daniel J. Bernstein, Times 33 with Addition)
 *
 * This is Daniel J. Bernstein's popular `times 33' hash function as
 * posted by him years ago on comp.lang.c. It basically uses a function
 * like ``hash(i) = hash(i-1) * 33 + str[i]''. This is one of the best
 * known hash functions for strings. Because it is both computed very
 * fast and distributes very well.
 *
 * The magic of number 33, i.e. why it works better than many other
 * constants, prime or not, has never been adequately explained by
 * anyone. So I try an explanation: if one experimentally tests all
 * multipliers between 1 and 256 (as RSE did now) one detects that even
 * numbers are not useable at all. The remaining 128 odd numbers
 * (except for the number 1) work more or less all equally well. They
 * all distribute in an acceptable way and this way fill a hash table
 * with an average percent of approx. 86%.
 *
 * If one compares the Chi^2 values of the variants, the number 33 not
 * even has the best value. But the number 33 and a few other equally
 * good numbers like 17, 31, 63, 127 and 129 have nevertheless a great
 * advantage to the remaining numbers in the large set of possible
 * multipliers: their multiply operation can be replaced by a faster
 * operation based on just one shift plus either a single addition
 * or subtraction operation. And because a hash function has to both
 * distribute good _and_ has to be very fast to compute, those few
 * numbers should be preferred and seems to be the reason why Daniel J.
 * Bernstein also preferred it.
 *
 *
 *                  -- Ralf S. Engelschall <rse@engelschall.com>
 */

static zend_always_inline zend_ulong zend_inline_hash_func(const char *str, size_t len)
{
	register zend_ulong hash = Z_UL(5381);

	/* variant with the hash unrolled eight times */
	for (; len >= 8; len -= 8) {
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
	}
	switch (len) {
		case 7: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 6: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 5: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 4: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 3: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 2: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 1: hash = ((hash << 5) + hash) + *str++; break;
		case 0: break;
EMPTY_SWITCH_DEFAULT_CASE()
	}

	/* Hash value can't be zero, so we always set the high bit */
#if SIZEOF_ZEND_LONG == 8
	return hash | Z_UL(0x8000000000000000);
#elif SIZEOF_ZEND_LONG == 4
	return hash | Z_UL(0x80000000);
#else
# error "Unknown SIZEOF_ZEND_LONG"
#endif
}

static zend_always_inline void zend_interned_empty_string_init(zend_string **s)
{
	zend_string *str;

	str = zend_string_alloc(sizeof("")-1, 1);
	ZSTR_VAL(str)[0] = '\000';

#ifndef ZTS
	*s = zend_new_interned_string(str);
#else
	zend_string_hash_val(str);
	GC_FLAGS(str) |= IS_STR_INTERNED;
	*s = str;
#endif
}

static zend_always_inline void zend_interned_empty_string_free(zend_string **s)
{
	if (NULL != *s) {
		free(*s);
		*s = NULL;
	}
}

#endif /* ZEND_STRING_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
