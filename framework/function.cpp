#define RPC_FUNC_HEADER
#include "custom_type.h"
#undef RPC_FUNC_HEADER

int add(MyType argPair) {
    return argPair.first + argPair.second;
}

int minus(int arg1, int arg2) {
    return arg1 - arg2;
}
