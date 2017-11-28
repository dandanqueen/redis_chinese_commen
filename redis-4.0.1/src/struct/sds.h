/* SDSLib 2.0 -- A C dynamic strings library
 *
 * Copyright (c) 2006-2015, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2015, Oran Agra
 * Copyright (c) 2015, Redis Labs, Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SDS_H
#define __SDS_H

#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>
#include <stdint.h>

/*动态字符串类型其实就是char类型的指针*/
typedef char *sds;

/* Note: sdshdr5 is never used, we just access the flags byte directly.
 * However is here to document the layout of type 5 SDS strings. */
/*已废弃*/
struct __attribute__ ((__packed__)) sdshdr5 {
    unsigned char flags; /* 3 lsb of type, and 5 msb of string length */
    char buf[];
};
struct __attribute__ ((__packed__)) sdshdr8 {
    uint8_t len; /* used */
    uint8_t alloc; /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct __attribute__ ((__packed__)) sdshdr16 {
    uint16_t len; /* used */
    uint16_t alloc; /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct __attribute__ ((__packed__)) sdshdr32 {
    uint32_t len; /* used */
    uint32_t alloc; /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};

/*__attribute__ ((__packed__))表示取消系统自动对齐，结构体大小为各字段之和*/
/*动态字符串头部结构体类型，用于记录动态字符串信息*/
struct __attribute__ ((__packed__)) sdshdr64 {
    /*uint64_t是stdint标准库中定长的数据类型，不随平台变化*/
    /*字符串长度*/
    uint64_t len; /* used */
    /*总申请的字符串空间大小，不包括头结构体和终止符*/
    uint64_t alloc; /* excluding the header and null terminator */
    /*前三位表示动态字符串类型，后五位未使用*/
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    /*实际字符串存储空间*/
    char buf[];
};

#define SDS_TYPE_5  0
#define SDS_TYPE_8  1
#define SDS_TYPE_16 2
#define SDS_TYPE_32 3
#define SDS_TYPE_64 4
#define SDS_TYPE_MASK 7
#define SDS_TYPE_BITS 3
/*##起连接作用*/
/*取得动态字符串头并赋值给sh*/
#define SDS_HDR_VAR(T,s) struct sdshdr##T *sh = (void*)((s)-(sizeof(struct sdshdr##T)));
/*取得动态字符串头*/
#define SDS_HDR(T,s) ((struct sdshdr##T *)((s)-(sizeof(struct sdshdr##T))))
#define SDS_TYPE_5_LEN(f) ((f)>>SDS_TYPE_BITS)

/*C99标准中支持了inline内联*/
/*取得动态字符串长度*/
static inline size_t sdslen(const sds s) {
    /*buf前一个字节就是类型标志*/
    unsigned char flags = s[-1];
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5:
            return SDS_TYPE_5_LEN(flags);
        case SDS_TYPE_8:
            return SDS_HDR(8,s)->len;
        case SDS_TYPE_16:
            return SDS_HDR(16,s)->len;
        case SDS_TYPE_32:
            return SDS_HDR(32,s)->len;
        case SDS_TYPE_64:
            return SDS_HDR(64,s)->len;
    }
    return 0;
}

/*计算动态字符串可用空间*/
static inline size_t sdsavail(const sds s) {
    unsigned char flags = s[-1];
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5: {
            return 0;
        }
        case SDS_TYPE_8: {
            SDS_HDR_VAR(8,s);
            return sh->alloc - sh->len;
        }
        case SDS_TYPE_16: {
            SDS_HDR_VAR(16,s);
            return sh->alloc - sh->len;
        }
        case SDS_TYPE_32: {
            SDS_HDR_VAR(32,s);
            return sh->alloc - sh->len;
        }
        case SDS_TYPE_64: {
            SDS_HDR_VAR(64,s);
            return sh->alloc - sh->len;
        }
    }
    return 0;
}

/*设置动态字符串长度头结构体属性*/
static inline void sdssetlen(sds s, size_t newlen) {
    unsigned char flags = s[-1];
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5:
            {
                unsigned char *fp = ((unsigned char*)s)-1;
                *fp = SDS_TYPE_5 | (newlen << SDS_TYPE_BITS);
            }
            break;
        case SDS_TYPE_8:
            SDS_HDR(8,s)->len = newlen;
            break;
        case SDS_TYPE_16:
            SDS_HDR(16,s)->len = newlen;
            break;
        case SDS_TYPE_32:
            SDS_HDR(32,s)->len = newlen;
            break;
        case SDS_TYPE_64:
            SDS_HDR(64,s)->len = newlen;
            break;
    }
}

/*增加动态字符串长度属性值*/
static inline void sdsinclen(sds s, size_t inc) {
    unsigned char flags = s[-1];
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5:
            {
                unsigned char *fp = ((unsigned char*)s)-1;
                unsigned char newlen = SDS_TYPE_5_LEN(flags)+inc;
                *fp = SDS_TYPE_5 | (newlen << SDS_TYPE_BITS);
            }
            break;
        case SDS_TYPE_8:
            SDS_HDR(8,s)->len += inc;
            break;
        case SDS_TYPE_16:
            SDS_HDR(16,s)->len += inc;
            break;
        case SDS_TYPE_32:
            SDS_HDR(32,s)->len += inc;
            break;
        case SDS_TYPE_64:
            SDS_HDR(64,s)->len += inc;
            break;
    }
}

/*取得动态字符串分配空间大小属性值*/
/* sdsalloc() = sdsavail() + sdslen() */
static inline size_t sdsalloc(const sds s) {
    unsigned char flags = s[-1];
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5:
            return SDS_TYPE_5_LEN(flags);
        case SDS_TYPE_8:
            return SDS_HDR(8,s)->alloc;
        case SDS_TYPE_16:
            return SDS_HDR(16,s)->alloc;
        case SDS_TYPE_32:
            return SDS_HDR(32,s)->alloc;
        case SDS_TYPE_64:
            return SDS_HDR(64,s)->alloc;
    }
    return 0;
}

/*设置动态字符串分配空间大小属性值*/
static inline void sdssetalloc(sds s, size_t newlen) {
    unsigned char flags = s[-1];
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5:
            /* Nothing to do, this type has no total allocation info. */
            break;
        case SDS_TYPE_8:
            SDS_HDR(8,s)->alloc = newlen;
            break;
        case SDS_TYPE_16:
            SDS_HDR(16,s)->alloc = newlen;
            break;
        case SDS_TYPE_32:
            SDS_HDR(32,s)->alloc = newlen;
            break;
        case SDS_TYPE_64:
            SDS_HDR(64,s)->alloc = newlen;
            break;
    }
}

/*创建动态字符串并初始化字符串内容和长度*/
sds sdsnewlen(const void *init, size_t initlen);
/*创建动态字符串并初始化字符串内容*/
sds sdsnew(const char *init);
/*创建空的动态字符串*/
sds sdsempty(void);
/*复制动态字符串*/
sds sdsdup(const sds s);
/*释放动态字符串*/
void sdsfree(sds s);
/*增长指定长度*/
sds sdsgrowzero(sds s, size_t len);
/*连接指定长度普通字符串*/
sds sdscatlen(sds s, const void *t, size_t len);
/*连接普通字符串*/
sds sdscat(sds s, const char *t);
/*连接动态字符串*/
sds sdscatsds(sds s, const sds t);
/*拷贝普通字符串指定长度*/
sds sdscpylen(sds s, const char *t, size_t len);
/*拷贝普通字符串*/
sds sdscpy(sds s, const char *t);
/*格式化打印动态字符串*/
sds sdscatvprintf(sds s, const char *fmt, va_list ap);
#ifdef __GNUC__
/*GUNC编译器*/
sds sdscatprintf(sds s, const char *fmt, ...)
    /*__attribute__ format属性进行参数检查*/
    __attribute__((format(printf, 2, 3)));
#else
sds sdscatprintf(sds s, const char *fmt, ...);
#endif
/*连接格式化字符串*/
sds sdscatfmt(sds s, char const *fmt, ...);
/*去掉两端空格*/
sds sdstrim(sds s, const char *cset);
/*取动态字符串指定范围内容*/
void sdsrange(sds s, int start, int end);
/*更新长度信息*/
void sdsupdatelen(sds s);
/*清空动态字符串*/
void sdsclear(sds s);
/*动态字符串相比较*/
int sdscmp(const sds s1, const sds s2);
/*将字符串拆分成多个动态字符串*/
sds *sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count);
/*释放动态字符串*/
void sdsfreesplitres(sds *tokens, int count);
/*动态字符串转小写*/
void sdstolower(sds s);
/*动态字符串转大写*/
void sdstoupper(sds s);
/*long long构建动态字符串*/
sds sdsfromlonglong(long long value);
/*连接普通字符串*/
sds sdscatrepr(sds s, const char *p, size_t len);
/*拆分参数*/
sds *sdssplitargs(const char *line, int *argc);
/*映射字符*/
sds sdsmapchars(sds s, const char *from, const char *to, size_t setlen);
/*用指定分隔符连接参数*/
sds sdsjoin(char **argv, int argc, char *sep);
/*用指定分隔符连接动态字符串参数*/
sds sdsjoinsds(sds *argv, int argc, const char *sep, size_t seplen);

/* Low level functions exposed to the user API */
/*确保腾出指定空间大小*/
sds sdsMakeRoomFor(sds s, size_t addlen);
/*增进长度*/
void sdsIncrLen(sds s, int incr);
/*去掉无效空格*/
sds sdsRemoveFreeSpace(sds s);
/*取得分配的空间大小*/
size_t sdsAllocSize(sds s);
/*动态字符串分配空间指针*/
void *sdsAllocPtr(sds s);

/* Export the allocator used by SDS to the program using SDS.
 * Sometimes the program SDS is linked to, may use a different set of
 * allocators, but may want to allocate or free things that SDS will
 * respectively free or allocate. */
void *sds_malloc(size_t size);
void *sds_realloc(void *ptr, size_t size);
void sds_free(void *ptr);

#ifdef REDIS_TEST
int sdsTest(int argc, char *argv[]);
#endif

#endif
