//
// Created by gubin on 17-11-28.
//

#include "log.h"

#include <unistd.h>

#include <signal.h>

static int end = 0;


static void signal_handler(int sig_num) {
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    end = 1;
}


int main(){
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    LOGGER *log = create_console_log("111");

    LOGGER *log2 = create_daily_log("222","./test.log",0,0);

    LOGGER *log3 = create_rotated_log("333","./testrotate.log",MAXLOG_FILE_SIZE, MAXLOG_FILE_NUM);

    while(!end){
        write_log(log,log->config->log_level,"1111");
        write_log(log2,log2->config->log_level,"2222");
        write_log(log3,log3->config->log_level,"333");
        usleep(100);
    }

    destroy_logger(&log);
    destroy_logger(&log2);
    destroy_logger(&log3);
}
