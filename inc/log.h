//
// Created by gubin on 17-11-22.
//

#ifndef IOTSEED_SDKS_CORE_LOG_H
#define IOTSEED_SDKS_CORE_LOG_H

#ifdef __cplusplus
extern "C" {
#endif


#include "sysinc.h"

#include "glbtypes.h"


#ifndef MAXLOG_FILE_SIZE
#define MAXLOG_FILE_SIZE (1048576 * 5) //5MB
#endif

#ifndef MAXLOG_FILE_NUM
#define MAXLOG_FILE_NUM 10
#endif

#ifndef LOG_NAME_MAX
#define LOG_NAME_MAX    63
#endif

#ifndef LOG_PATTERN_MAX
#define LOG_PATTERN_MAX 127
#endif


#define DEFINE_LOG_PATTERN  "*** [%H:%M:%S %z] [thread %t] %v ***"


typedef enum _enum_log_type_t {
    Normal,
    Rotate,
    Daily,
    Console
}LOG_TYPE;


typedef enum _enum_log_level_t {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Err = 4,
    Critical = 5,
    Off = 6
}LOG_LEVEL;


typedef struct _struct_log_config_t {
    char        name[LOG_NAME_MAX + 1];
    LOG_TYPE    log_type;
    char        pattern[LOG_PATTERN_MAX + 1];
    ST_BOOLEAN  aync_mode; //默认为异步模式
    size_t      queue_size; //queue size must be power of 2
    LOG_LEVEL   log_level;
}LOG_CONFIG;


typedef struct _struct_logger_t {
    ST_VOID_PTR     spd_logger;
    LOG_CONFIG      *config;
}LOGGER;

extern LOGGER* create_rotated_log(const char *name, const char *path, const size_t file_size_bit, const size_t file_num);

extern LOGGER* create_console_log(const char* name);

extern LOGGER* create_daily_log(const char *name, const char *path, const int hour, const int minute);

extern ST_VOID destroy_logger(LOGGER **log);

extern ST_VOID write_log(const LOGGER* logger, LOG_LEVEL level, const char* msg);

extern ST_RET set_log_msg_format(LOGGER* pLogger, const char* format);



#ifdef __cplusplus
}
#endif

#endif //IOTSEED_SDKS_CORE_LOG_H