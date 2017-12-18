//
// Created by gubin on 17-11-24.
//

#include "operation_utils.h"


ST_RET judge_is_log2(const int n){
    int b = n - 1;
    if ((n & b) == 0)
    {
        return SD_TRUE;
    }

    return SD_FALSE;
}