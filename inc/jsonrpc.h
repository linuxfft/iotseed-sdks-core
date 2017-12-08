//
// Created by gubin on 17-12-5.
//

#ifndef IOTSEED_SDKS_CORE_JSONRPC_H
#define IOTSEED_SDKS_CORE_JSONRPC_H


#ifdef __cplusplus
extern "C" {
#endif

#ifndef JSONRPC_VERSION
#define JSONRPC_VERSION "2.0"
#endif


#include "sysinc.h"

#include "glbtypes.h"

#include "recipe.h"


typedef void (*iotseed_rpc_handler_t)(void* params, void* user_data);


typedef struct _JSONRPCRequest_common_struct_t {

    ST_UINT32               id;
    const ST_CHAR           *jsonrpc; // always = "2.0"
    const ST_CHAR           *method;
    const ST_CHAR           *params;
}JSONRPCRequest;


typedef struct _JSONRPCResponse_commonstruct_t {
    ST_UINT32               id;
    const ST_CHAR           *jsonrpc; // always = "2.0"
}JSONRPCResponse;


#define METHOD_SETRECIPE_STR    "setRecipe"
#define METHOD_ACTIVERECIPE_STR "activeRecipe"

typedef enum _enum_iotseed_rpc_method_t {
    METHOD_SETRECIPE = 1,
    METHOD_ACTIVERECIPE = 2
}IOTSEED_RPC_METHOD;


/*!
 *
 * @param _method : 方法的类型
 * @param f ： 注册的回调函数指针
 * @param user_data: 需要传递的用户数据
 * @return
 */
ST_RET registry_iotseed_rpc_method(IOTSEED_RPC_METHOD _method, iotseed_rpc_handler_t f, void *user_data);

ST_VOID dispatch_rpc_method(JSONRPCRequest* request);


ST_VOID create_jsonrpc_params(const ST_CHAR *params);

ST_VOID insert_jsonrpc_param(const ST_CHAR *params, const ST_CHAR* param_name, const void *param_value, IOTSEED_VAL_TYPE type);

JSONRPCRequest create_jsonrpc_request(const ST_UINT32 id, const ST_CHAR *method, const ST_CHAR *params);

/*!
 *
 * @param params
 * @param index 索引从1开始的索引号
 * @param name
 * @param value
 * @return
 */
ST_RET get_jsonrpc_param(const ST_CHAR *params,const ST_UINT32 index, char *name, void *value, IOTSEED_VAL_TYPE type);

#ifdef __cplusplus
}
#endif


#endif //IOTSEED_SDKS_CORE_JSONRPC_H
