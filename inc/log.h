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


#ifndef IOTSEED_MAXLOG_FILE_SIZE
#define IOTSEED_MAXLOG_FILE_SIZE (1048576 * 5) //5MB
#endif

#ifndef IOTSEED_MAXLOG_FILE_NUM
#define IOTSEED_MAXLOG_FILE_NUM 10
#endif

#ifndef IOTSEED_LOG_NAME_MAX
#define IOTSEED_LOG_NAME_MAX    63
#endif

#ifndef IOTSEED_LOG_PATTERN_MAX
#define IOTSEED_LOG_PATTERN_MAX 127
#endif

#ifndef IOTSEED_CLIENT_ID_MAX
#define IOTSEED_CLIENT_ID_MAX   63
#endif

#ifndef IOTSEED_LOG_PATTERN_DEFAULT
//#define LOG_PATTERN_DEFAULT "*** [%H:%M:%S %z] [%l] [thread %t] %v ***"
#define IOTSEED_LOG_PATTERN_DEFAULT "%v"
#endif


typedef enum _enum_iotseed_logger_type_t {
    Normal,
    Rotate,
    Daily,
    Console
}IOTSEED_LOGGER_TYPE;


typedef enum _enum_iotseed_log_level_t {
//    Trace = 0,
//    Debug = 1,
    Info = 2,
    Warn = 3,
    Err = 4,
    Critical = 5,
//    Off = 6
}IOTSEED_LOG_LEVEL;

typedef enum _enum_iotseed_log_info_type_t {
    Log = 0,
    Recipe = 1
}IOTSEED_LOG_TYPE;


typedef struct _iotseed_struct_log_config_t {
    char                name[IOTSEED_LOG_NAME_MAX + 1];
    IOTSEED_LOGGER_TYPE logger_type;
    char                pattern[IOTSEED_LOG_PATTERN_MAX + 1];
    char                clientID[IOTSEED_CLIENT_ID_MAX + 1];
    IOTSEED_LOG_LEVEL   log_level;
}IOTSEED_LOG_CONFIG;


typedef struct _iotseed_struct_logger_t {
    ST_VOID					*spd_logger;
    IOTSEED_LOG_CONFIG      *config;
}IOTSEED_LOGGER;

IOTSEED_API IOTSEED_LOGGER* iotseed_create_rotated_log(const char *name, const char *clientID, const char *path, const size_t file_size_bit, const size_t file_num);

IOTSEED_API IOTSEED_LOGGER* iotseed_create_console_log(const char* name, const char *clientID);

IOTSEED_API IOTSEED_LOGGER* iotseed_create_daily_log(const char *name, const char *clientID, const char *path, const int hour, const int minute);

IOTSEED_API ST_VOID iotseed_destroy_logger(IOTSEED_LOGGER **log);

IOTSEED_API ST_VOID iotseed_write_log(const IOTSEED_LOGGER* logger, IOTSEED_LOG_LEVEL level, const char* msg, IOTSEED_LOG_TYPE type);

IOTSEED_API ST_RET iotseed_set_log_format(IOTSEED_LOGGER* pLogger, const char* format);

IOTSEED_API ST_RET iotseed_set_log_level(IOTSEED_LOGGER* pLogger, const IOTSEED_LOG_LEVEL level);

IOTSEED_API ST_RET iotseed_log_async_mode(const size_t buf_size, const ST_UINT32 seconds);

IOTSEED_API ST_RET iotseed_log_sync_mode(ST_VOID);



#ifdef __cplusplus
}
#endif

#endif //IOTSEED_SDKS_CORE_LOG_H
