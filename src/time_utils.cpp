//
// Created by gubin on 17-11-30.
//

#include "time_utils.h"

#include "mongoose.h"


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


ST_DOUBLE iotseed_get_current_ts(void){
    return mg_time();
}