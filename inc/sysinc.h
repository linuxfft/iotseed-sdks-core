//
// Created by gubin on 17-11-20.
//

#ifndef IOTSEED_SDKS_CORE_SYSINC_H_H
#define IOTSEED_SDKS_CORE_SYSINC_H_H


#ifndef IOTSEED_SDK_VERSION
#define IOTSEED_SDK_VERSION "0.0.1"
#endif

#if defined (_WIN32)
#if !defined (_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE
#define IOTSEED_API _declspec(dllexport)
#endif
#if !defined(__GNUC__)
#pragma warning(disable : 4996)
#pragma warning(disable : 4786 4800)
#endif
#include <winsock2.h>		/* must be before windows.h	*/
#include <windows.h>
#include <process.h>		/* for _beginthread, _endthread	*/
#include <sys/timeb.h>		/* for ftime, timeb		*/

#else
#define IOTSEED_API
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#if defined(linux) || defined(__LYNX)
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
