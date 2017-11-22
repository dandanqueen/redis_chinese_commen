/* Hash Tables Implementation.
 *
 * This file implements in-memory hash tables with insert/del/replace/find/
 * get-random-element operations. Hash tables will auto-resize if needed
 * tables of power of two in size are used, collisions are handled by
 * chaining. See the source code for more information... :)
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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

#include <stdint.h>

#ifndef __DICT_H
#define __DICT_H

#define DICT_OK 0
#define DICT_ERR 1

/* Unused arguments generate annoying warnings... */
#define DICT_NOTUSED(V) ((void) V)

/*hash表元素*/
typedef struct dictEntry {
    /*元素key*/
    void *key;
    /*元素值*/
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    } v;
    /*下一个元素*/
    struct dictEntry *next;
} dictEntry;

/*hash表类型*/
typedef struct dictType {
    /*根据key计算hash值*/
    uint64_t (*hashFunction)(const void *key);
    /*复制key*/
    void *(*keyDup)(void *privdata, const void *key);
    /*复制value*/
    void *(*valDup)(void *privdata, const void *obj);
    /*key比较*/
    int (*keyCompare)(void *privdata, const void *key1, const void *key2);
    /*key释放*/
    void (*keyDestructor)(void *privdata, void *key);
    /*value释放*/
    void (*valDestructor)(void *privdata, void *obj);
} dictType;

/* This is our hash table structure. Every dictionary has two of this as we
 * implement incremental rehashing, for the old to the new table. */
/*hash表*/
typedef struct dictht {
    /*二维hash元素集合*/
    dictEntry **table;
    /*hash表总大小*/
    unsigned long size;
    /*总大小掩码*/
    unsigned long sizemask;
    /*已经用了的空间*/
    unsigned long used;
} dictht;

/*dict*/
typedef struct dict {
    /*dict类型*/
    dictType *type;
    void *privdata;
    /*两张hash表*/
    dictht ht[2];
    /*重新hash下标*/
    long rehashidx; /* rehashing not in progress if rehashidx == -1 */
    /*迭代器数目*/
    unsigned long iterators; /* number of iterators currently running */
} dict;

/* If safe is set to 1 this is a safe iterator, that means, you can call
 * dictAdd, dictFind, and other functions against the dictionary even while
 * iterating. Otherwise it is a non safe iterator, and only dictNext()
 * should be called while iterating. */
/*dict迭代器*/
typedef struct dictIterator {
    /*要迭代的dict*/
    dict *d;
    /*迭代下标*/
    long index;
    int table, safe;
    /*临时存储表元素*/
    dictEntry *entry, *nextEntry;
    /* unsafe iterator fingerprint for misuse detection. */
    long long fingerprint;
} dictIterator;

typedef void (dictScanFunction)(void *privdata, const dictEntry *de);
typedef void (dictScanBucketFunction)(void *privdata, dictEntry **bucketref);

/* This is the initial size of every hash table */
/*dict中hash table初始大小
*/
#define DICT_HT_INITIAL_SIZE     4

/* ------------------------------- Macros ------------------------------------*/
/*释放entry value*/
#define dictFreeVal(d, entry) \
    if ((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->v.val)

/*设置entry value*/
#define dictSetVal(d, entry, _val_) do { \
    if ((d)->type->valDup) \
        (entry)->v.val = (d)->type->valDup((d)->privdata, _val_); \
    else \
        (entry)->v.val = (_val_); \
} while(0)

/*设置有符号整数值*/
#define dictSetSignedIntegerVal(entry, _val_) \
    do { (entry)->v.s64 = _val_; } while(0)

/*设置无符号整数值*/
#define dictSetUnsignedIntegerVal(entry, _val_) \
    do { (entry)->v.u64 = _val_; } while(0)

/*设置浮点数值*/
#define dictSetDoubleVal(entry, _val_) \
    do { (entry)->v.d = _val_; } while(0)

/*释放entry key*/
#define dictFreeKey(d, entry) \
    if ((d)->type->keyDestructor) \
        (d)->type->keyDestructor((d)->privdata, (entry)->key)

/*设置 entry key*/
#define dictSetKey(d, entry, _key_) do { \
    if ((d)->type->keyDup) \
        (entry)->key = (d)->type->keyDup((d)->privdata, _key_); \
    else \
        (entry)->key = (_key_); \
} while(0)

/*entry key比较函数*/
#define dictCompareKeys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
        (d)->type->keyCompare((d)->privdata, key1, key2) : \
        (key1) == (key2))

/*计算key的hash值*/
#define dictHashKey(d, key) (d)->type->hashFunction(key)
/*取得key*/
#define dictGetKey(he) ((he)->key)
/*取得指针值*/
#define dictGetVal(he) ((he)->v.val)
/*取得有符号整数值*/
#define dictGetSignedIntegerVal(he) ((he)->v.s64)
/*取得无符号整数值*/
#define dictGetUnsignedIntegerVal(he) ((he)->v.u64)
/*取得浮点数值*/
#define dictGetDoubleVal(he) ((he)->v.d)
/*两张hash表总空间*/
#define dictSlots(d) ((d)->ht[0].size+(d)->ht[1].size)
/*两张hash表总已使用空间*/
#define dictSize(d) ((d)->ht[0].used+(d)->ht[1].used)
/*是否正在重新hash*/
#define dictIsRehashing(d) ((d)->rehashidx != -1)

/* API */
/*创建dict*/
dict *dictCreate(dictType *type, void *privDataPtr);
/*dict扩充容量*/
int dictExpand(dict *d, unsigned long size);
/*dict添加新key-value对*/
int dictAdd(dict *d, void *key, void *val);
/*如果key已存在就赋值给existing返回NULL，否则返回新添加的entry*/
dictEntry *dictAddRaw(dict *d, void *key, dictEntry **existing);
/*dictAddRaw的封装，返回新添加或已存在的entry*/
dictEntry *dictAddOrFind(dict *d, void *key);
/*替换dict中指定key的值*/
int dictReplace(dict *d, void *key, void *val);
/*删除dict中的key*/
int dictDelete(dict *d, const void *key);
/*将entry从hash表中断开连接并返回，但没有释放，方便在释放前做其他操作，
避免多次遍历查找*/
dictEntry *dictUnlink(dict *ht, const void *key);
/*释放已断开连接的entry*/
void dictFreeUnlinkedEntry(dict *d, dictEntry *he);
/*释放dict*/
void dictRelease(dict *d);
/*根据key在dict中查找entry*/
dictEntry * dictFind(dict *d, const void *key);
/*根据key获取值*/
void *dictFetchValue(dict *d, const void *key);
/*调整容量到包含所有元素的最小值*/
int dictResize(dict *d);
/*创建一个dict的迭代器*/
dictIterator *dictGetIterator(dict *d);
/*创建一个dict的安全迭代器*/
dictIterator *dictGetSafeIterator(dict *d);
/*迭代器遍历entry*/
dictEntry *dictNext(dictIterator *iter);
/*释放迭代器*/
void dictReleaseIterator(dictIterator *iter);
/*随机取得dict中一个entry*/
dictEntry *dictGetRandomKey(dict *d);
/*随机获取指定个数entry*/
unsigned int dictGetSomeKeys(dict *d, dictEntry **des, unsigned int count);
/*调试用，取得dict当前状态*/
void dictGetStats(char *buf, size_t bufsize, dict *d);
/*计算key的hash值*/
uint64_t dictGenHashFunction(const void *key, int len);
/*不区分大小写生成hash值*/
uint64_t dictGenCaseHashFunction(const unsigned char *buf, int len);
/*清空dict并调用回调函数*/
void dictEmpty(dict *d, void(callback)(void*));
/*打开dict调整大小开关*/
void dictEnableResize(void);
/*关闭dict调整大小开关*/
void dictDisableResize(void);
/*dict再hash*/
int dictRehash(dict *d, int n);
/*dict再hash指定时间*/
int dictRehashMilliseconds(dict *d, int ms);
/*设置hash种子*/
void dictSetHashFunctionSeed(uint8_t *seed);
/*取得hash种子*/
uint8_t *dictGetHashFunctionSeed(void);
/*遍历dict*/
unsigned long dictScan(dict *d, unsigned long v, dictScanFunction *fn, dictScanBucketFunction *bucketfn, void *privdata);
/*计算key的hash值*/
unsigned int dictGetHash(dict *d, const void *key);
/*根据指针和hash值查找entry*/
dictEntry **dictFindEntryRefByPtrAndHash(dict *d, const void *oldptr, unsigned int hash);

/* Hash table types */
extern dictType dictTypeHeapStringCopyKey;
extern dictType dictTypeHeapStrings;
extern dictType dictTypeHeapStringCopyKeyValue;

#endif /* __DICT_H */
