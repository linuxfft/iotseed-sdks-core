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

    const ST_CHAR params[1024] = {};


    registry_iotseed_recipe_rpc_method(RPC_METHOD_SETRECIPE, setrecipe, NULL);

    registry_iotseed_recipe_rpc_method(RPC_METHOD_ACTIVERECIPE, activerecipe, NULL);


    int i = 500;

    ST_BOOLEAN x = SD_TRUE;

    const char* dummy = "dummy";

    create_jsonrpc_params(params);

    insert_jsonrpc_param(params, "Index", &i, R_VAL_INT64_T);
    insert_jsonrpc_param(params, "xx", &x, R_VAL_BOOLEAN_T);
    insert_jsonrpc_param(params, "ee", dummy, R_VAL_STRING_T);

    JSONRPCRequest r1 = create_jsonrpc_request(11, RPC_METHOD_SETRECIPE_STR, params);

    JSONRPCRequest r2 = create_jsonrpc_request(13, RPC_METHOD_ACTIVERECIPE_STR, params);

    char req[1024] = {};

    deserializer_jsonrpc_request(&r1, req);

    printf("%s\n",req);

    dispatch_rpc_method(&r1);

    deserializer_jsonrpc_request(&r1, req);

    printf("%s\n",req);

    dispatch_rpc_method(&r2);



//
//    while(!end){
//        sleep(1);
//    }

}

