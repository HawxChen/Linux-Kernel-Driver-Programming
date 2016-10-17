#define HASH_DUMP       (2020)
#define RET_CUR_SIZE    (2021)
#define ALL_DUMP        (2022)
#define MAX_DUMP  (8)

typedef struct ht_object {
    int key;
    int data;
} ht_object_t;

struct dump_org {
    int n;
    ht_object_t  object_array[MAX_DUMP];
};
