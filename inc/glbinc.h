//
// Created by gubin on 17-12-9.
//

#ifndef IOTSEED_SDKS_CORE_GLBINC_H
#define IOTSEED_SDKS_CORE_GLBINC_H


#ifdef __cplusplus
extern "C" {
#endif

#include "sysinc.h"

#include "glbtypes.h"

#define RPC_METHOD_SETRECIPE_STR    "setRecipe"
#define RPC_METHOD_ACTIVERECIPE_STR "activeRecipe"



typedef enum _enum_iotseed_rpc_method_t {
    RPC_METHOD_SETRECIPE = 1,
    RPC_METHOD_ACTIVERECIPE = 2
}IOTSEED_RPC_METHOD;

#ifdef __cplusplus
}
#endif

#endif //IOTSEED_SDKS_CORE_GLBINC_H
