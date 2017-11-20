//
// Created by gubin on 17-11-20.
//

#ifndef IOTSEED_SDKS_CORE_SYSINC_H_H
#define IOTSEED_SDKS_CORE_SYSINC_H_H

#if defined (_WIN32) || defined(_WIN64)
#if !defined (_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE
#endif
#if !defined(__GNUC__)
#pragma warning(disable : 4996)
#pragma warning(disable : 4786 4800)
#endif
#endif


//统一导入的头文件
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#if defined(linux)
#include <sys/timeb.h>		/* for ftime, timeb		*/
#include <dirent.h>		/* for POSIX directory functions*/
#endif


#endif //IOTSEED_SDKS_CORE_SYSINC_H_H
