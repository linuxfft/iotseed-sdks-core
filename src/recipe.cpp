//
// Created by gubin on 17-12-5.
//

#include "recipe.h"

#include "time_utils.h"

#include "log.h"

#include <iostream>

#include <list>

#include <map>

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

    void value_to_string(const void *value, const IOTSEED_RECIPE_VAL_TYPE type){
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
        IOTSEED_RECIPE_PARAM ret = {this->index, this->name.c_str(), this->unit.c_str(), this->orig_data};
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

    void write_recipe(const LOGGER* logger, LOG_LEVEL level){
        json j;
        this->serialize(j);
        write_log(logger,level,j.dump().c_str(),Recipe);
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

    void write_recipes(const LOGGER* logger, LOG_LEVEL level){
        json j;
        this->serialize(j);
        write_log(logger,level,j.dump().c_str(),Recipe);
    }

};





static DEVICE_RECIPES *g_DeviceRecipes = nullptr;


ST_RET init_device_recipes(const char* client_id){
    if(nullptr == g_DeviceRecipes){
        g_DeviceRecipes = new DEVICE_RECIPES(client_id);
    }
    return SD_SUCCESS;
}

ST_RET write_device_recipes(const LOGGER *logger){
    if(nullptr != g_DeviceRecipes){
        g_DeviceRecipes->write_recipes(logger, Info);

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


ST_RET write_recipe(IOTSEED_RECIPE* recipe, const LOGGER *logger, LOG_LEVEL level){
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



IOTSEED_RECIPE_PARAM create_recipe_param(const IOTSEED_RECIPE* recipe, const char *name, const char* unit, const void *value, const IOTSEED_RECIPE_VAL_TYPE type){
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