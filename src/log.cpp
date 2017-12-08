//
// Created by gubin on 17-11-22.
//

#include "log.h"

#include <sys/stat.h>

//spdlog 库头文件
#include "spdlog/spdlog.h"
#include "spdlog/logger.h"

#include <iostream>
#include <memory>
#include <cstdarg>



#include "operation_utils.h"

#include "json/json.hpp"

// for convenience
using nlohmann::json;

using std::string;

static void parse_time_offset(char* dst){
    int _offset = atoi(dst);
    int _hour = _offset / 100;
    int _minutes = _offset % 100;
    sprintf(dst, "%02d:%02d",_hour,_minutes);
}


namespace iotseed_log {
    class _iotseed_log_msg{
        public:
            string IOTSeedLogLevel;
            string IOTSeedClientID;
            string IOTSeedLogType;
            string IOTSeedMessage;
            string IOTSeedTS; //时间戳
        public:
            _iotseed_log_msg(string clientID, string type, string msg){
                this->IOTSeedClientID = clientID;
                this->IOTSeedLogType = type;
                this->IOTSeedMessage = msg;
                auto now = time(nullptr);
                auto tm_time = spdlog::details::os::localtime(now);
                char date_buf[100];
                std::strftime(date_buf, sizeof(date_buf), "%Y-%m-%dT%H:%M:%S%z", &tm_time);
                size_t _offset = strlen(date_buf) - 4;
                parse_time_offset(date_buf + _offset);
                this->IOTSeedTS = date_buf;
            }

    };

    void to_json(json& j, const _iotseed_log_msg& p) {
        j = json{{"IOTSeedLogLevel", p.IOTSeedLogLevel},
                 {"IOTSeedClientID", p.IOTSeedClientID},
                 {"IOTSeedLogType", p.IOTSeedLogType},
                 {"IOTSeedMessage", p.IOTSeedMessage},
                 {"IOTSeedTS", p.IOTSeedTS}
        };
    }
}


namespace spd = spdlog;

static inline ST_BOOLEAN iConfigIsNull(const LOG_CONFIG* config ){
    if (nullptr == config){
        return SD_TRUE;
    }

    return SD_FALSE;
}

static spd::level::level_enum iotseed_loglevel_to_spd(const LOG_LEVEL level){
    switch (level){
//        case Trace:
//            return spd::level::trace;
//        case Debug:
//            return spd::level::debug;
        case Info:
            return spd::level::info;
        case Warn:
            return spd::level::warn;
        case Err:
            return spd::level::err;
        case Critical:
            return spd::level::critical;
//        case Off:
//            return spd::level::off;
        default:
            return spd::level::info;
    }
}

static LOG_CONFIG* create_log_config(const char *name, const char *clientID){

    if(nullptr == name || strlen(name) > LOG_NAME_MAX){
        fprintf(stderr, "日志名称长度大于最大长度:%d\n", LOG_NAME_MAX);
        return nullptr;
    }
    auto pCnf = new LOG_CONFIG();
    strcpy(pCnf->pattern,LOG_PATTERN_DEFAULT);
    strcpy(pCnf->clientID, clientID);
    pCnf->log_level = Info;
    strcpy(pCnf->name , name);
    pCnf->log_type = Normal;
    return pCnf;
}

ST_VOID destroy_log_config(LOG_CONFIG** config){
    delete(*config);
    *config = nullptr;
}


/*!
 *
 * @param name
 * @param path
 * @param eLogType
 * @param ...
 * @return
 */
ST_RET create_spd_logger(LOGGER* pLogger, const char* name, const char *path, const LOG_TYPE eLogType, ...) {
    va_list args;
    va_start(args, eLogType);
    std::shared_ptr<spd::logger> logger;
    switch (eLogType) {
        case Normal:
            logger = spd::basic_logger_mt(name, path, true); //默认打开追加模式
            break;
        case Rotate:
        {
            size_t file_size_bit = va_arg(args, size_t);
            size_t file_num = va_arg(args, size_t);
            logger = spd::rotating_logger_mt(name, path, file_size_bit, file_num);
        }
            break;
        case Daily:
        {
            int hour = va_arg(args, int);
            int minute = va_arg(args, int);
            logger = spd::daily_logger_mt(name,path, hour, minute);
            logger->flush_on(spd::level::warn); //默认等级高于warn时触发
        }
            break;
        default:
            logger = spd::stdout_color_mt(name);
            break;
    }

    va_end(args);

    strncpy(pLogger->config->name, name, strlen(name) + 1);

    logger->set_pattern(LOG_PATTERN_DEFAULT);

    pLogger->spd_logger = logger.get();

    return SD_SUCCESS;

}


/*!
 * 创建每日日志
 * @param name 日志的名字
 * @param path 日志文件夹的路径
 * @param file_size_bit 日志文件大小(bit)
 * @param file_num 日志文件最多生成几份
 * @return
 */
LOGGER* create_rotated_log(const char *name, const char *clientID, const char *path, const size_t file_size_bit, const size_t file_num) {
    LOGGER* pLogger = new LOGGER();
    if((pLogger->config = create_log_config(name, clientID)) == nullptr){
        fprintf(stderr, "日志配置创建失败\n");
        return nullptr;
    };

    create_spd_logger(pLogger,name, path, Rotate, file_size_bit, file_num);
    return pLogger;

}

/*!
 * 创建终端日志
 * @return
 */
LOGGER* create_console_log(const char* name, const char *clientID) {
    LOGGER* pLogger = new LOGGER();
    if((pLogger->config = create_log_config(name, clientID)) == nullptr){
        fprintf(stderr, "日志配置创建失败\n");
        return nullptr;
    };

    create_spd_logger(pLogger, name, nullptr, Console);
    return pLogger;
}

/*!
 * 创建每日日志
 * @param name 日志的名字
 * @param path 日志文件的路径
 * @param hour 日志文件在每天几时生成
 * @param minute 日志文件在每天几分生成
 * @return
 */
LOGGER* create_daily_log(const char *name, const char *clientID, const char *path, const int hour, const int minute) {
    LOGGER* pLogger = (LOGGER*) calloc(1, sizeof(LOGGER));
    if((pLogger->config = create_log_config(name, clientID)) == nullptr){
        fprintf(stderr, "日志配置创建失败\n");
        return nullptr;
    };

    create_spd_logger(pLogger,name, path, Daily, hour, minute);
    return pLogger;
}


ST_VOID destroy_logger(LOGGER **log){
    destroy_log_config(&((*log)->config));
    delete(*log);
    *log = nullptr;
}

/*!
 *
 * @return SD_SUCCESS: 成功
 *         SD_FAILURE: 失败
 */
ST_RET set_iotseed_log_sync_mode(ST_VOID){

    spd::set_sync_mode();

    return SD_SUCCESS;
}

/*!
 *
 * @param buf_size 异步模式缓存大小：2的次方
 * @param seconds 多少秒刷新一次
 * @return SD_SUCCESS: 成功
 *         SD_FAILURE: 失败
 */
ST_RET set_iotseed_log_async_mode(const size_t buf_size, const ST_UINT32 seconds){
    if(!judge_is_log2((int)buf_size)){
        return SD_FAILURE;
    }
    spd::set_async_mode(buf_size, spdlog::async_overflow_policy::block_retry, nullptr, std::chrono::seconds(seconds));

    return SD_SUCCESS;
}


/*!
 *
 * @param config 日志配置指针
 * @param level  日志记录等级
 * @return SD_SUCCESS: 成功
 *         SD_FAILURE: 失败
 */
ST_RET set_log_level(LOGGER* pLogger, const LOG_LEVEL level){
    if (iConfigIsNull(pLogger->config)){
        return SD_FAILURE;
    }

    spd::get(pLogger->config->name)->set_level(iotseed_loglevel_to_spd(level));

    pLogger->config->log_level = level;

    return SD_SUCCESS;
}



/*!
 *
 * @param config 日志配置指针
 * @param format  日志自定义格式
 * @return SD_SUCCESS: 成功
 *         SD_FAILURE: 失败
 */
ST_RET set_log_msg_format(LOGGER* pLogger, const char* format){
    if (iConfigIsNull(pLogger->config)){
        return SD_FAILURE;
    }
    if(strlen(format) > LOG_PATTERN_MAX){
        fprintf(stderr, "日志自定义格式长度大于:%d\n", LOG_PATTERN_MAX);
        return SD_FAILURE;
    }

    spd::get(pLogger->config->name)->set_pattern(format);

    strncpy(pLogger->config->pattern, format, strlen(format) + 1);

    return SD_SUCCESS;
}


ST_VOID write_log(const LOGGER* logger, LOG_LEVEL level, const char* msg, LOG_INFO_TYPE type){
    auto local_log = spd::get(logger->config->name);
    if (nullptr == msg || nullptr == local_log){
        return;
    }
    string _type;
    switch (type){
        case Log:
            _type = "log";
            break;
        case Recipe:
            _type = "recipe";
            break;
        default:
            fprintf(stderr,"日志类型未定义\n");
            return;
    }
    auto message = iotseed_log::_iotseed_log_msg(logger->config->clientID, _type, msg);
    json j;
    switch (level){
        case Info:
            message.IOTSeedLogLevel = "INFO";
            j = message;
            (local_log)->info(j.dump());
            break;
        case Warn:
            message.IOTSeedLogLevel = "WARNING";
            j = message;
            (local_log)->warn(j.dump());
            break;
        case Critical:
            message.IOTSeedLogLevel = "CRITICAL";
            j = message;
            (local_log)->critical(j.dump());
            break;
        case Err:
            message.IOTSeedLogLevel = "ERROR";
            j = message;
            (local_log)->error(j.dump());
            break;
//        case Trace:
//            message.IOTSeedLogLevel = "TRACE";
//            j = message;
//            (local_log)->trace(j.dump());
//            break;
//        case Debug:
//            message.IOTSeedLogLevel = "DEBUG";
//            j = message;
//            (local_log)->debug(j.dump());
//            break;
//        case Off:
//            message.IOTSeedLogLevel = "OFF";
//            j = message;
//            (local_log)->error(j.dump());
//            break;
        default:
            message.IOTSeedLogLevel = "INFO";
            j = message;
            (local_log)->info(j.dump());
            break;
    }

}