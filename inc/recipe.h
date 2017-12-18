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
    ST_INT32 LastActivedTimeStamp;
    ST_INT32 Index;
    IOTSEED_RECIPE_PARAM *Params;
} IOTSEED_RECIPE;

typedef struct {
    ST_INT32 Index;
    ST_VOID *params;
} IOTSEED_RPC_RECIPE;

typedef struct {
    IOTSEED_RPC_RECIPE Recipes[MAX_RPC_RECIPES];
    ST_INT32 size;
} IOTSEED_RPC_SET_RECIPE_PARAM;

typedef struct {
    ST_INT32 code;                                 
    IOTSEED_RPC_RECIPE Recipes[MAX_RPC_RECIPES];
    ST_INT32 size;
} IOTSEED_RPC_SET_RECIPE_RESULT;

typedef struct {
    ST_INT32 Index; 
} IOTSEED_RPC_ACTIVE_RECIPE_PARAM;

typedef struct
{
    ST_INT32        code;           
    ST_INT32        ActivedGroup;   
    IOTSEED_RECIPE  Actived;        
} IOTSEED_RPC_ACTIVE_RECIPE_RESULT;

typedef struct
{
    char            Message[MAX_MESSAGE_LEN];  
    ST_VOID         *Recipes;                   
} IOTSEED_RPC_ERROR_MESSAGE_BODY;

typedef struct
{
    ST_INT32                                code;       
    IOTSEED_RPC_ERROR_MESSAGE_BODY          message;  
} IOTSEED_RPC_ERROR_MESSAGE;

IOTSEED_API ST_RET init_device_recipes(const char* client_id);


IOTSEED_API IOTSEED_RECIPE* create_recipe(ST_VOID);


IOTSEED_API ST_RET active_recipe(const ST_INT32 actived_group);

IOTSEED_API IOTSEED_RECIPE* get_actived_recipe(ST_VOID);

IOTSEED_API ST_RET write_device_recipes(const IOTSEED_LOGGER *logger);

IOTSEED_API IOTSEED_RECIPE_PARAM create_recipe_param(const IOTSEED_RECIPE* recipe, const char *name, const char* unit, const void *value, const IOTSEED_VAL_TYPE type);

IOTSEED_API ST_RET registry_iotseed_recipe_rpc_method(IOTSEED_RPC_METHOD _method, iotseed_rpc_handler_t f, void *user_data);

IOTSEED_API IOTSEED_RECIPE *iotseed_set_recipe(ST_INT32 *recipe_index, const void *params);

IOTSEED_API ST_VOID show_all_recipes(ST_VOID);

IOTSEED_API IOTSEED_RECIPE *handler_set_recipe_params(IOTSEED_RPC_SET_RECIPE_PARAM *param, const char *raw_str);

IOTSEED_API ST_RET serializer_set_recipe_result(char *raw_str, const IOTSEED_RPC_SET_RECIPE_RESULT *result, unsigned int response_id);

IOTSEED_API ST_RET handler_active_recipe_params(IOTSEED_RPC_ACTIVE_RECIPE_PARAM *param, const char *raw_str);

IOTSEED_API ST_RET serializer_active_recipe_result(char *raw_str, const IOTSEED_RPC_ACTIVE_RECIPE_RESULT *result, unsigned int response_id);

IOTSEED_API ST_RET serializer_error(char *raw_str, const IOTSEED_RPC_ERROR_MESSAGE *error, unsigned int response_id);

#ifdef __cplusplus
}
#endif


#endif //IOTSEED_SDKS_CORE_RECIPE_H_H
