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
#define C_OK 0
#define C_ERR -1

/* log level */
#define LL_DEBUG 0
#define LL_VERBOSE 1
#define LL_NOTICE 2
#define LL_WARNING 3
#define LL_RAW (1<<10) /* 无需时间戳即可记录 */


#define serverPanic(...) _serverPanic(__FILE__,__LINE__,__VA_ARGS__),redis_unreachable()

/*------------------------------
 * Data type
 *-----------------------------*/

#define OBJ_STRING 0 /* string object */
#define OBJ_LIST 1 /* list object */
#define OBJ_SET 2 /* set object */
#define OBJ_ZSET 3  /* sorted set object */
#define OBJ_HASH 4 /* hash object */
#define OBJ_MODULE 5
#define OBJ_STREAM 6

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

#define OBJ_ENCODING_RAW 0  /* 简单动态字符串(sds) */
#define OBJ_ENCODING_INT 1  /* 整数 */
#define OBJ_ENCODING_HT 2  /* 字典(dict) */
#define OBJ_ENCODING_ZIPMAP 3  /* zipmap */
#define OBJ_ENCODING_LINKEDLIST 4 /* 未使用 */
#define OBJ_ENCODING_ZIPLIST 5 /* 压缩列表 */
#define OBJ_ENCODING_INTSET 6
#define OBJ_ENCODING_SKIPLIST 7 /* 有序集合 */
#define OBJ_ENCODING_EMBSTR 8 /* 简单动态字符串 */
#define OBJ_ENCODING_Quicklist 9 /* 快速链表 */
#define OBJ_ENCODING_STREAM 10 /* stream */


#define LRU_BITS 24  /* 低 8bit 存储对方访问次数，高 16bit 存储对象上次访问时间 */


#define OBJ_SHARED_REFCOUNT INT_MAX
#define OBJ_STATIC_REFCOUNT (INT_MAX-1)
#define OBJ_FIRST_SPECIAL_REFCOUNT OBJ_STATIC_REFCOUNT

typedef struct redisobject {
    unsigned type:4;  /* data type */
    unsigned encoding:4; /* current object datastructure */
    unsigned lru:LRU_BITS;  /* 缓存淘汰，占 24bit */
    int refcount;  /* 引用计数 */
    void* ptr;  /* 指向实际存储的某一种数据结构 */
} robj;


/*-------------------------
 * Redis Object Impl
 *-------------------------*/

/* refcount relative functions, in "object.c" file */
void incrRefCount(robj *o);
void decrRefCount(robj *o);
void decrRefCountVoid(void *o);



/*--------------------------
 * Global server state
 *-------------------------*/

 struct redisServer {
    pid_t pid;  /* Main process pid */
    pthread_t main_thread_id;  /* Main thread id */


    redisAtomic unsigned int lruclock;  /* redis每一秒执行系统调用获取，通过LRU_CLOCK函数获取当前时间 */


 };

 #endif