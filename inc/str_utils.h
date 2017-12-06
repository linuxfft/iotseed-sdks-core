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

ST_BOOLEAN isAllWhitespace (const char *astr);

ST_VOID_PTR create_string(const size_t length, const char *sSrc);

ST_RET free_string(char **pString);

#ifdef __cplusplus
}
#endif

#endif //IOTSEED_SDKS_CORE_STR_UTIL_H
