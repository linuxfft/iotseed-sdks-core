//
// Created by gubin on 17-12-5.
//

#include "jsonrpc.h"

#include <string>
#include <map>

#include <vector>
#include <list>
#include <ostream>

#include "glbtypes.h"
#include "json/json.hpp"


using namespace std;


// for convenience
using nlohmann::json;



class _iotseed_rpc_method_t{
private:
    iotseed_rpc_handler_t                       f_name;
    void                                        *user_data;
    const char                                  *method;
public:
    explicit  _iotseed_rpc_method_t(const char *_method, iotseed_rpc_handler_t f, void *_user_data): method(_method), f_name(f), user_data(_user_data){};

    const char* get_method_enum(){
        return this->method;
    }

    void run_rpc_handler(JSONRPCRequest* request){
        this->f_name(request, this->user_data);
    }
};

typedef map<std::string,_iotseed_rpc_method_t> IOTSEED_RPC_METHOD_MAP ;

static IOTSEED_RPC_METHOD_MAP g_IOTSeedRPCMethodsDict;


#if 0
class _iotseed_jsonrpc_request_class_t{
private:
    JSONRPCRequest                  obj;
    json                            request; 


public:

    explicit _iotseed_jsonrpc_request_class_t(const JSONRPCRequest req){
        this->obj = req;
        this->request = json{
                {"id",req.id},
                {"jsonrpc", req.jsonrpc},
                {"method", req.method},
                {"params", req.params},
        };
    }

    JSONRPCRequest* to_c_struct_obj(){
        return &(this->obj);
    }

};
#endif


ST_VOID dispatch_rpc_method(JSONRPCRequest* request){
    if(nullptr == request)
        return;
    g_IOTSeedRPCMethodsDict.at(request->method).run_rpc_handler(request);

}


static void to_json(json& j, const JSONRPCRequest& req){
    j[IOTSEED_RPC_KEY_ID] = req.id;
    j[IOTSEED_RPC_KEY_JSONRPC] = req.jsonrpc;
    j[IOTSEED_RPC_KEY_METHOD] = req.method;
    j[IOTSEED_RPC_KEY_PARAMS] = json::parse(req.params);
}

static void deserializer_jsonrpc_request_part1(const json& j, JSONRPCRequest* req){

    if(strlen(j.at(IOTSEED_RPC_KEY_METHOD).get<std::string>().c_str()) > IOTSEED_RPC_METHOD_MAX )
        return;

    memset(req->params, 0, sizeof(req->params));

    req->id = j.at(IOTSEED_RPC_KEY_ID).get<int>();
    req->jsonrpc = (char*)JSONRPC_VERSION;
    strncpy(req->method,j.at(IOTSEED_RPC_KEY_METHOD).get<std::string>().c_str(),strlen(j.at(IOTSEED_RPC_KEY_METHOD).get<std::string>().c_str()) + 1);

    std::string params = j[IOTSEED_RPC_KEY_PARAMS].dump().c_str();
    if(j[IOTSEED_RPC_KEY_PARAMS].is_array())
    {
        strncpy(req->params, params.c_str(), params.length());
    }
    else
    {
        strncpy(req->params, j.at(IOTSEED_RPC_KEY_PARAMS).get<std::string>().c_str(), strlen(j.at(IOTSEED_RPC_KEY_PARAMS).get<std::string>().c_str() + 1));
    }

}

ST_RET init_jsonrpc_request(JSONRPCRequest *req, const ST_UINT32 id,  ST_CHAR *method){
    if(nullptr == req){
        return SD_FAILURE;
    }
    if(strlen(method) > IOTSEED_RPC_METHOD_MAX){
        return SD_FAILURE;
    }
    memset(req,0, sizeof(JSONRPCRequest)); //clear all
    strncpy(req->method, method, strlen(method) + 1);
    req->id = id;
    req->jsonrpc = (char*)JSONRPC_VERSION; //const value
    return SD_SUCCESS;
}

ST_VOID serializer_jsonrpc_request(const JSONRPCRequest* req, ST_CHAR* cRet){
    JSONRPCRequest _r = *req;
    json _j = _r;
    strncpy(cRet, _j.dump().c_str(), strlen(_j.dump().c_str()) + 1);
}

ST_VOID deserializer_jsonrpc_request(const ST_CHAR* cSrc,JSONRPCRequest* req){
//    memset(req->params, 0, sizeof(req->params));
    json _j = json::parse(cSrc);
#ifdef IOTSEED_DEBUG
    // special iterator member functions for objects
    for (json::iterator it = _j.begin(); it != _j.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n";
    }
#endif
    deserializer_jsonrpc_request_part1(_j, req);
}

ST_RET init_jsonrpc_response(JSONRPCResponse* res, const ST_UINT32 id,IOTSEED_RPC_RESPONSE_TYPE type){
    if(nullptr == res)
        return SD_FAILURE;
    res->id = id;
    res->jsonrpc = (char*)JSONRPC_VERSION;
    res->type = type;
}

static void to_json(json& j, JSONRPCResponse& res){
    j[IOTSEED_RPC_KEY_ID] = res.id;
    j[IOTSEED_RPC_KEY_JSONRPC] = res.jsonrpc;
    switch (res.type){
        case RPC_RESP_P:{
            j[IOTSEED_RPC_KEY_RESULT] = res.result;
            break;
        }
        case RPC_RESP_N:{
            j[IOTSEED_RPC_KEY_ERROR] = res.result;
            break;
        }
        default:
            return;
    }
}

ST_VOID serializer_jsonrpc_response(const JSONRPCResponse* res, ST_CHAR* cRet, ST_BOOLEAN result_as_object){
//    JSONRPCResponse _r;
//    memcpy(&_r, res, sizeof(JSONRPCResponse));

    json _j = json::object();

//    _j.erase("type");
//    _j.erase("result");

    _j["id"] = res->id;
    _j["jsonrpc"] = "2.0";

    if(res->type == RPC_RESP_P)
    {

        _j[IOTSEED_RPC_KEY_RESULT] = json::parse(res->result);
    }
    else if(res->type == RPC_RESP_N)
    {

        _j[IOTSEED_RPC_KEY_ERROR] = json::parse(res->result);
    }

    strncpy(cRet, _j.dump().c_str(), strlen(_j.dump().c_str()) + 1);
}

ST_VOID insert_jsonrpc_param2(JSONRPCRequest *req, const ST_CHAR* param_name, const void *param_value, IOTSEED_VAL_TYPE type){
    switch (type){
        case R_VAL_OBJECT_T:

            break;
    }
}

ST_VOID set_jsonrpc_response_param(JSONRPCResponse *response, const void *param_value)
{
    json _j = json::object();

    _j = *(json*)param_value;
    strncpy(response->result, _j.dump().c_str(), strlen(_j.dump().c_str()) + 1);
}

ST_VOID insert_jsonrpc_param(JSONRPCRequest *req, const ST_CHAR* param_name, const void *param_value, IOTSEED_VAL_TYPE type){

    json _j;
    if(0 == strlen(req->params)){
        //init json array
        _j = json::array();
    } else{
        //already exists
        _j = json::parse(req->params);
    }
    json _param;
    switch (type){
        case R_VAL_INT8_T:
            _param[param_name] = *(ST_INT8*)param_value;
            break;
        case R_VAL_INT16_T:
            _param[param_name] = *(ST_INT16*)param_value;
            break;
        case R_VAL_INT32_T:
            _param[param_name] = *(ST_INT32*)param_value;
            break;
        case R_VAL_INT64_T:
            _param[param_name] = *(ST_INT64*)param_value;
            break;
        case R_VAL_UINT8_T:
            _param[param_name] = *(ST_UINT8*)param_value;
            break;
        case R_VAL_UINT16_T:
            _param[param_name] = *(ST_UINT16*)param_value;
            break;
        case R_VAL_UINT32_T:
            _param[param_name] = *(ST_UINT32*)param_value;
            break;
        case R_VAL_UINT64_T:
            _param[param_name] = *(ST_UINT64*)param_value;
            break;
        case R_VAL_FLOAT_T:
            _param[param_name] = *(ST_FLOAT*)param_value;
            break;
        case R_VAL_DOUBLE_T:
            _param[param_name] = *(ST_DOUBLE*)param_value;
            break;
        case R_VAL_BOOLEAN_T:
            _param[param_name] = *(ST_BOOLEAN*)param_value == SD_TRUE;
            break;
        case R_VAL_STRING_T:
            _param[param_name] = string((char*)param_value);
            break;
        case R_VAL_OBJECT_T:
            _param[param_name] = *(json*)param_value;
            break;
        default:
#ifdef IOTSEED_DEBUG
            fprintf(stderr,"type not supported\n");
#endif
            break;

    }
    _j.push_back(_param);
    if(strlen(_j.dump().c_str()) > IOTSEED_RPC_PARAMS_MAX)
        return;
    memcpy((void*)req->params,_j.dump().c_str(),strlen(_j.dump().c_str()) + 1);
}

ST_RET get_jsonrpc_param(const ST_CHAR *params,const ST_UINT32 index, char *name, void *value, IOTSEED_VAL_TYPE type){
    json _j;
    if(0 == strlen(params))
        return SD_FAILURE;
    try{
         _j = json::parse(params);
    }catch (...){
        return SD_FAILURE;
    }
    if(index > _j.size()){
#ifdef IOTSEED_DEBUG
        fprintf(stderr,"index valid:%u\n",(unsigned int)_j.size());
#endif
        return SD_FAILURE;
    }
    json o = _j[index - 1];
//    if(o.is_array()){
//        //
//    }
//    if(o.is_structured()){
//        //
//    }
//    if(o.is_primitive())
    // special iterator member functions for objects
    for (json::iterator it = o.begin(); it != o.end(); ++it) {
        strncpy(name, it.key().c_str(), strlen(it.key().c_str()) + 1);
        switch (type){
            case R_VAL_INT8_T:
                *(ST_INT8*)value = it.value();
                break;
            case R_VAL_INT16_T:
                *(ST_INT16*)value = it.value();
                break;
            case R_VAL_INT32_T:{
                *(ST_INT32*)value = it.value();
                break;
            }
            case R_VAL_INT64_T:
                 *(ST_INT64*)value  = it.value();
                break;
            case R_VAL_UINT8_T:
                *(ST_UINT8*)value  = it.value();
                break;
            case R_VAL_UINT16_T:
                *(ST_UINT16*)value = it.value();
                break;
            case R_VAL_UINT32_T:
                *(ST_UINT32*)value = it.value();
                break;
            case R_VAL_UINT64_T:
                *(ST_UINT64*)value = it.value();
                break;
            case R_VAL_FLOAT_T:
                *(ST_FLOAT*)value = it.value();
                break;
            case R_VAL_DOUBLE_T:
                *(ST_DOUBLE*)value = it.value();
                break;
            case R_VAL_BOOLEAN_T:
                *(ST_BOOLEAN*)value = it.value().get<bool>()? SD_TRUE : SD_FALSE;
                break;
            case R_VAL_STRING_T:
                strncpy((char*)value, it.value().get<std::string>().c_str(),strlen(it.value().get<std::string>().c_str()) + 1);
                break;
            default:
#ifdef IOTSEED_DEBUG
                fprintf(stderr,"type not supported\n");
#endif
                return SD_FAILURE;
        }
    }

    return SD_SUCCESS;
}


ST_RET registry_iotseed_rpc_method(const char* _method, iotseed_rpc_handler_t f, void *user_data){
    g_IOTSeedRPCMethodsDict.emplace(_method, _iotseed_rpc_method_t(_method,f, user_data));
	return SD_SUCCESS;
}
