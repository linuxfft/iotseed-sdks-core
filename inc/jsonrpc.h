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

#ifndef IOTSEED_RPC_PARAMS_MAX
#define IOTSEED_RPC_PARAMS_MAX  4096
#endif

#ifndef IOTSEED_RPC_METHOD_MAX
#define IOTSEED_RPC_METHOD_MAX  32
#endif

#ifndef IOTSEED_RPC_RESULT_MAX
#define IOTSEED_RPC_RESULT_MAX  4096
#endif

#define IOTSEED_RPC_KEY_ID              "id"
#define IOTSEED_RPC_KEY_JSONRPC         "jsonrpc"
#define IOTSEED_RPC_KEY_METHOD          "method"
#define IOTSEED_RPC_KEY_PARAMS          "params"
#define IOTSEED_RPC_KEY_RESULT          "result"
#define IOTSEED_RPC_KEY_ERROR           "error"

typedef void (*iotseed_rpc_handler_t)(void* params, void* user_data);

typedef enum _enum_iotseed_rpc_response_t {
    RPC_RESP_P = 1, //positive
    RPC_RESP_N = 2 //negative
}IOTSEED_RPC_RESPONSE_TYPE;



typedef struct _JSONRPCRequest_common_struct_t {
    ST_UINT32               id;
    ST_CHAR           *jsonrpc; // always = "2.0"
    ST_CHAR           method[IOTSEED_RPC_METHOD_MAX];
    ST_CHAR           params[IOTSEED_RPC_PARAMS_MAX];
}JSONRPCRequest;


typedef struct _JSONRPCResponse_commonstruct_t {
    ST_UINT32                   id;
    ST_CHAR                     *jsonrpc; // always = "2.0"
    IOTSEED_RPC_RESPONSE_TYPE   type;
    ST_CHAR                     result[IOTSEED_RPC_RESULT_MAX];
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

ST_VOID insert_jsonrpc_param(JSONRPCRequest *req, const ST_CHAR* param_name, const void *param_value, IOTSEED_VAL_TYPE type);

ST_RET init_jsonrpc_request(JSONRPCRequest *req, const ST_UINT32 id,  ST_CHAR *method);

ST_VOID serializer_jsonrpc_request(const JSONRPCRequest* req, ST_CHAR* cRet);

ST_VOID deserializer_jsonrpc_request(const ST_CHAR* cSrc,JSONRPCRequest* req);

/*!
 *
 * @param params
 * @param index 索引从1开始的索引号
 * @param name
 * @param value
 * @return
 */
ST_RET get_jsonrpc_param(const ST_CHAR *params,const ST_UINT32 index, char *name, void *value, IOTSEED_VAL_TYPE type);

ST_RET init_jsonrpc_response(JSONRPCResponse* res, const ST_UINT32 id,IOTSEED_RPC_RESPONSE_TYPE type);

ST_VOID serializer_jsonrpc_response(const JSONRPCResponse* res, ST_CHAR* cRet, ST_BOOLEAN result_as_object);

ST_VOID set_jsonrpc_response_param(JSONRPCResponse *response, const void *param_value);

#ifdef __cplusplus
}
#endif


#endif //IOTSEED_SDKS_CORE_JSONRPC_H
