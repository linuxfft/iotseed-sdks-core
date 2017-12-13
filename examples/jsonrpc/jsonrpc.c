//
// Created by gubin on 17-12-7.
//

#include "glbinc.h"

#include "jsonrpc.h"

#include "recipe.h"

#include <unistd.h>

#include <signal.h>

static int end = 0;

const char *CLIENT_ID = "23f901e0-ccc3-11e7-bf1a-59e9355b22c6";


const int RECIPE_PARAM_LEN = 5;


const int RECIPE_LEN = 5;


static void signal_handler(int sig_num) {
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    end = 1;
}

void setrecipe(void *req, void *data){
    printf("setrecipe\n");
    JSONRPCRequest *request = (JSONRPCRequest*) req;
    printf("%d\n",request->id);
    printf("%s\n",request->jsonrpc);
    printf("%s\n",request->params);
    int index;
    char name[100]= {};
    char value[100] = {};
    ST_BOOLEAN b;
    get_jsonrpc_param(request->params,1, name,&index, R_VAL_INT32_T);
    get_jsonrpc_param(request->params,2, name,&b, R_VAL_BOOLEAN_T);
    get_jsonrpc_param(request->params,3, name,&value, R_VAL_STRING_T);
    if(NULL != data)
        printf("%s\n",(char*)data);
}

void activerecipe(void *req, void *data){
    printf("activerecipe\n");
    JSONRPCRequest *request = (JSONRPCRequest*) req;
    printf("%d\n",request->id);
    printf("%s\n",request->jsonrpc);
    printf("%s\n",request->params);
    int index;
    char name[100]= {};
    char value[100] = {};
    ST_BOOLEAN b;
    get_jsonrpc_param(request->params,1, name,&index, R_VAL_INT32_T);
    get_jsonrpc_param(request->params,2, name,&b, R_VAL_BOOLEAN_T);
    get_jsonrpc_param(request->params,3, name,&value, R_VAL_STRING_T);
    if(NULL != data)
        printf("%s\n",(char*)data);
}



int main(){
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    init_device_recipes(CLIENT_ID);

    for(int i=0; i < RECIPE_LEN; ++i){
        IOTSEED_RECIPE* r  = create_recipe();
        for(int j=0; j < RECIPE_PARAM_LEN; ++j){
            double value = 14;
            create_recipe_param(r, "加速度", "m/s/s", &value, R_VAL_DOUBLE_T);
        }
    }

    registry_iotseed_recipe_rpc_method(RPC_METHOD_SETRECIPE, setrecipe, NULL);

    registry_iotseed_recipe_rpc_method(RPC_METHOD_ACTIVERECIPE, activerecipe, NULL);


//    int i = 500;
//
//    ST_BOOLEAN x = SD_TRUE;
//
//    const char* dummy = "dummy";

    JSONRPCRequest r1;
    init_jsonrpc_request(&r1, 11, RPC_METHOD_SETRECIPE_STR);

    seriral_device_recipes(&r1);

//    insert_jsonrpc_param(&r1, "Index", &i, R_VAL_INT32_T);
//    insert_jsonrpc_param(&r1, "xx", &x, R_VAL_BOOLEAN_T);
//    insert_jsonrpc_param(&r1, "ee", dummy, R_VAL_STRING_T);
//
//    JSONRPCRequest r2;
//    init_jsonrpc_request(&r2, 13, RPC_METHOD_ACTIVERECIPE_STR);

    char req[1024] = {};

    //第一组参数验证
    serializer_jsonrpc_request(&r1, req);

    printf("%s\n",req);

//    JSONRPCRequest r3;
//
//    deserializer_jsonrpc_request(req, &r3);
//
//    dispatch_rpc_method(&r3);
//
//    //第二组参数验证;
//    serializer_jsonrpc_request(&r2, req);
//
//    printf("%s\n",req);
//
//    JSONRPCRequest r4;
//
//    deserializer_jsonrpc_request(req, &r4);
//
//    dispatch_rpc_method(&r4);



//
//    while(!end){
//        sleep(1);
//    }

}

