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

    void *log = create_console_log("111");

//    void *log2 = create_daily_log("222","./test.log",0,0);
//
//    void *log3 = create_rotated_log("333","./testrotate.log",MAXLOG_FILE_SIZE, MAXLOG_FILE_NUM);

    while(!end){
        write_log(log, Critical, "1111");
//        write_log(log2, 3, "2222");
//        write_log(log3, 3, "333");
        sleep(1);
    }

    destroy_logger(&log);
//    destroy_logger(&log2);
//    destroy_logger(&log3);
}
