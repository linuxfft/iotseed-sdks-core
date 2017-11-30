//
// Created by gubin on 17-11-20.
//

#include "str_utils.h"


ST_BOOLEAN isAllWhitespace (const char *astr)
{
    ST_BOOLEAN bRet = SD_TRUE;
    size_t   len;
    char *tmp;

    if (!astr)
        return SD_FALSE;
    if ((len = strlen(astr)) == 0)
        return SD_TRUE;

    for (tmp=(char *)astr; tmp<astr+len; ++tmp)
    {
        if (!isspace(*tmp))
        {
            bRet = SD_FALSE;
            break;
        }
    }

    return bRet;
}

ST_VOID_PTR create_string(const size_t length , const char *sSrc)
{
    ST_VOID_PTR pRet = calloc(1, length);

    memcpy(pRet, sSrc, length);

    return pRet;
}

ST_RET free_string(char **pString)
{
    free(*pString);
    *pString = nullptr;
    return SD_SUCCESS;
}

