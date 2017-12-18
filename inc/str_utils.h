//
// Created by gubin on 17-11-20.
//

#ifndef IOTSEED_SDKS_CORE_STR_UTIL_H
#define IOTSEED_SDKS_CORE_STR_UTIL_H



#ifdef __cplusplus
extern "C" {
#endif

#include "sysinc.h"

#include "glbtypes.h"

IOTSEED_API ST_BOOLEAN isAllWhitespace (const char *astr);

IOTSEED_API ST_VOID* create_string(const size_t length, const char *sSrc);

IOTSEED_API ST_RET free_string(char **pString);

#ifdef __cplusplus
}
#endif

#endif //IOTSEED_SDKS_CORE_STR_UTIL_H
