//
// Created by gubin on 17-11-28.
//

#include "log.h"

#include <unistd.h>

#include <signal.h>

static int end = 0;

const char *CLIENT_ID = "23f901e0-ccc3-11e7-bf1a-59e9355b22c6";


static void signal_handler(int sig_num) {
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    end = 1;
}


int main(){
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    LOGGER *log = create_console_log("111",CLIENT_ID);

    LOGGER *log2 = create_daily_log("222", CLIENT_ID,"./test.log",0,0);

    LOGGER *log3 = create_rotated_log("333",CLIENT_ID,"./testrotate.log",MAXLOG_FILE_SIZE, MAXLOG_FILE_NUM);

    while(!end){
        write_log(log, Critical, "1111", Log);
        write_log(log2,Info, "2222", Recipe);
        write_log(log3,Info, "333", Log);
        sleep(1);
    }

    destroy_logger(&log);
    destroy_logger(&log2);
    destroy_logger(&log3);
}
