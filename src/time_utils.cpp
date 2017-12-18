//
// Created by gubin on 17-11-30.
//

#include "time_utils.h"


#if defined(_WIN32)

ST_VOID iotseed_msSleep (ST_LONG ms)
  {
  Sleep (ms);
  }

#elif defined(__VMS)

ST_VOID iotseed_msSleep (ST_LONG ms)
  {
ST_ULONG     msec = (ST_ULONG) ms;
unsigned int us;

  while (msec > 0)
    {
    if (msec >= 1000)
      {
      us = 999999;    /* microseconds, it must be less than 1000000 for usleep */
      msec -= 1000;
      }
    else
      {
      us = msec * 1000;
      msec = 0;
      }
    usleep (us);    /* unistd.h */
    }
  }

#else	/* all other systems	*/

/* This version of the function uses the POSIX nanosleep function.	*/
ST_VOID iotseed_msSleep (ST_LONG ms)
{
    struct timespec rqtp;

    rqtp.tv_sec = ms / 1000;
    rqtp.tv_nsec =  (ms % 1000) * 1000000;    /* 1000 ms/us, 1000 us/ns ... 	*/

    nanosleep (&rqtp, NULL);
}

#endif	/* all other systems	*/


/*****************************************************************************/
/*  gettimeofday - simulate Unix/Linux function gettimeofday()               */
/*  function is used for debugging purposes only                             */
/*****************************************************************************/


#ifdef STDTIMEW32_USE_GETTIMEOFDAY

STDTIMEW32_STATIC int gettimeofday (
  struct timeval *pTimeVal, void *pObsolete)
  {
  STDTIME_WIN_FILETIME       ft;
  QUADLIB_U64                qFileTime;
  QUADLIB_U64                qSec;
  QUADLIB_U64                qNsec100;
  QUADLIB_U64                qNum;

  if (pTimeVal == NULL)
    {
    errno = EFAULT;
    return STDTIMEW32_GETTIMEOFDAY_FAILURE;
    }

  if (pObsolete != NULL)
    {
    /* the obsolete argument is not supposed to be used */
    errno = EINVAL;
    return STDTIMEW32_GETTIMEOFDAY_FAILURE;
    }

  GetSystemTimeAsFileTime (&ft);

  QUADLIB_U64_HI(qFileTime) = ft.dwHighDateTime;
  QUADLIB_U64_LO(qFileTime) = ft.dwLowDateTime;

  /* conversion code adapted from MSDN KB Q167296 */
  /* subtract epoch factor and divide by 10_000_000  */

  QUADLIB_U64_HI(qNum) = STDTIMEW32_FT_TIMET_BASE_HI;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_BASE_LO;

  QUADLIB_I64_SUB_EQ (qFileTime, qNum);

  QUADLIB_U64_HI(qNum) = 0;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_SCALE;

  qSec = QuadLibU64DivMod (qFileTime, qNum, &qNsec100);

  pTimeVal->tv_sec = (long) QUADLIB_U64_LO(qSec);

  /* qNsec100 has multiples of 100 nanoseconds, or 1/10 microseconds */
  /* to get whole microseconds, divide by 10 */

  pTimeVal->tv_usec = (long) QUADLIB_U64_LO(qNsec100) / 10;

  return STDTIMEW32_GETTIMEOFDAY_SUCCESS;

  } /* gettimeofday */


#endif /* STDTIMEW32_USE_GETTIMEOFDAY */

