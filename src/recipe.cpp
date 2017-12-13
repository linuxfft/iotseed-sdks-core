//
// Created by gubin on 17-12-5.
//

#include "recipe.h"

#include "time_utils.h"

#include <iostream>

#include <list>

#include <map>
#include <recipe.h>

#include "json/json.hpp"


using namespace std;


// for convenience
using nlohmann::json;


class _iotseed_recipe_param_class_t {
public:
    ST_INT32    index;
    string      name;
    string      unit;
    string      _value;
    void        *orig_data;

public:
    explicit _iotseed_recipe_param_class_t(const ST_CHAR *_name, const ST_CHAR *_unit){
        this->name = _name;
        this->unit = _unit;
    }

    void value_to_string(const void *value, const IOTSEED_VAL_TYPE type){
        switch (type){
            case R_VAL_INT8_T:{
                this->orig_data = new(ST_INT8);
                *(ST_INT8*)this->orig_data = *(ST_INT8*)value;
                this->_value  = std::to_string(*(ST_INT8*)value);
                break;
            }
            case R_VAL_INT16_T:{
                this->orig_data = new(ST_INT16);
                *(ST_INT16*)this->orig_data = *(ST_INT16*)value;
                this->_value  = std::to_string(*(ST_INT16*)value);
                break;
            }
            case R_VAL_INT32_T:{
                this->orig_data = new(ST_INT32);
                *(ST_INT32*)this->orig_data = *(ST_INT32*)value;
                this->_value  = std::to_string(*(ST_INT32*)value);
                break;
            }

            case R_VAL_INT64_T:{
                this->orig_data = new(ST_INT64);
                *(ST_INT64*)this->orig_data = *(ST_INT64*)value;
                this->_value  = std::to_string(*(ST_INT64*)value);
                break;
            }

            case R_VAL_UINT8_T:{
                this->orig_data = new(ST_UINT8);
                *(ST_UINT8*)this->orig_data = *(ST_UINT8*)value;
                this->_value  = std::to_string(*(ST_UINT8*)value);
                break;
            }

            case R_VAL_UINT16_T:{
                this->orig_data = new(ST_UINT16);
                *(ST_UINT16*)this->orig_data = *(ST_UINT16*)value;
                this->_value  = std::to_string(*(ST_UINT16*)value);
                break;
            }

            case R_VAL_UINT32_T:{
                this->orig_data = new(ST_UINT32);
                *(ST_UINT32*)this->orig_data = *(ST_UINT32*)value;
                this->_value  = std::to_string(*(ST_UINT32*)value);
                break;
            }

            case R_VAL_UINT64_T:{
                this->orig_data = new(ST_UINT64);
                *(ST_UINT64*)this->orig_data = *(ST_UINT64*)value;
                this->_value  = std::to_string(*(ST_UINT64*)value);
                break;
            }

            case R_VAL_FLOAT_T:{
                this->orig_data = new(ST_FLOAT);
                *(ST_FLOAT*)this->orig_data = *(ST_FLOAT*)value;
                this->_value  = std::to_string(*(ST_FLOAT*)value);
                break;
            }

            case R_VAL_DOUBLE_T:{
                this->orig_data = new(ST_DOUBLE);
                *(ST_DOUBLE*)this->orig_data = *(ST_DOUBLE*)value;
                this->_value  = std::to_string(*(ST_DOUBLE*)value);
                break;
            }

            case R_VAL_BOOLEAN_T:{
                this->orig_data = new(ST_BOOLEAN);
                *(ST_BOOLEAN*)this->orig_data = *(ST_BOOLEAN*)value;
                ST_BOOLEAN _t = *(ST_BOOLEAN*)value;
                if(_t == SD_TRUE){
                    this->_value  = "true";
                } else{
                    this->_value  = "false";
                }
                break;
            }

            case R_VAL_STRING_T:{
                this->_value  = std::string((ST_CHAR*)value);
                this->orig_data = (void*)(this->_value.c_str());
                break;
            }
            default:
#ifdef IOTSEED_DEBUG
                fprintf(stderr, "数据类型未定义\n");
#endif
                break;
        }
    }

    IOTSEED_RECIPE_PARAM to_c_recipe_param_struct(){
        IOTSEED_RECIPE_PARAM ret = {this->index, (char *)this->name.c_str(), (char *)this->unit.c_str(), this->orig_data};
        return ret;
    }
};


void to_json(json& j, const _iotseed_recipe_param_class_t& p) {
    j = json{{"index", p.index},
             {"name", p.name},
             {"value", p._value},
             {"unit", p.unit}
    };
}


class _iotseed_recipe_class_t {

    IOTSEED_RECIPE                                  *obj;
public:
    map<ST_INT32,_iotseed_recipe_param_class_t>     Params;


    IOTSEED_RECIPE* to_c_recipe_struct_point(){
        return this->obj;
    }

    _iotseed_recipe_class_t(){
        this->obj = new IOTSEED_RECIPE();
    }

    ~_iotseed_recipe_class_t(){
        delete(this->obj);
    }

    void serialize(json& j){
        j["LastActivedTimeStamp"] = this->obj->LastActivedTimeStamp;
        j["Index"] = this->obj->Index;
        j["Params"] = json::array();
        for(auto i = this->Params.begin(); i != this->Params.end(); ++i){
            json _j = i->second;
            j["Params"].push_back(_j);
        }
    }

    void write_recipe(const IOTSEED_LOGGER* logger, IOTSEED_LOG_LEVEL level){
        json j;
        this->serialize(j);
        iotseed_write_log(logger,level,j.dump().c_str(),Recipe);
    }
};


class DEVICE_RECIPES {
public:
    string      ClientID;
    string      Type;
    ST_INT32    ActivedGroup; //当前激活的参数组
    map<ST_INT32, _iotseed_recipe_class_t> Recipes;

    explicit DEVICE_RECIPES(string client_id){
        this->ClientID = client_id;
        this->ActivedGroup = 0;
        this->Type = "recipe";// 类型为recipe或者log
    }

    void serialize(json& j){
        j["ClientID"] = this->ClientID;
        j["Type"] = this->Type;
        j["ActivedGroup"] = this->ActivedGroup;
        j["Recipes"] = json::array();
        for(auto i = this->Recipes.begin(); i != this->Recipes.end(); ++i){
            json _j;
            i->second.serialize(_j);
            j["Recipes"].push_back(_j);
        }
    }

    void write_recipes(const IOTSEED_LOGGER* logger, IOTSEED_LOG_LEVEL level){
        json j;
        this->serialize(j);
        iotseed_write_log(logger,level,j.dump().c_str(),Recipe);
    }

};


static DEVICE_RECIPES *g_DeviceRecipes = nullptr;


ST_RET init_device_recipes(const char* client_id){
    if(nullptr == g_DeviceRecipes){
        g_DeviceRecipes = new DEVICE_RECIPES(client_id);
    }
    return SD_SUCCESS;
}

ST_RET write_device_recipes(const IOTSEED_LOGGER *logger){
    if(nullptr != g_DeviceRecipes){
        g_DeviceRecipes->write_recipes(logger, Info);

        return SD_SUCCESS;
    }
    return SD_FAILURE;
}

ST_RET create_params(){
    if(nullptr != g_DeviceRecipes){
        json _j;
        g_DeviceRecipes->serialize(_j);
//        insert_jsonrpc_param((void*)&_j);

        return SD_SUCCESS;
    }
    return SD_FAILURE;
}



IOTSEED_RECIPE* create_recipe(ST_VOID){
    if(nullptr == g_DeviceRecipes){
#ifdef IOTSEED_DEBUG
        fprintf(stderr, "请先初始化设备定制组\n");
        return nullptr;
#endif
    }
    auto _class_ret = new _iotseed_recipe_class_t();
    auto _c_struct_ret = _class_ret->to_c_recipe_struct_point();
    _c_struct_ret->Index = (ST_INT32)g_DeviceRecipes->Recipes.size() +1;
    g_DeviceRecipes->Recipes.emplace(_c_struct_ret->Index, *_class_ret);

    return _c_struct_ret;
}

ST_VOID destroy_recipe(IOTSEED_RECIPE* recipe){
    if(nullptr == g_DeviceRecipes){
#ifdef IOTSEED_DEBUG
        fprintf(stderr, "请先初始化设备定制组\n");
        return;
#endif
    }
    if( recipe->Index > (ST_INT32)g_DeviceRecipes->Recipes.size()){
#ifdef IOTSEED_DEBUG
        fprintf(stderr, "索引超限\n");
        return;
#endif
    }
    g_DeviceRecipes->Recipes.erase(recipe->Index); //从map中将此对象移除
}


ST_RET write_recipe(IOTSEED_RECIPE* recipe, const IOTSEED_LOGGER *logger, IOTSEED_LOG_LEVEL level){
    if(nullptr != g_DeviceRecipes && g_DeviceRecipes->ActivedGroup){
        g_DeviceRecipes->Recipes[g_DeviceRecipes->ActivedGroup].write_recipe(logger,level);
    }
    return SD_FAILURE;
}


ST_RET active_recipe(const ST_INT32 actived_group){

    if(actived_group > (ST_INT32)g_DeviceRecipes->Recipes.size()){
#ifdef IOTSEED_DEBUG
        fprintf(stderr, "定义的激活组大于设备激活组长度\n");
#endif
        return SD_FAILURE;
    }

    if(nullptr == g_DeviceRecipes){
        return SD_FAILURE;
    }

    g_DeviceRecipes->ActivedGroup = actived_group;

    struct timeval tBegin;
    gettimeofday(&tBegin, nullptr);
    g_DeviceRecipes->Recipes[actived_group].to_c_recipe_struct_point()->LastActivedTimeStamp = (ST_INT32)tBegin.tv_sec;

    return SD_SUCCESS;

}

IOTSEED_RECIPE* get_actived_recipe(ST_VOID){
    if(nullptr != g_DeviceRecipes && g_DeviceRecipes->ActivedGroup){
        return g_DeviceRecipes->Recipes[g_DeviceRecipes->ActivedGroup].to_c_recipe_struct_point();
    }
    return nullptr;
}


IOTSEED_RECIPE *iotseed_set_recipe(ST_INT32 *recipe_index, const void *params)
{
    assert(g_DeviceRecipes != nullptr);

    IOTSEED_RECIPE *obj_recipe = NULL;
    json *j_params = (json *)params;

    if(recipe_index)
    {
        ST_INT32 index = *recipe_index;
        map<ST_INT32, _iotseed_recipe_class_t>::iterator it = g_DeviceRecipes->Recipes.find(index);
        if(it != g_DeviceRecipes->Recipes.end())
        {
            // 工艺存在，修改
            _iotseed_recipe_class_t recipe = g_DeviceRecipes->Recipes.at(index);
            recipe.Params.clear();

            obj_recipe = recipe.to_c_recipe_struct_point();
            for (json::iterator it_params = j_params->begin(); it_params != j_params->end(); ++it_params)
            {
                json p = *it_params;
                if(p["value"].is_boolean())
                {
                    bool v = p["value"].get<bool>();
                    create_recipe_param(obj_recipe, p["name"].get<std::string>().c_str(), p["unit"].get<std::string>().c_str(), &v, R_VAL_BOOLEAN_T);
                }
                else if(p["value"].is_number_float())
                {
                    double v = p["value"].get<double>();
                    create_recipe_param(obj_recipe, p["name"].get<std::string>().c_str(), p["unit"].get<std::string>().c_str(), &v, R_VAL_DOUBLE_T);
                }
                else if(p["value"].is_number_integer())
                {
                    int v = p["value"].get<int>();
                    create_recipe_param(obj_recipe, p["name"].get<std::string>().c_str(), p["unit"].get<std::string>().c_str(), &v, R_VAL_INT64_T);
                }
                else if(p["value"].is_number_unsigned())
                {
                    unsigned v = p["value"].get<unsigned>();
                    create_recipe_param(obj_recipe, p["name"].get<std::string>().c_str(), p["unit"].get<std::string>().c_str(), &v, R_VAL_UINT64_T);
                }
                else if(p["value"].is_string())
                {
                    create_recipe_param(obj_recipe, p["name"].get<std::string>().c_str(), p["unit"].get<std::string>().c_str(), p["value"].get<std::string>().c_str(), R_VAL_STRING_T);
                }
            }
        }
        else
        {
            // 找不到工艺
            return NULL;
        }
    }
    else
    {
        // 新建
        obj_recipe = create_recipe();
        for (json::iterator it_params = j_params->begin(); it_params != j_params->end(); ++it_params)
        {
            json p = *it_params;
            if(p["value"].is_boolean())
            {
                bool v = p["value"].get<bool>();
                create_recipe_param(obj_recipe, p["name"].get<std::string>().c_str(), p["unit"].get<std::string>().c_str(), &v, R_VAL_BOOLEAN_T);
            }
            else if(p["value"].is_number_float())
            {
                double v = p["value"].get<double>();
                create_recipe_param(obj_recipe, p["name"].get<std::string>().c_str(), p["unit"].get<std::string>().c_str(), &v, R_VAL_DOUBLE_T);
            }
            else if(p["value"].is_number_integer())
            {
                int v = p["value"].get<int>();
                create_recipe_param(obj_recipe, p["name"].get<std::string>().c_str(), p["unit"].get<std::string>().c_str(), &v, R_VAL_INT64_T);
            }
            else if(p["value"].is_number_unsigned())
            {
                unsigned v = p["value"].get<unsigned>();
                create_recipe_param(obj_recipe, p["name"].get<std::string>().c_str(), p["unit"].get<std::string>().c_str(), &v, R_VAL_UINT64_T);
            }
            else if(p["value"].is_string())
            {
                create_recipe_param(obj_recipe, p["name"].get<std::string>().c_str(), p["unit"].get<std::string>().c_str(), p["value"].get<std::string>().c_str(), R_VAL_STRING_T);
            }
        }
    }

    return obj_recipe;
}

IOTSEED_RECIPE_PARAM create_recipe_param(const IOTSEED_RECIPE* recipe, const char *name, const char* unit, const void *value, const IOTSEED_VAL_TYPE type){
    if(nullptr == g_DeviceRecipes){
#ifdef IOTSEED_DEBUG
        fprintf(stderr, "请先初始化设备定制组\n");
        return *(IOTSEED_RECIPE_PARAM*)nullptr;
#endif
    }
    auto _class_ret = _iotseed_recipe_param_class_t(name, unit);
    _class_ret.index = (ST_INT32)g_DeviceRecipes->Recipes[recipe->Index].Params.size() +1;
    _class_ret.value_to_string(value, type);
    g_DeviceRecipes->Recipes[recipe->Index].Params.emplace(_class_ret.index, _class_ret);

    return _class_ret.to_c_recipe_param_struct();
}

ST_VOID destroy_recipe_param(IOTSEED_RECIPE_PARAM **param){
    delete *param;
    *param = nullptr;
}


//ST_RET insert_recipe_param(IOTSEED_RECIPE *pRecipe, const IOTSEED_RECIPE_PARAM *pRecipeParam){
//
//}

ST_RET registry_iotseed_recipe_rpc_method(IOTSEED_RPC_METHOD _method, iotseed_rpc_handler_t f, void *user_data){
    switch (_method){
        case RPC_METHOD_SETRECIPE:{
            registry_iotseed_rpc_method(RPC_METHOD_SETRECIPE_STR, f, user_data);
            break;
        }
        case RPC_METHOD_ACTIVERECIPE:{
            registry_iotseed_rpc_method(RPC_METHOD_ACTIVERECIPE_STR, f, user_data);
            break;
        }
        default:
#ifdef IOTSEED_DEBUG
            fprintf(stderr,"不支持此方法\n");
#endif
            return SD_FAILURE;
    }
}

// 工艺下发请求参数(反序列化)
IOTSEED_RECIPE *handler_set_recipe_params(IOTSEED_RPC_SET_RECIPE_PARAM *param, const char *str_rpc_params)
{
    assert(param && str_rpc_params);

    IOTSEED_RECIPE *rt = nullptr;
    json rpc_params = json::parse(str_rpc_params);

    if(!rpc_params.is_array())
    {
        return nullptr;
    }

    // 遍历rpc参数
    for (json::iterator it = rpc_params.begin(); it != rpc_params.end(); ++it)
    {
        json j_set_recipe_params = (*it)["Recipes"];
        if(!j_set_recipe_params.is_array())
        {
            continue;
        }

        // 遍历工艺组
        ST_INT32 index_recipe = 0;
        for (json::iterator it_recipes = j_set_recipe_params.begin(); it_recipes != j_set_recipe_params.end(); ++it_recipes)
        {
            json j_recipe = *it_recipes;
            param->size = j_recipe.size();
            if(j_recipe["Index"].is_null())
            {
                param->Recipes[index_recipe].Index = 0;
            }
            else
            {
                param->Recipes[index_recipe].Index = j_recipe["Index"];
            }

            json j_recipe_params = j_recipe["Params"];
            if(!j_recipe_params.is_array())
            {
                continue;
            }

            if(param->Recipes[index_recipe].Index == 0)
            {
                rt = iotseed_set_recipe(nullptr, &j_recipe_params);
            }
            else
            {
                rt = iotseed_set_recipe(&(param->Recipes[index_recipe].Index), &j_recipe_params);
            }

            index_recipe++;
        }

        break;
    }

    return rt;
}

//ST_RET seriral_device_recipes(JSONRPCRequest *req)
//{
//    json _j;
//    g_DeviceRecipes->serialize(_j);
//    insert_jsonrpc_param(req, "recipes",(void*)&_j,R_VAL_OBJECT_T );
//}

// 工艺下发肯定响应结果(序列化)
ST_RET serializer_set_recipe_result(char *raw_str, const IOTSEED_RPC_SET_RECIPE_RESULT *result, unsigned int response_id)
{
    json j;
    j["code"] = result->code;
    j["Recipes"] = json::array();

    json j_recipe;
    _iotseed_recipe_class_t recipe = g_DeviceRecipes->Recipes.at(result->Recipes[0].Index);
    recipe.serialize(j_recipe);
    j_recipe.erase("LastActivedTimeStamp");

    j["Recipes"].push_back(j_recipe);

    JSONRPCResponse rpc_response;
    init_jsonrpc_response(&rpc_response, response_id, RPC_RESP_P);
    set_jsonrpc_response_param(&rpc_response, &j);

    serializer_jsonrpc_response(&rpc_response, raw_str, SD_TRUE);

    return SD_SUCCESS;
}

// 工艺激活请求参数(反序列化)
ST_RET handler_active_recipe_params(IOTSEED_RPC_ACTIVE_RECIPE_PARAM *param, const char *raw_str)
{
    json rpc_params = json::parse(raw_str);

    for (json::iterator it_recipes = rpc_params.begin(); it_recipes != rpc_params.end(); ++it_recipes)
    {
        param->Index = (*it_recipes)["Index"];
        break;
    }

    return active_recipe(param->Index);
}

// 工艺激活肯定响应结果(序列化)
ST_RET serializer_active_recipe_result(char *raw_str, const IOTSEED_RPC_ACTIVE_RECIPE_RESULT *result, unsigned int response_id)
{
    json j;
    j["code"] = result->code;

    json j_actived;
    g_DeviceRecipes->Recipes[g_DeviceRecipes->ActivedGroup].serialize(j_actived);

    j["ActivedGroup"] = j_actived["Index"];
    j["Actived"] = j_actived;

    JSONRPCResponse rpc_response;
    init_jsonrpc_response(&rpc_response, response_id, RPC_RESP_P);
    set_jsonrpc_response_param(&rpc_response, &j);

    serializer_jsonrpc_response(&rpc_response, raw_str, SD_TRUE);

    return SD_SUCCESS;
}

// 工艺否定响应消息(序列化)
ST_RET serializer_error(char *raw_str, const IOTSEED_RPC_ERROR_MESSAGE *error, unsigned int response_id)
{
    json _j;
    g_DeviceRecipes->serialize(_j);

    json j;
    j["code"] = error->code;
    j["message"]["Message"] = error->message.Message;
    j["message"]["Recipes"] = _j;

    JSONRPCResponse rpc_response;
    init_jsonrpc_response(&rpc_response, response_id, RPC_RESP_N);
    set_jsonrpc_response_param(&rpc_response, &j);

    serializer_jsonrpc_response(&rpc_response, raw_str, SD_TRUE);

    return SD_SUCCESS;
}

ST_VOID show_all_recipes(ST_VOID)
{
    json _j;
    g_DeviceRecipes->serialize(_j);


    std::cout<<"当前设备所有工艺：\n"<<std::setw(4) << _j <<std::endl;
}