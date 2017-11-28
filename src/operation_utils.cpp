//
// Created by gubin on 17-11-24.
//

#include "operation_utils.h"


/*!
 *
 * @param n 一个整数
 * @return SD_TRUE:是2的次方
 *         SD_FALSE: 不是2的次方
 */
ST_RET judge_is_log2(const int n){
    int b = n - 1;
    if ((n & b) == 0)
    {
        return SD_TRUE;
    }

    return SD_FALSE;
}