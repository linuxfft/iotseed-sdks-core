//
// Created by gubin on 17-12-5.
//

#ifndef IOTSEED_SDKS_CORE_RECIPE_H
#define IOTSEED_SDKS_CORE_RECIPE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "sysinc.h"

#include "glbtypes.h"

#include "log.h"


typedef struct _iotseed_recipe_param_struct_t {
    ST_INT32    index;
    const ST_CHAR     *name;
    const ST_CHAR     *unit;
    ST_VOID     *value;
}IOTSEED_RECIPE_PARAM;

typedef struct _iotseed_recipe_struct_t {
    ST_INT32                LastActivedTimeStamp; //上次激活时间戳
    ST_INT32                Index; //工艺组索引
    IOTSEED_RECIPE_PARAM    *Params;
}IOTSEED_RECIPE;


ST_RET init_device_recipes(const char* client_id);


IOTSEED_RECIPE* create_recipe(ST_VOID);


//激活工艺组
ST_RET active_recipe(const ST_INT32 actived_group);

IOTSEED_RECIPE* get_actived_recipe(ST_VOID);

ST_RET write_device_recipes(const IOTSEED_LOGGER *logger);

IOTSEED_RECIPE_PARAM create_recipe_param(const IOTSEED_RECIPE* recipe, const char *name, const char* unit, const void *value, const IOTSEED_VAL_TYPE type);


#ifdef __cplusplus
}
#endif


#endif //IOTSEED_SDKS_CORE_RECIPE_H_H
