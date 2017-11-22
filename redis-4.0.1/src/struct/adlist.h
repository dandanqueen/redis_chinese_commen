/* adlist.h - A generic doubly linked list implementation
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

#ifndef __ADLIST_H__
#define __ADLIST_H__

/* Node, List, and Iterator are the only data structures used currently. */
/*链表节点*/
typedef struct listNode {
    /*前一个节点*/
    struct listNode *prev;
    /*后一个节点*/
    struct listNode *next;
    /*节点值*/
    void *value;
} listNodec;

/*链表迭代器*/
typedef struct listIter {
    /*下一个节点*/
    listNode *next;
    /*访问方向*/
    int direction;
} listIter;

/*双向链表*/
typedef struct list {
    /*链表头节点*/
    listNode *head;
    /*链表尾节点*/
    listNode *tail;
    /*节点值复制函数*/
    void *(*dup)(void *ptr);
    /*节点值释放函数*/
    void (*free)(void *ptr);
    /*节点值比较函数*/
    int (*match)(void *ptr, void *key);
    /*链表长度*/
    unsigned long len;
} list;

/* Functions implemented as macros */
/*宏定义*/
/*获取链表长度*/
#define listLength(l) ((l)->len)
/*获取链表头节点*/
#define listFirst(l) ((l)->head)
/*获取链表尾节点*/
#define listLast(l) ((l)->tail)
/*获取前一个节点*/
#define listPrevNode(n) ((n)->prev)
/*获取后一个节点*/
#define listNextNode(n) ((n)->next)
/*获取节点值*/
#define listNodeValue(n) ((n)->value)
/*设置值复制函数*/
#define listSetDupMethod(l,m) ((l)->dup = (m))
/*设置值释放函数*/
#define listSetFreeMethod(l,m) ((l)->free = (m))
/*设置值比较函数*/
#define listSetMatchMethod(l,m) ((l)->match = (m))
/*获取值复制函数*/
#define listGetDupMethod(l) ((l)->dup)
/*获取值释放函数*/
#define listGetFree(l) ((l)->free)
/*获取值比较函数*/
#define listGetMatchMethod(l) ((l)->match)

/* Prototypes */
/*函数声明*/
/*创建链表*/
list *listCreate(void);
/*释放链表*/
void listRelease(list *list);
/*清空链表*/
void listEmpty(list *list);
/*从头部添加节点*/
list *listAddNodeHead(list *list, void *value);
/*从尾部添加节点*/
list *listAddNodeTail(list *list, void *value);
/*在指定节点处插入节点*/
list *listInsertNode(list *list, listNode *old_node, void *value, int after);
/*删除节点*/
void listDelNode(list *list, listNode *node);
/*取得指定方向的迭代器*/
listIter *listGetIterator(list *list, int direction);
/*通过迭代器访问节点*/
listNode *listNext(listIter *iter);
/*释放迭代器*/
void listReleaseIterator(listIter *iter);
/*复制链表*/
list *listDup(list *orig);
/*链表中搜索值*/
listNode *listSearchKey(list *list, void *key);
/*获取指定下标的节点*/
listNode *listIndex(list *list, long index);
void listRewind(list *list, listIter *li);
void listRewindTail(list *list, listIter *li);
void listRotate(list *list);
void listJoin(list *l, list *o);

/* Directions for iterators */
#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif /* __ADLIST_H__ */
