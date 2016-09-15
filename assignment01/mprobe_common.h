#define RING_SIZE (10)
struct module_sections {
    unsigned long long bss;
    unsigned long long text;
};

struct debug_request {
    unsigned long of_line;
    unsigned int of_local;
    unsigned int of_gbl;
    struct module_sections sect;
};

struct debug_result {
    unsigned long addr;
    unsigned long pid;
    unsigned long long xtc;
    int g_var;
    int local_var;
};
