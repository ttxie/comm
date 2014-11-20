#include <pthread.h>
#include <string.h>
#include "log.h"
#include "utl.h"



int main()
{
    uint64_t start, end;
    FILE *fp;
    int n=0, fd=0;
    char buf[128];
    struct log_s *log = (struct log_s*)calloc(1, sizeof(struct log_s));

    if (log == NULL){
        perror("server_init: calloc log_s err.");
        exit(1);
    }
    log->log_path = "./";
    log->log_filename = "log_test";
    log->log_suffix = ".log";
    if (log_open(log) == -1) {
        perror("server_init: init log err.");
        exit(1);
    }


    fd = open("log_fd.log", O_WRONLY|O_APPEND|O_CREAT);
    strcpy(buf, "server_init log ok\n");
    start = tm64();
    for(n=0; n<10000; n++) {
        write(fd, buf, strlen(buf));
    }
    end = tm64();
    printf("write end:%llu, start:%llu, time:%d\n", end, start, end-start);



    start = tm64();
    for(n=0; n<10000; n++) {
        log_printf(log, "server_init log ok\n");
    }
    end = tm64();
    printf("end:%llu, start:%llu, time:%d\n", end, start, end-start);

    sleep(10);
    return 0;
}

