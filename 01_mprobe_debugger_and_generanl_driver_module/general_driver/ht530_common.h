#include<linux/ioctl.h>
#define DUMP_MAGIC      ('D')
#define ALL_DUMP        _IOWR(DUMP_MAGIC, 1, int*)
#define HASH_DUMP       _IOWR(DUMP_MAGIC, 2, int*)
#define RET_CUR_SIZE    _IOWR(DUMP_MAGIC, 3, int*)
#define MAX_DUMP  (8)

typedef struct ht_object {
    int key;
    int data;
} ht_object_t;

struct dump_org {
    int n;
    ht_object_t  object_array[MAX_DUMP];
};
