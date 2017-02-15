#include"common_user.h"
#define DO_ADD (1)
#define DO_DELETE (2)
#define DO_SEARCH (3)

#define ADD(K,V) (write_hash_atomic((K),(V)))
#define DELETE(K) (write_hash_atomic((K),0))
#define SEARCH(K) (read_hash((K)))
#define RAND_SCALE(MIN, MAX) ((rand() % ((MAX) +1 -(MIN))) + (MIN))
#define HASH_INIT_PUSH_SIZE (200)
#define ThreadNum (4)
#define StrSize (100)
#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
