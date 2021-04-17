#include "server.h"

/* 
 * 延迟删除线程来释放对象
 * 只使用 decrRefCount() 函数更新
 * object 的引用计数
 */

void lazyfreeFreeObject(void* args[]) {
    robj *o = (robj *) args[0];
    decrRefCount(o);

}

void lazyfreeFreeDatabase(void* args[]) {

}

void lazyfreeFreeSlotsMap(void* args[]) {

}