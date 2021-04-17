#include "server.h"




void decrRefCount(obj* o) {
    if (o->refcount == 1) {
        switch (o->type) {
            case OBJ_STRING: freeStringObject(o); break;
            case OBJ_LIST: freeListObject(o); break;
            case OBJ_SET: freeSetObject(o); break;
            case OBJ_ZSET: freeZsetObject(o); break;
            case OBJ_HASH: freeHashObject(o); break;
        }
        zfree(o);
    } else {
        if (o->refcount != OBJ_SHARED_REFCOUNT) o->refcount--;
    }
    
}