#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<string.h>
#include"mprobe_common.h"
#include"mprobe_user.h"

static int mprobe_fd;
static int ht530_fd;
char buff[1024];
struct debug_request req1 = {
    .of_line = 0xa0,     //break at fsync
    .of_local = -4,    //print xmen3 in ht530_fsync 
    .of_gbl = 0x440,   //print cur_size @ht530.c
};

struct debug_request req2 = {
    .of_line = 0x0,  
    .of_local = -8,    //print supermen2 in ht530_fsync 
    .of_gbl = 0,        //print Ht530_devnum @ht530.c
};
char*fp_bss = "/sys/module/ht530/sections/.bss";
char*fp_text = "/sys/module/ht530/sections/.text";
char buf[1024];

char* get_sect_addr(char*fp) {
    int fd = open(fp, O_RDONLY);
    memset(buf, 0, 1024);
    read(fd, buf,1024);
    puts(buf);
    close(fd);
    return buf;
}

int get_module_sections(struct debug_request* req) {
    sscanf(get_sect_addr(fp_text),"0x%llx", &(req->sect.text));
    sscanf(get_sect_addr(fp_bss),"0x%llx", &(req->sect.bss));
    printf("text:%llx\n",req->sect.text);
    printf("bss:%llx\n",req->sect.bss);
    return 0;
}

struct debug_result items[RING_SIZE];
int main(int argc, char*argv[]) {
    int ret_val = 0;
    mprobe_fd = open("/dev/mprobe", O_RDWR);
    ht530_fd = open("/dev/ht530_drv", O_RDWR);
    get_module_sections(&req1);

    write(mprobe_fd,&req1,sizeof(struct debug_request));
    fsync(ht530_fd);
    if( 0 > (ret_val = read(mprobe_fd,items,sizeof(items)))){
       printf("errno:%d\n" , errno); 
       handle_error("XXXXX");
    }
    for(int i = 0; i < ret_val/sizeof(struct debug_request); i++) {
        printf("items[%d].xtc:%llx\n", i, items[i].xtc);
    }



    close(mprobe_fd);
    close(ht530_fd);

    return 0;
    
}
