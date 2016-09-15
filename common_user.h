#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
