#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include"ht530_user.h"
#include"ht530_common.h"

int dev_fd;

struct Thread_Info{
    pthread_t thread_id;
    int thread_num;
};
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int write_hash_atomic(int key, int data) {
    int ret = 0;
    int s = 0;
    ht_object_t obj = {key, data};

    s = pthread_mutex_lock(&mtx);
    ret = write(dev_fd, (void*) &obj, sizeof(ht_object_t));
    s = pthread_mutex_unlock(&mtx);
    if(0 ==  obj.data) 
        printf("delete_hash_atomic: key:%d, data:%d\n", obj.key, obj.data);
    else
        printf("write_hash_atomic: key:%d, data:%d\n", obj.key, obj.data);

    return (ret == 0 && s == 0);
}
int read_hash(int key) {
    int ret;
    ht_object_t obj = {key, 0};
    ret = read(dev_fd, (void*) &obj, sizeof(ht_object_t));
    printf("read_hash: key:%d, data:%d\n", obj.key, obj.data);
    return ret;
}

void* thread_exec(void* arg) {
    /*random access*/
    int ret = 0;
    struct Thread_Info* tinfo = (struct Thread_Info*) arg;
    printf("I am thread:%2d\n", tinfo->thread_num);
    int i = 0;
    int act = 0;
    for(i = 0; i < 100;i++) {
        act = RAND_SCALE(DO_ADD, DO_SEARCH);
        switch (act) {
            case DO_ADD:
                ret = ADD(RAND_SCALE(1,200), RAND_SCALE(1,200));
                break;
            case DO_DELETE:
                ret = DELETE(RAND_SCALE(1,200));
                break;
            case DO_SEARCH:
                ret = SEARCH(RAND_SCALE(1,200));
                break;
            default:
                break;
        }
    }
    if(0 > ret) puts("----");
    return NULL;
}

int part1_execute(int size) {
    /*PUSH values into driver*/
    /*Reference: pthread_create manual page*/
    struct Thread_Info* tinfo = calloc(ThreadNum, sizeof(struct Thread_Info));
    char* msg = (char*) malloc(StrSize);
    int ret = 0;
    
    for(int i = 0; i < ThreadNum; i++) {
        tinfo[i].thread_num = i+1;

        if(0 != (ret = pthread_create(&tinfo[i].thread_id, NULL, &thread_exec, &tinfo[i]))) {
            pthread_setschedprio(tinfo[i].thread_id, 10+i);
            snprintf(msg, StrSize, "create #%d thread: ERROR",(unsigned int) tinfo[i].thread_id);
            handle_error_en(ret, msg);
        }
    }

    for(int i = 0; i < ThreadNum; i++) {
        pthread_join(tinfo[i].thread_id, NULL);
    }
    return 0;
}


int fill_hash_table(int size) {
    for(int i = 0; i < size; i++) {
       write_hash_atomic(RAND_SCALE(1, 200), RAND_SCALE(1,200)); 
    }
    return 0;
}

int main(int argc, char*argv[]) {
    struct dump_org; 
    dev_fd = open("/dev/ht530_drv", O_RDWR);
    unsigned long hash_cur_size = 0;
    ht_object_t* dump_record = NULL;
    srand(time(NULL));
    if(dev_fd < 0) {
        puts("-----fail-----\n");
    }
    part1_execute(200);
    int i = 0;
//    for() { }
    fill_hash_table(HASH_INIT_PUSH_SIZE);

    ioctl(dev_fd, RET_CUR_SIZE, &hash_cur_size);
    printf("CURRENT SIZE:%lu\n", hash_cur_size);
    dump_record = (ht_object_t*) malloc(sizeof(ht_object_t)*hash_cur_size);
    //ioctl(dev_fd, ALL_DUMP, dump_record);
    ioctl(dev_fd, ALL_DUMP, dump_record);
    for(i = 0; i < hash_cur_size; i++) {
        printf("i:%d,(%d,%d)\n",i, dump_record[i].key, dump_record[i].data);
    }


    //dump_set.n = 0;
    //ioctl(dev_fd, HASH_DUMP,(unsigned long) &dump_set);

    return 0;
    
}
