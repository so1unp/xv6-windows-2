void thread_init(void);
void thread_schedule(void);
void thread_create(void (*func)());
void thread_yield(void);
void thread_exit(void);
int thread_id(void);
