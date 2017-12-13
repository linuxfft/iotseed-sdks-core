//
// Created by gubin on 17-12-5.
//

#ifndef IOTSEED_SDKS_CORE_RECIPE_H
#define IOTSEED_SDKS_CORE_RECIPE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "glbinc.h"

#include "log.h"

#include "jsonrpc.h"

#define MAX_MESSAGE_LEN     256
#define MAX_RPC_PARAMS      256
#define MAX_RPC_RECIPES     256

typedef struct _iotseed_recipe_param_struct_t {
    ST_INT32 index;
    ST_CHAR *name;
    ST_CHAR *unit;
    ST_VOID *value;
} IOTSEED_RECIPE_PARAM;

typedef struct _iotseed_recipe_struct_t {
    ST_INT32 LastActivedTimeStamp; //上次激活时间戳
    ST_INT32 Index; //工艺组索引
    IOTSEED_RECIPE_PARAM *Params;
} IOTSEED_RECIPE;

typedef struct {
    ST_INT32 Index; //工艺组索引
    ST_VOID *params;
} IOTSEED_RPC_RECIPE;

// 工艺下发请求参数
typedef struct {
    IOTSEED_RPC_RECIPE Recipes[MAX_RPC_RECIPES];   // 多组工艺
    ST_INT32 size;
} IOTSEED_RPC_SET_RECIPE_PARAM;

// 工艺下发肯定响应结果
typedef struct {
    ST_INT32 code;                                  // 肯定结果代码为0
    IOTSEED_RPC_RECIPE Recipes[MAX_RPC_RECIPES];    // 返回创建或修改的工艺列表
    ST_INT32 size;
} IOTSEED_RPC_SET_RECIPE_RESULT;

// 工艺激活请求参数
typedef struct {
    ST_INT32 Index;      // 要激活的工艺组索引
} IOTSEED_RPC_ACTIVE_RECIPE_PARAM;

// 工艺激活肯定响应结果
typedef struct
{
    ST_INT32        code;           // 肯定结果代码为0
    ST_INT32        ActivedGroup;   // 激活的工艺组索引
    IOTSEED_RECIPE  Actived;        // 被激活的工艺组数据
} IOTSEED_RPC_ACTIVE_RECIPE_RESULT;

typedef struct
{
    char            Message[MAX_MESSAGE_LEN];   // 可读的错误消息
    ST_VOID_PTR     Recipes;                    // 整个设备的工艺
} IOTSEED_RPC_ERROR_MESSAGE_BODY;

// 工艺否定响应的错误消息
typedef struct
{
    ST_INT32                                code;           // 错误码
    IOTSEED_RPC_ERROR_MESSAGE_BODY          message;        // 错误消息
} IOTSEED_RPC_ERROR_MESSAGE;

ST_RET init_device_recipes(const char* client_id);


IOTSEED_RECIPE* create_recipe(ST_VOID);


//激活工艺组
ST_RET active_recipe(const ST_INT32 actived_group);

IOTSEED_RECIPE* get_actived_recipe(ST_VOID);

ST_RET write_device_recipes(const IOTSEED_LOGGER *logger);

IOTSEED_RECIPE_PARAM create_recipe_param(const IOTSEED_RECIPE* recipe, const char *name, const char* unit, const void *value, const IOTSEED_VAL_TYPE type);

ST_RET registry_iotseed_recipe_rpc_method(IOTSEED_RPC_METHOD _method, iotseed_rpc_handler_t f, void *user_data);

IOTSEED_RECIPE *iotseed_set_recipe(ST_INT32 *recipe_index, const void *params);

ST_VOID show_all_recipes();

// 工艺下发请求参数(反序列化)
IOTSEED_RECIPE *handler_set_recipe_params(IOTSEED_RPC_SET_RECIPE_PARAM *param, const char *raw_str);

// 工艺下发肯定响应结果(序列化)
ST_RET serializer_set_recipe_result(char *raw_str, const IOTSEED_RPC_SET_RECIPE_RESULT *result, unsigned int response_id);

// 工艺激活请求参数(反序列化)
ST_RET handler_active_recipe_params(IOTSEED_RPC_ACTIVE_RECIPE_PARAM *param, const char *raw_str);

// 工艺激活肯定响应结果(序列化)
ST_RET serializer_active_recipe_result(char *raw_str, const IOTSEED_RPC_ACTIVE_RECIPE_RESULT *result, unsigned int response_id);

// 工艺否定响应消息(序列化)
ST_RET serializer_error(char *raw_str, const IOTSEED_RPC_ERROR_MESSAGE *error, unsigned int response_id);

//ST_RET seriral_device_recipes(JSONRPCRequest *req);

#ifdef __cplusplus
}
#endif


#endif //IOTSEED_SDKS_CORE_RECIPE_H_H
