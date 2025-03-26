#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthreadlib.h"

static int numexecs = 0;

static void 
mythread(void)
{
  int i;
  printf(1, "Thread 0x%x started!\n", thread_id());
  for (i = 0; i < numexecs; i++) {
    printf(1, "Thread 0x%x: %d\n", thread_id(), i);
    thread_yield();
  }
  printf(1, "Thread 0x%x ended!\n", thread_id());
  thread_exit();
}

int 
main(int argc, char *argv[]) 
{
  if (argc != 3) {
      printf(2, "%s numthreads numexecs\n", argv[0]);
      exit();
  }

  int numthreads = atoi(argv[1]);
  numexecs = atoi(argv[2]);
  
  thread_init();

  int i;
  for (i = 0; i < numthreads; i++) {
    thread_create(mythread);
  }

  thread_schedule();

  printf(1, "all threads ended.\n");
  exit();
}
