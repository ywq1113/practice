#include "./server.h"

void incrRefCount(robj *o) {
    if (o->refcount < OBJ_FIRST_SPECIAL_REFCOUNT) {
        o->refcount++;
    } else {
        if (o->refcount == OBJ_SHARED_REFCOUNT) {
            /* 无法再增加 */
        } else if (o->refcount == OBJ_STATIC_REFCOUNT) {
            /* 如果引用计数为 static_refcount，则 pannic */
            serverPanic("you try to retain an object allocated in stack")
        }
    }
}

void decrRefCount(robj *o) {
    if (o->refcount == 1) {
        switch (o->type) {
            case OBJ_STRING: freeStringObject(o); break;
            case OBJ_LIST: freeListObject(o); break;
            case OBJ_SET: freeSetObject(o); break;
            case OBJ_ZSET: freeZsetObject(o); break;
            case OBJ_HASH: freeHashObject(o); break;
            case OBJ_MODULE: freeModuleObject(o); break;
            case OBJ_STREAM: freeStreamObject(o); break;
            default: serverPanic("unKnown Object type"); break;
        }
        zfree(o);
    } else {
        if (o->refcount <= 0) serverPanic("decrRefCount against refcount<=0");
        if (o->refcount != OBJ_SHARED_REFCOUNT) o->refcount--;
    }
}

void decrRefCountVoid(void *o) {
    decrRefCount(o);
}