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


#include "glbinc.h"


typedef void (*iotseed_rpc_handler_t)(void* params, void* user_data);

typedef enum _enum_iotseed_rpc_response_t {
    RPC_RESP_P = 1, //positive
    RPC_RESP_N = 2 //negative
}IOTSEED_RPC_RESPONSE_TYPE;



typedef struct _JSONRPCRequest_common_struct_t {
    ST_UINT32               id;
    const ST_CHAR           *jsonrpc; // always = "2.0"
    const ST_CHAR           *method;
    const ST_CHAR           *params;
}JSONRPCRequest;


typedef struct _JSONRPCResponse_commonstruct_t {
    ST_UINT32                   id;
    const ST_CHAR               *jsonrpc; // always = "2.0"
    IOTSEED_RPC_RESPONSE_TYPE   type;
    const ST_CHAR               *result;
}JSONRPCResponse;



/*!
 *
 * @param _method : 方法的类型
 * @param f ： 注册的回调函数指针
 * @param user_data: 需要传递的用户数据
 * @return
 */
ST_RET registry_iotseed_rpc_method(const char* _method, iotseed_rpc_handler_t f, void *user_data);

ST_VOID dispatch_rpc_method(JSONRPCRequest* request);

ST_VOID create_jsonrpc_params(const ST_CHAR *params);

ST_VOID insert_jsonrpc_param(const ST_CHAR *params, const ST_CHAR* param_name, const void *param_value, IOTSEED_VAL_TYPE type);

JSONRPCRequest create_jsonrpc_request(const ST_UINT32 id, const ST_CHAR *method, const ST_CHAR *params);

ST_VOID deserializer_jsonrpc_request(const JSONRPCRequest* req, ST_CHAR* cRet);

/*!
 *
 * @param params
 * @param index 索引从1开始的索引号
 * @param name
 * @param value
 * @return
 */
ST_RET get_jsonrpc_param(const ST_CHAR *params,const ST_UINT32 index, char *name, void *value, IOTSEED_VAL_TYPE type);

JSONRPCResponse create_jsonrpc_response(const ST_UINT32 id, const ST_CHAR *result, IOTSEED_RPC_RESPONSE_TYPE type);

ST_VOID deserializer_jsonrpc_response(const JSONRPCResponse* res, ST_CHAR* cRet);

#ifdef __cplusplus
}
#endif


#endif //IOTSEED_SDKS_CORE_JSONRPC_H
