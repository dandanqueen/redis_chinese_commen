/*
 * Copyright (c) 2009-2012, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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

#ifndef _ZIPLIST_H
#define _ZIPLIST_H

#define ZIPLIST_HEAD 0
#define ZIPLIST_TAIL 1

/*创建新的字符串压缩链表*/
unsigned char *ziplistNew(void);
/*合并两条压缩链表为一条压缩链表*/
unsigned char *ziplistMerge(unsigned char **first, unsigned char **second);
/*向压缩链表指定位置插入指定长度的字符串*/
unsigned char *ziplistPush(unsigned char *zl, unsigned char *s, unsigned int slen, int where);
/*通过下标获取压缩链表中指定节点*/
unsigned char *ziplistIndex(unsigned char *zl, int index);
/*获取压缩链表中指定节点的下一个节点*/
unsigned char *ziplistNext(unsigned char *zl, unsigned char *p);
/*获取压缩链表中指定节点的前一个节点*/
unsigned char *ziplistPrev(unsigned char *zl, unsigned char *p);
/*在压缩链表中查找*/
unsigned int ziplistGet(unsigned char *p, unsigned char **sval, unsigned int *slen, long long *lval);
/*在压缩链表中指定位置插入字符串*/
unsigned char *ziplistInsert(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen);
/*删除压缩链表中指定字符串*/
unsigned char *ziplistDelete(unsigned char *zl, unsigned char **p);
/*删除压缩链表中指定范围的节点*/
unsigned char *ziplistDeleteRange(unsigned char *zl, int index, unsigned int num);
/*压缩链表相比较*/
unsigned int ziplistCompare(unsigned char *p, unsigned char *s, unsigned int slen);
/*压缩链表中查找*/
unsigned char *ziplistFind(unsigned char *p, unsigned char *vstr, unsigned int vlen, unsigned int skip);
/*字符串压缩链表长度*/
unsigned int ziplistLen(unsigned char *zl);
/*字符串压缩链表长度*/
size_t ziplistBlobLen(unsigned char *zl);
/**/
void ziplistRepr(unsigned char *zl);

#ifdef REDIS_TEST
int ziplistTest(int argc, char *argv[]);
#endif

#endif /* _ZIPLIST_H */
