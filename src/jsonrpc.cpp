//
// Created by gubin on 17-12-5.
//

#include "jsonrpc.h"

#include <string>
#include <map>

#include <vector>
#include <list>
#include <ostream>


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


class _iotseed_jsonrpc_request_class_t{
private:
    JSONRPCRequest                  obj;
    json                            request; //请求是一个json对象直接进行序列化


public:
    explicit _iotseed_jsonrpc_request_class_t(const char *req_msg){
        this->request = json::parse(req_msg);//因为这是jsonRPC payload 进行检查
        this->obj = JSONRPCRequest{this->request.at("id").get<ST_UINT32>(),
                                   this->request.at("jsonrpc").get<std::string>().c_str(),
                                   this->request.at("method").get<std::string>().c_str(),
                                   this->request.at("params").get<std::string>().c_str(),
        };
    }

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


ST_VOID dispatch_rpc_method(JSONRPCRequest* request){
    g_IOTSeedRPCMethodsDict.at(request->method).run_rpc_handler(request);

}


static void to_json(json& j, JSONRPCRequest& req){
    j["id"] = req.id;
    j["jsonrpc"] = req.jsonrpc;
    j["method"] = req.method;
    j["params"] = req.params;
}

JSONRPCRequest create_jsonrpc_request(const ST_UINT32 id, const ST_CHAR *method, const ST_CHAR *params){
    return {id, JSONRPC_VERSION, method, params};
}

ST_VOID deserializer_jsonrpc_request(const JSONRPCRequest* req, ST_CHAR* cRet){
    JSONRPCRequest _r = *req;
    json _j = _r;
    strncpy(cRet, _j.dump().c_str(), strlen(_j.dump().c_str()) + 1);
}

JSONRPCResponse create_jsonrpc_response(const ST_UINT32 id, const ST_CHAR *result, IOTSEED_RPC_RESPONSE_TYPE type){
    return {id, JSONRPC_VERSION, type, result};
}

static void to_json(json& j, JSONRPCResponse& res){
    j["id"] = res.id;
    j["jsonrpc"] = res.jsonrpc;
    switch (res.type){
        case RPC_RESP_P:{
            j["result"] = res.result;
            break;
        }
        case RPC_RESP_N:{
            j["message"] = res.result;
            break;
        }
        default:
            return;
    }
}

ST_VOID deserializer_jsonrpc_response(const JSONRPCResponse* res, ST_CHAR* cRet){
    JSONRPCResponse _r = *res;
    json _j = _r;
    strncpy(cRet, _j.dump().c_str(), strlen(_j.dump().c_str()) + 1);
}

ST_VOID insert_jsonrpc_param(const ST_CHAR *params, const ST_CHAR* param_name, const void *param_value, IOTSEED_VAL_TYPE type){
    json _j = json::parse(params);
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
        default:
#ifdef IOTSEED_DEBUG
            fprintf(stderr,"不支持此方法\n");
#endif
            break;

    }
    _j.push_back(_param);
    memcpy((void*)params,_j.dump().c_str(),strlen(_j.dump().c_str()));
}


ST_VOID create_jsonrpc_params(const ST_CHAR *params){
    json _j = json::array();
    const char* _c = _j.dump().c_str();
    memcpy((void*)params,_c,strlen(_c));
}

ST_RET get_jsonrpc_param(const ST_CHAR *params,const ST_UINT32 index, char *name, void *value, IOTSEED_VAL_TYPE type){
    json _j = json::parse(params);
    if(index > _j.size()){
#ifdef IOTSEED_DEBUG
        fprintf(stderr,"获取参数索引超限:%u\n",(unsigned int)_j.size());
#endif
        return SD_FAILURE;
    }
    json o = _j[index - 1];
    if(o.is_array()){
        //
    }
    if(o.is_structured()){
        //
    }
    if(o.is_primitive())
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
                fprintf(stderr,"不支持此方法\n");
#endif
                return SD_FAILURE;
        }
    }

    return SD_SUCCESS;
}


ST_RET registry_iotseed_rpc_method(const char* _method, iotseed_rpc_handler_t f, void *user_data){
    g_IOTSeedRPCMethodsDict.emplace(_method, _iotseed_rpc_method_t(_method,f, user_data));
}
