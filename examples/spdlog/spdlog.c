//
// Created by gubin on 17-11-28.
//

#include "log.h"

#if !defined(_WIN32)
#include <unistd.h>
#include <signal.h>
#endif

#include "time_utils.h"

static int end = 0;

const char *CLIENT_ID = "23f901e0-ccc3-11e7-bf1a-59e9355b22c6";

#if !defined(_WIN32)
static void signal_handler(int sig_num) {
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    end = 1;
}
#endif


int main(){

#if !defined(_WIN32)
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
#endif

    if(iotseed_log_async_mode(4096,5) == SD_FAILURE){
        fprintf(stderr,"设定异步刷新失败\n");
        exit(1);
    }; 

    IOTSEED_LOGGER *log = iotseed_create_console_log("111",CLIENT_ID);

    IOTSEED_LOGGER *log2 = iotseed_create_daily_log("222", CLIENT_ID,"./test.log",0,0);

    IOTSEED_LOGGER *log3 = iotseed_create_rotated_log("333",CLIENT_ID,"./testrotate.log",IOTSEED_MAXLOG_FILE_SIZE, IOTSEED_MAXLOG_FILE_NUM);

    while(!end){
        iotseed_write_log(log, Critical, "1111", Log);
        iotseed_write_log(log2,Info, "2222", Recipe);
        iotseed_write_log(log3,Info, "333", Log);

        iotseed_msSleep(1000);
    }

    iotseed_destroy_logger(&log);
    iotseed_destroy_logger(&log2);
    iotseed_destroy_logger(&log3);
}
