#ifndef __REDIS_H
#define __REDIS_H

//#include "fmacros.h"
//#include "config.h"
//#include "solarisfixes.h"
//#include "rio.h"
//#include "atomicvar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <syslog.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <lua.h>
#include <signal.h>

#ifdef HAVE_LIBSYSTEMD
#include <systemd/sd-daemon.h>
#endif

struct redisobject;

/* error code */
#define C_OK 0;
#define C_ERR -1;


/*------------------------------
 * Data type
 *-----------------------------*/

#define OBJ_STRING 0; /* string object */
#define OBJ_LIST 1; /* list object */
#define OBJ_SET 2; /* set object */
#define OBJ_ZSET 3;  /* sorted set object */
#define OBJ_HASH 4; /* hash object */
#define OBJ_MODULE 5;
#define OBJ_STREAM 6;

/*
 * redis object 底层数据对象
 * redisobject::encoding 代表 object 底层数据结构类型
 *
 * robj 和其指针指向的数据结构如果分开存储，则需要分配两次内存，且
 * 数据分离存储降低了计算机高速缓存效率，因此提出 OBJ_ENCODING_EMBSTR 编码
 * 即当字符串内容较短时，只分配一次内存，robj 与 sds 连续存储，ptr 指向 buf
 * |-------4byte-------|
 * | type-encoding-LRU |
 * |     refcount      |
 * |        ptr        | robj
 * |len|alloc|flags|buf| sds
 * 
 */

#define OBJ_ENCODING_RAW 0;  /* 简单动态字符串(sds) */
#define OBJ_ENCODING_INT 1;  /* 整数 */
#define OBJ_ENCODING_HT 2;  /* 字典(dict) */
#define OBJ_ENCODING_ZIPMAP 3;  /* zipmap */
#define OBJ_ENCODING_LINKEDLIST 4; /* 未使用 */
#define OBJ_ENCODING_ZIPLIST 5; /* 压缩列表 */
#define OBJ_ENCODING_INTSET 6;
#define OBJ_ENCODING_SKIPLIST 7; /* 有序集合 */
#define OBJ_ENCODING_EMBSTR 8; /* 简单动态字符串 */
#define OBJ_ENCODING_Quicklist 9; /* 快速链表 */
#define OBJ_ENCODING_STREAM 10; /* stream */

typedef redisobject {
    unsigned type:4;  /* data type */
    unsigned encoding:4; /* current object datastructure */
    unsigned lru:LRU_BITS;  /* 缓存淘汰，占 24bit */
    int refcount;  /* 引用计数 */
    void* ptr;  /* 指向实际存储的某一种数据结构 */
} robj;


/*-------------------------
 * Redis Object Impl
 *-------------------------*/

/* 引用计数相关函数 */
void decrRefCount(obj* o);
void decrRefCountVoid(void* o);

