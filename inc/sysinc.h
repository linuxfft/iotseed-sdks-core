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
#include <winsock2.h>		/* must be before windows.h	*/
#include <windows.h>
#include <process.h>		/* for _beginthread, _endthread	*/
#include <sys/timeb.h>		/* for ftime, timeb		*/
#endif


//统一导入的头文件
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#if defined(linux)
#include <sys/timeb.h>		/* for ftime, timeb		*/
#include <dirent.h>		/* for POSIX directory functions*/
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#endif

#ifndef IOTSEED_CLIENT_ID_MAX
#define IOTSEED_CLIENT_ID_MAX   63
#endif

#endif //IOTSEED_SDKS_CORE_SYSINC_H_H
