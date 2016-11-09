#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<errno.h>
#include<features.h>
#include"eosi_barrier_user.h"
static unsigned int sleep_time = 1;

void* pfunction(void* datain) {
    int cnt = 0;
    unsigned int barrier_id = *(unsigned int*) datain;

    printf("pid: 0x%x, tid: 0x%x\n", (int)getpid(), (unsigned int)pthread_self());

re_cnt:
    if(cnt++ == 3) return 0;
    sleep(1);
    barrier_wait(barrier_id);
    goto re_cnt;

    return NULL;
}

void* run_barriers(void* datain) {
    int num_threads = *(int*)datain;
    pthread_t tids[num_threads]; int idx = 0;
    unsigned int barrier_id = 0;

    barrier_init(num_threads, &barrier_id);
    for(idx = 0; idx < num_threads; idx++) {
        pthread_create(&tids[idx], NULL, pfunction, &barrier_id);
    }

    for(idx = 0; idx < num_threads; idx++) {
        pthread_join(tids[idx], NULL);
    }

    barrier_destroy(barrier_id);
    return NULL;
}

//for official use 
void fork_run() {
    int t1_num = 5;
    int t2_num = 20;
    pthread_t t1;
    pthread_t t2;

    pthread_create(&t1, NULL, run_barriers, &t1_num);
    pthread_create(&t2, NULL, run_barriers, &t2_num);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}

//for development use
void dev_run() {
    pthread_t t1;
    int t_num = 2;
    pthread_create(&t1, NULL, run_barriers, &t_num);
    pthread_join(t1, NULL);
}

int main(int argc, char*argv[]) {
    if(2 == argc) {
        sleep_time = atoi(argv[1]);
    } else if(2 < argc) {
        puts("At most, only 1 parameter is allowed!");
        return 0;
    }

    eosi_barrer_fd = open("/dev/eosi_barrier_1", O_RDWR);
    if(0 == fork()) {
        //fork_run();
        dev_run();
        return 0;
    }
    sleep(5);
    puts("--------");

    if(0 == fork()) {
        //fork_run();
        return 0;
    }

    while(waitpid(-1, NULL, 0)) {
        if(ECHILD == errno) break;
    }
    
    puts("done");

    return 0;
}
