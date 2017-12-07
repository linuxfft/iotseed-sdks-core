//
// Created by gubin on 17-11-30.
//

#ifndef IOTSEED_SDKS_CORE_TIME_UTILS_H
#define IOTSEED_SDKS_CORE_TIME_UTILS_H


#ifdef __cplusplus
extern "C" {
#endif

#include "sysinc.h"

#include "glbtypes.h"


#if defined(_WIN32)
#define STDTIMEW32_USE_GETTIMEOFDAY
#endif

#define STDTIMEW32_GETTIMEOFDAY_SUCCESS 0
#define STDTIMEW32_GETTIMEOFDAY_FAILURE (-1)

#ifdef STDTIMEW32_USE_GETTIMEOFDAY

/* on Unix/Linux systems, gettimeofday() is defined in */
/* <time.h> or <sys/time.h> */

#ifndef _WINSOCKAPI_
struct timeval
  {
  long    tv_sec;    /* seconds since Jan 1 1970 */
  long    tv_usec;   /* microseconds (seconds / 1_000_000) */
  };
#endif

STDTIMEW32_STATIC int gettimeofday (
  struct timeval *pTimeVal, void *pObsolete);

/*****************************************************************************/
/*                                                                           */
/*  pObsolete as an argument is obsolete on most Unix/Linux systems, in      */
/*  which it is an error to specify a non-NULL value.                        */
/*                                                                           */
/*  gettimeofday() return codes                                              */
/*  rc 0: success                                                            */
/*  rc -1: failure, errno set.                                               */
/*  errno: EFAULT pTimeVal points outside the accessible address space.      */
/*  errno: EINVAL The structure pointed to by 'pTimeVal' specifies an        */
/*         invalid time (not supported here)                                 */
/*  errno: EINVAL pObsolete argument is not NULL                             */
/*  errno: EOVERFLOW: The system time is greater than 2038, cannot be stored.*/
/*         Windows does not have EOVERFLOW in errno.h; ERANGE could be used, */
/*         but for simulation purposes, this condition is not supported.     */
/*                                                                           */
/*****************************************************************************/


#endif /* STDTIMEW32_USE_GETTIMEOFDAY */

ST_VOID iotseed_msSleep(ST_LONG ms);

#ifdef __cplusplus
}
#endif

#endif //IOTSEED_SDKS_CORE_TIME_UTILS_H
